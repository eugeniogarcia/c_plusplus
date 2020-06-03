# Introducción

Ver la documentación de la api [aqui](http://mongocxx.org/mongocxx-v3/tutorial/)

## Documentos

Los namespaces:

```c
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
```

### Crea un documento

```c
//Crea un documento
bsoncxx::document::value creaDocumento(std::string nombre, int rating, int duracion, int x, int y) {
	auto mi_documento = bsoncxx::builder::stream::document{};

	bsoncxx::document::value doc_value = mi_documento
		<< "name" << nombre
		<< "rating" << rating
		<< "time" << bsoncxx::builder::stream::open_array
		<< duracion << "v3.0" << "v2.6"
		<< close_array
		<< "info" << bsoncxx::builder::stream::open_document
		<< "x" << x
		<< "y" << y
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;

	return doc_value;
}
```

Con `builder` empezamos un documento

```c
	auto mi_documento = bsoncxx::builder::stream::document{};
	
	bsoncxx::document::value doc_value = mi_documento
	
	...
	
	<< bsoncxx::builder::stream::finalize;

```

Es análogo a 

```json
{

...

}
```

Veamos paso a paso lo que estamos haciendo con el contenido del documento si llamamos a `creaDocumento("xxxxx", 4, 90, 2, 3)`

```c
	<< "name" << nombre
	<< "rating" << rating
```

```json
{

"name": "xxxxxx",
"rating": "4"

...

}
```

Siguiente:

```c
	<< "time" << bsoncxx::builder::stream::open_array
	<< duracion << "v3.0" << "v2.6"
	<< close_array
```

Equivale:

```json
{

"name": "xxxxxx",
"rating": "4",
"time":["90","v3.0","v2.6"]

...

}
```

Siguiente:

```c
	<< "info" << bsoncxx::builder::stream::open_document
	<< "x" << x
	<< "y" << y
	<< bsoncxx::builder::stream::close_document
```

Equivale:

```json
{

"name": "xxxxxx",
"rating": "4",
"time":["90","v3.0","v2.6"],
"info":{
	"x":"2",
	"y":"3"
	}
}
```

### Crea un documento anónimo

Esto, aunque no lo parezca, usa la misma técnica que hemos visto antes:

```c
pelis.delete_many(document{} << "name" << open_document << "$regex"<< "Vida de.*" << close_document << finalize);
```

Como hemos definido en el using el namespace, esto:

```c
auto mi_documento = bsoncxx::builder::stream::document{};
```

es igual que:

```c
auto mi_documento = document{};
```

### Ver un documento

Para acceder a los componentes de un documento:

1. Creamos una vista

```c
bsoncxx::document::view vista = documento.view();
```

2. Accede a uno de los elementos del documento

```c
bsoncxx::document::element element = vista["name"];
```

3. Usamos el elemento:

```c
if (element.type() != bsoncxx::type::k_utf8) {
	cout << "Error" << endl;
	return -1;
}
string name = element.get_utf8().value.to_string();
cout << name << endl;
```

### Documento a Json

```c
mongocxx::cursor cursor = pelis.find(document{} << "name" << open_document << "$regex" << "Vida de.*" << close_document << finalize);
for (auto doc : cursor) {
	std::cout << bsoncxx::to_json(doc) << "\n";
}
```