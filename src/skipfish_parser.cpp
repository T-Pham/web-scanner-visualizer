#include "database_handler.hpp"
#include "skipfish_parser.hpp"
#include "helper.hpp"
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
	std::string line;
	std::ifstream file;

	db->open_database();
	db->begin_transaction();

	file.open(filename);
	getline(file, line);

	if(!file.is_open()) {
		return -1;
	}

	while(strcmp("var issue_samples = [", line.c_str())) {
		getline(file, line);
	}
	getline(file, line);

	while(strcmp("  { 'severity':", line.substr(0, 15).c_str()) == 0) {
		std::string severity_level (line.substr(16, 1).c_str());
		std::string type (line.substr(27, 5).c_str()); 
		bool skip = false;

		if(!type.compare(XSS_ERROR1) || !type.compare(XSS_ERROR2) || !type.compare(XSS_ERROR3)) {
			type = "Cross Site Scripting";
		}
		else {
			if(!type.compare(SQLI_ERROR1) || !type.compare(SQLI_ERROR2)) {
				type = "SQL Injection";
			}
			else {
				skip = true;
			}
		}

		getline(file, line);

		while(strcmp("    { 'url':", line.substr(0, 12).c_str()) == 0) {
			if(!skip) {
				line.erase(0, 14);
				int n = line.find_first_of("'");

				insert_error_with_url(line.substr(0, n).c_str(), type.c_str(), severity_level.c_str(), "", SKIPFISH_APP_NAME, "", db);
			}
			getline(file, line);
		}

		if(strcmp("},", line.substr(0, 4).c_str())) {
			getline(file, line);
		}
	}

	file.close();
	db->commit_transaction();
	db->close_database();
	return 1;
}