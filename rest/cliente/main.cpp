#include <iostream>
#include "miServicio.h"

int main()
{
    std::cout << "Mi primer servicio Rest en C!\n";

    //Puntero a un output stream
    auto fileStream = std::make_shared<ostream>();

    //Crea un puntero - open_ostream - que apunta a un archivo llamado results.html
    //open_stream devuelve un stream, y aplicamos un lambda. Pasamos por valor - [=] - el puntero al lambda
    pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
        {
            //Guarda el puntero
            *fileStream = outFile;

            //Crea un http cliente que apunta a google
            http_client client(U("http://www.google.com/"));

            // Creamos un builder para construir la uri. La uri es /search...
            uri_builder builder(U("/search"));
            //con un query paramter ?q=cpprestsdk github
            builder.append_query(U("q"), U("messi"));
            //Hace un get a la uri
            return client.request(methods::GET, builder.to_string());
        })
        //Encadenamos con otro lambda. Pasamos por valor - [=] - la respuesta que hemos obtenido
            .then([=](http_response response)
                {
                    printf("El http status code:%u que hemos recibido\n", response.status_code());

                    //Leemos el body, y lo devolvemos
                    return response.body().read_to_end(fileStream->streambuf());
                })
            //Encadena con otro lambda
                    .then([=](size_t)
                        {
                            return fileStream->close();
                        });

        // No terminamos hasta que no hallan terminado las tareas
        try
        {
            requestTask.wait();
        }
        catch (const std::exception& e)
        {
            printf("Excepcion: %s\n", e.what());
        }

        return 0;
}