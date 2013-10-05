#include "./src/wapiti_parser.h"
#include "./src/sqlite3.h"
#include "./src/database_handler.hpp"
#include <iostream>

int main()
{
	// std::cout << "0\n";

	database_handler* db = new database_handler();
	
	// std::cout << "1\n";
	db->open_database();
	// std::cout << "2\n";
	db->initialize_database();

	wapiti_tree_parse("testphp.vulnweb.com.xml", db);

	wapiti_parse("vulnerabilities.xml", db);

	db->close_database();

	delete db;

	return 1;
}
