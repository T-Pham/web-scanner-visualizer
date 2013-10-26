#include "arachni_parser.hpp"
#include "database_handler.hpp"
#include "helper.hpp"
#include "pugixml.hpp"
#include <string>

const char *ARACHNI_APP_NAME = "ARACHNI";

int arachni_parse (const char* filename, database_handler* db_handler) {
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node issues = doc.child("arachni_report").child("issues");
	pugi::xml_node issue = issues.child("issue");

	while(issue) {
		const char* url = issue.child("url").child_value();
		const char* error_type = issue.child("name").child_value();
		const char* severity = issue.child("severity").child_value();
		const char* injected_param = issue.child("var").child_value();
		const char* injection_value = issue.child("variations").child("variation").child("injected").child_value();

		bool skip = false;
		std::string str;
		str += error_type;

		if(str.find("Cross-Site Scripting") != std::string::npos) {
			error_type = "Cross Site Scripting";
		}
		else {
			if(str.find("SQL Injection") != std::string::npos) {
				error_type = "SQL Injection";
			}
			else {
				skip = true;
			}
		}

		if(!skip) {
			insert_error_with_url(url, injected_param, error_type, "1", injection_value, ARACHNI_APP_NAME, "", db_handler);
		}

		issue = issue.next_sibling("issue");
	}
}