#include "./src/wapiti_parser.h"
#include "./src/sqlite3.h"
#include "./src/database_handler.hpp"
#include <iostream>

int main()
{
	// std::cout << "0" << "\n";
	char asd[] = "vulnerabilities.xml";
	database_handler* db = new database_handler();
	
	// std::cout << "1";
	db->open_database();
	// std::cout << "2";
	db->initialize_database();
	// std::cout << "3";
	db->insert_url("asdasdasd", "asdasdasd");
	// std::cout << "4";
	db->insert_url_relationship(1, 2);

	db->insert_error("asdasd", "asdasdSAD", 123123, "asdasdasd", "asdasdasd");

	db-> insert_parameter("asdasd", 12);
	db->close_database();

	delete db;

	return 1;
}
