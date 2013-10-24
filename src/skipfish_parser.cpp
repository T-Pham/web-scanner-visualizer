#include "pugixml.hpp"
#include "database_handler.hpp"
#include "skipfish_parser.hpp"
#include "wapiti_parser.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>

const char * SKIPFISH_APP_NAME = "SKIPFISH";
const char * XSS_ERROR1 = "40101";
const char * XSS_ERROR2 = "40102";
const char * XSS_ERROR3 = "40105";
const char * SQLI_ERROR1 = "50103";
const char * SQLI_ERROR2 = "50201";

int skipfish_parse(const char* filename, database_handler* db) {
	std::string input;
	std::ifstream file;

	db->open_database();
	db->begin_transaction();

	file.open(filename);
	getline(file, input);
	while(strcmp("var issue_samples = [", input.c_str())) {
		getline(file, input);
	}
	getline(file, input);

	while(strcmp("  { 'severity':", input.substr(0, 15).c_str()) == 0) {
		std::string severity_level (input.substr(16, 1).c_str());
		std::string type (input.substr(27, 5).c_str()); 
		bool skip = false;
		std::string string_type;

		if(!type.compare(XSS_ERROR1) || !type.compare(XSS_ERROR2) || !type.compare(XSS_ERROR3)) {
			string_type = "Cross Site Scripting";
		}
		else {
			if(!type.compare(SQLI_ERROR1) || !type.compare(SQLI_ERROR2)) {
				string_type = "SQL Injection";
			}
			else {
				skip = true;
			}
		}

		getline(file, input);

		while(strcmp("    { 'url':", input.substr(0, 12).c_str()) == 0) {
			if(!skip) {
				input.erase(0, 14);
				int n = input.find_first_of("'");
				std::cout << input.substr(0, n).c_str() << " || "  << string_type.c_str() << " || "  << severity_level << std::endl;	
				insert_error_with_url(input.substr(0, n).c_str(), string_type.c_str(), severity_level.c_str(), "", SKIPFISH_APP_NAME, "", db);
			}
			getline(file, input);
		}

		if(strcmp("},", input.substr(0, 4).c_str())) {
			getline(file, input);
		}
	}

	file.close();
	db->commit_transaction();
	db->close_database();
	return 1;
}