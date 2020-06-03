#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include "bsoncxx/json.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/collection.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

bsoncxx::document::value creaDocumento(std::string nombre, int rating, int duracion, int x, int y);

int main()
{
	//Cargar el driver
	mongocxx::instance instance{}; // This should be done only once.

	//Conectarse a mongo
	mongocxx::uri uri("mongodb://localhost:27017");
	mongocxx::client client(uri);

	//Acceder a una base de datos
	mongocxx::database db = client["cinema"];

	//Acceder a una colección
	mongocxx::collection pelis = db["movies"];

	//Borramos un documento, el documento que cumple el criterio {"name":"Vida de Ryan"}
	pelis.delete_one(document{} << "name" << "Vida de Ryan"<< finalize);

	//Borramos varios documentos usando una regex
	bsoncxx::stdx::optional<mongocxx::result::delete_result> resultado =
		pelis.delete_many(document{} << "name" << open_document << "$regex"<< "Vida de.*" << close_document << finalize);

	if (resultado) {
		std::cout << resultado->deleted_count() << "\n";
	}

	//Creamos un documento
	auto documento=creaDocumento("Vida de Ryan", 4, 90, 3, 4);

	//Podemos acceder al documento
	//1. Crea una vista
	bsoncxx::document::view vista = documento.view();
	//2. Accede a uno de los elementos del documento
	bsoncxx::document::element element = vista["name"];
	if (element.type() != bsoncxx::type::k_utf8) {
		cout << "Error" << endl;
		return -1;
	}
	string name = element.get_utf8().value.to_string();
	cout << name << endl;

	//Inserta el documento
	bsoncxx::stdx::optional<mongocxx::result::insert_one> result =pelis.insert_one(documento.view());

	//Inserta varios documentos
	std::vector<bsoncxx::document::value> documentos;
	for (int i = 0; i < 20; i++) {
		documentos.push_back(creaDocumento("Vida de Ryan "+std::to_string(i), 4, i+10, 3, 4));
	}
	pelis.insert_many(documentos);

	mongocxx::cursor cursor = pelis.find(document{} << "name" << open_document << "$regex" << "Vida de.*" << close_document << finalize);
	for (auto doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << "\n";
	}

	//Consulta un documento
	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
		pelis.find_one(document{} << "name" << "Vida de Ryan" << finalize);
	if (maybe_result) {
		cout<<maybe_result.get().view().data()<<endl;
		// Do something with *maybe_result;
	}

	//Consulta varios documentos
	mongocxx::cursor cur = pelis.find(
		document{} << "rating" << open_document <<"$gt" << 2 <<"$lte" << 4<< close_document << finalize);
	for (auto doc : cur) {
		std::cout << bsoncxx::to_json(doc) << "\n";
	}

	//Actualiza un documento
	pelis.update_one(document{} << "name" << "Vida de Ryan"<< finalize,
		document{} << "$set" << open_document << "rating" << 5 << close_document << finalize);

	//Actualiza varios documentos
	bsoncxx::stdx::optional<mongocxx::result::update> resultado_update =
		pelis.update_many(
			document{} << "rating" << open_document <<"$gt" << 4 << close_document << finalize,
			document{} << "$inc" << open_document << "rating" << 1 << close_document << finalize);

	if (resultado_update) {
		std::cout << resultado_update->modified_count() << "\n";
	}

	return 0;
}

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