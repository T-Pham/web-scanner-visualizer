#include "./src/wapiti_parser.h"
#include "./src/sqlite3.h"
#include "./src/database_handler.hpp"
#include <iostream>

// Functions
void start_database();
void finalize_database();

void parse_wapiti();

// Variables
database_handler* db;

// main
int main()
{
  start_database();
	parse_wapiti();
  finalize_database();
	return 1;
}

// Database functions
void start_database() {
	db = new database_handler();
	db->open_database();
	db->initialize_database();
}

void finalize_database() {
  db->close_database();
	delete db;
}

// Parsers
void parse_wapiti() {
  wapiti_tree_parse("testphp.vulnweb.com.xml", db);
	wapiti_parse("vulnerabilities.xml", db);
}
