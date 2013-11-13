#include "./src/database_handler.hpp"
#include "./src/wapiti_parser.hpp"
#include "./src/arachni_parser.hpp"
#include "./src/w3af_parser.hpp"
#include "./src/json_creator.hpp"
#include <iostream>
#include <string.h>
#include <unistd.h>

// Constants
char *FILE_FOR_WAPITI_TREE_PARSE = "testphp.vulnweb.com.xml";
char *FILE_FOR_WAPITI_PARSE = "vulnerabilities.xml";
char *FILE_FOR_W3AF_PARSE = "W3afReport.xml";
char *FILE_FOR_ARACHNI_PARSE = "my_report.xml";

// Function Prototypes
void start_database();
void finalize_database();

void parse_wapiti();
void parse_w3af();
void parse_arachni();

// Variables
database_handler* db;

// main
int main(int n, char* args[])
{
	// parsing arguments for file paths       
	int option;
	while ((option = getopt(n, args, "t:w:s:a:")) != -1) {
		switch (option) {
		case 't':
			FILE_FOR_WAPITI_TREE_PARSE = optarg;
			break;
		case 'w':
			FILE_FOR_WAPITI_PARSE = optarg;
			break;
		case 'f':
			FILE_FOR_W3AF_PARSE = optarg;
			break;
		case 'a':
			FILE_FOR_ARACHNI_PARSE = optarg;
			break;
		case '?':
			if ((optopt == 't') || (optopt == 'w') || (optopt == 's') || (optopt == 'a'))
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			break;
		default:
			break;
		}
	}

	start_database();
	//parse_wapiti();
	parse_w3af();
	//parse_arachni();
	finalize_database();

	create_json();
	return 0;
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
	if(wapiti_tree_parse(FILE_FOR_WAPITI_TREE_PARSE, db) == -1) {
		std::cout << "FILE FOR WAPITI TREE PARSE NOT FOUND" << std::endl;
	}

	if(wapiti_parse(FILE_FOR_WAPITI_PARSE, db) == -1) {
		std::cout << "FILE FOR WAPITI PARSE NOT FOUND " << std::endl;
	}
}

void parse_w3af() {
	if (w3af_parse(FILE_FOR_W3AF_PARSE, db) == -1) {
		std::cout << "FILE FOR W3AF PARSE NOT FOUND" << std::endl;
	}
}

void parse_arachni() {
	if(arachni_parse(FILE_FOR_ARACHNI_PARSE, db) == -1) {
		std::cout << "FILE FOR ARACHNI PARSE NOT FOUND" << std::endl;
	}
}
