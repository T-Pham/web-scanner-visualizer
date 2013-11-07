#include "./src/database_handler.hpp"
#include "./src/skipfish_parser.hpp"
#include "./src/wapiti_parser.hpp"
#include "./src/arachni_parser.hpp"
#include "./src/json_creator.hpp"
#include <iostream>
#include <string.h>

// Constants
char *FILE_FOR_WAPITI_TREE_PARSE = "testphp.vulnweb.com.xml";
char *FILE_FOR_WAPITI_PARSE = "vulnerabilities.xml";
char *FILE_FOR_SKIPFISH_PARSE = "samples.js";
char *FILE_FOR_ARACHNI_PARSE = "my_report.xml";

// Function Prototypes
void start_database();
void finalize_database();

void parse_wapiti();
void parse_skipfish();
void parse_arachni();

// Variables
database_handler* db;

// main
int main(int n, char* args[])
{
	// parsing arguments for file paths
	
	for(int i = 1; i < n; i++) {
		if(!strcmp(args[i], "-t")) {
			FILE_FOR_WAPITI_TREE_PARSE = args[++i];
		}
		else {
			if(!strcmp(args[i], "-w")) {
				FILE_FOR_WAPITI_PARSE = args[++i];
			}
			else {
				if(!strcmp(args[i], "-s")) {
					FILE_FOR_SKIPFISH_PARSE = args[++i];
				}
				else {
					if(!strcmp(args[i], "-a")) {
						FILE_FOR_ARACHNI_PARSE = args[++i];
					}
				}
			}
		}
	}


	start_database();
	parse_wapiti();
	parse_skipfish();
	parse_arachni();
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
	if(wapiti_tree_parse(FILE_FOR_WAPITI_TREE_PARSE, db) == -1) {
		std::cout << "FILE FOR WAPITI TREE PARSE NOT FOUND" << std::endl;
	}

	if(wapiti_parse(FILE_FOR_WAPITI_PARSE, db) == -1) {
		std::cout << "FILE FOR WAPITI PARSE NOT FOUND " << std::endl;
	}
}

void parse_skipfish() {
	if(skipfish_parse(FILE_FOR_SKIPFISH_PARSE, db) == -1) {
		std::cout << "FILE FOR SKIPFISH PARSE NOT FOUND" << std::endl;
	}
}

void parse_arachni() {
	if(arachni_parse(FILE_FOR_ARACHNI_PARSE, db) == -1) {
		std::cout << "FILE FOR ARACHNI PARSE NOT FOUND" << std::endl;
	}
}
