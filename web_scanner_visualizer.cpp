#include "./src/wapiti_parser.h"
#include "./src/sqlite3.h"
#include "./src/database_handler.hpp"
#include "./src/skipfish_parser.hpp"
#include "./src/json_creator.hpp"

// Constants
const char *FILE_FOR_WAPITI_TREE_PARSE = "testphp.vulnweb.com.xml";
const char *FILE_FOR_WAPITI_PARSE = "vulnerabilities.xml";
const char *FILE_FOR_SKIPFISH_PARSE = "samples.js";

// Functions
void start_database();
void finalize_database();

void parse_wapiti();
void parse_skipfish();

// Variables
database_handler* db;

// main
int main()
{
	start_database();
	parse_wapiti();
	parse_skipfish();
	finalize_database();
	create_json();
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
	wapiti_tree_parse(FILE_FOR_WAPITI_TREE_PARSE, db);
	wapiti_parse(FILE_FOR_WAPITI_PARSE, db);
}

void parse_skipfish() {
	skipfish_parse(FILE_FOR_SKIPFISH_PARSE, db);
}
