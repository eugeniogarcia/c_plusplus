# Crea una imagen para compilar

## Obtiene la imagen base

```ps
docker pull ubuntu:bionic
```

```ps
docker images
```

## Crea nuestra imagen 

Usaremos este docker file. En la imagen vamos a crear:

- Utilidades: git, zip, curl, etc.
- SSH. Para poder conectarnos remotamente desde Visual Studio, openssh-server
- Herramientas de C: g++, make, gdb
- Linreria para construir servicios rest: libcpprest-dev

```txt
# our local base image
FROM ubuntu:bionic 

LABEL description="Container for use with Visual Studio" 

# install build dependencies 
RUN apt-get update && apt-get install -y g++ rsync zip openssh-server make gdb vim curl

# Instala la libreria para implementar REST services 
RUN apt-get install -y libcpprest-dev

# configure SSH for communication with Visual Studio 
RUN mkdir -p /var/run/sshd

RUN echo 'PasswordAuthentication yes' >> /etc/ssh/sshd_config && \ 
   ssh-keygen -A 

# expose port 22 8080 8081
EXPOSE 22 8080 8081
```

```ps
docker build -t ubuntu-vs .
```

## Ejecutar la imagen

```ps
docker run -p 5000:22 -i -t ubuntu-vs /bin/bash
```

```ps
service ssh start
useradd -m -d /home/egsmartin -s /bin/bash -G sudo egsmartin
passwd egsmartin
```

Habilitamos ssh en el arranque

```ps
systemctl enable ssh
```

Editar `/etc/ssh/sshd_config` y comprobar que figuren estas líneas, o sino añadirlas:

```txt
Ciphers aes256-cbc,aes192-cbc,aes128-cbc,3des-cbc
HostKeyAlgorithms ssh-rsa
KexAlgorithms diffie-hellman-group-exchange-sha256,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1
MACs hmac-sha2-256,hmac-sha1
```

```ps
service ssh restart
```

### Automtización

Vamos a automatizar la configuracion para que no haya que hacer ningun paso manual cuando creemos la imagen. Como parte de la automatización vamos a crear un argumento, de modo que podamos especificar la contraseña del usuario que vamos a crear en la imagen. La contraseña se usara para hacer ssh.

```ps
docker build --build-arg PW="test" -t ubuntu-vs -f dockerfile . 
```

### Utilidades

Borrar contenedores

```ps
docker ps --all -q|%{docker rm -f $_}
```

Ejecutar imagen con un entrypoint diferente:

```ps
docker run -p 5000:22 -i -t --entrypoint /bin/bash ubuntu-vs
```

Ejecuta la imagen

```ps
docker run -p 5000:22 -i -t ubuntu-vs
```

## Visual Studio

Tenemos que seguir estos pasos para configurar Visual Studio con nuestra imagen:

- Tools -> Options
- Opcion `Cross Platform`, seleccionar `Connection Manager` y añadir nuestra imagen


g++ -std=c++11 main.cpp -o my_file -lboost_system -lcrypto -lssl -lcpprest