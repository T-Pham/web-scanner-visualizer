#include "database_handler.hpp"
#include "skipfish_parser.hpp"
#include "helper.hpp"
#include <fstream>
#include <string.h>

int skipfish_parse(const char* filename, database_handler* db_handler) {
	std::string line;
	std::ifstream file;

	db_handler->begin_transaction();

	file.open(filename);

	if(!file.is_open()) {
		return -1;
	}

	getline(file, line);

	const char* issue_line_prefix = "var issue_samples = [";
	while(strncmp(line.c_str(), issue_line_prefix, strlen(issue_line_prefix))) {
		getline(file, line);
		
		if (file.eof()) {
			break;
		}
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

		if (file.eof()) {
			break;
		}

		while(strcmp("    { 'url':", line.substr(0, 12).c_str()) == 0) {
			if(!skip) {
				line.erase(0, 14);
				int n = line.find_first_of("'");

				insert_error_with_url(line.substr(0, n).c_str(), "POST", type.c_str(), severity_level.c_str(), "", SKIPFISH_APP_NAME, "", db_handler);
			}
			getline(file, line);

			if (file.eof()) {
				break;
			}
		}

		if(strcmp("},", line.substr(0, 4).c_str())) {
			getline(file, line);

			if (file.eof()) {
				break;
			}
		}
	}

	file.close();
	db_handler->commit_transaction();
	
	return 1;
}