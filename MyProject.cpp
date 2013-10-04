#include "./src/wapiti_parser.h"
#include "./src/sqlite3.h"
#include "./src/database_handler.hpp"
#include <iostream>

int main()
{
	// std::cout << "0" << "\n";
	char asd[] = "testphp.vulnweb.com.xml";
	database_handler* db = new database_handler();

	// std::cout << "1";
	db->open_database();
	// std::cout << "2";
	db->initialize_database();

	if(wapiti_tree_parse(asd, db) == -1){
		std::cout << "Error opening xml file \n";
	}

	db->close_database();

	delete db;

	return 1;
}
