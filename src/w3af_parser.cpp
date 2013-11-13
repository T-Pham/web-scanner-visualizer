#include "pugixml.hpp"
#include "database_handler.hpp"
#include "base64decoder.hpp"
#include "w3af_parser.hpp"
#include "helper.hpp"
#include <string>
#include <string.h>
#include <iostream>


int w3af_parse(const char* filename, database_handler* db_handler) {
	pugi::xml_document doc;

	if (!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node bug = doc.child("w3afrun").child("vulnerability");

	db_handler->begin_transaction();

	while (bug) {
		const char* url = bug.attribute("url").value();
		const char* error_type = bug.attribute("name").value();
		const char* severity = bug.attribute("severity").value();
		const char* injected_param = bug.attribute("var").value();
		const char* method = bug.attribute("method").value();
		
		char* bug_level = "3";

		std::string str;
		str += error_type;

		if (str.find(W3AF_XSS_PATTERN) != std::string::npos) {
			error_type = "Cross Site Scripting";
		}
		else {
			if (str.find(W3AF_SQLI_PATTERN) != std::string::npos) {
				error_type = "SQL Injection";
			}
			else {
				bug = bug.next_sibling("vulnerability");
				continue;
			}
		}

		if (!strcmp(severity, "High")) {
			bug_level = "1";
		}
		else {
			if (!strcmp(severity, "Medium")) {
				bug_level = "2";
			}
			else {
				if (!strcmp(severity, "Low")) {
					bug_level = "3";
				}
			}
		}

		const char* injection_value;

		if (!strcmp(method, "GET")) {
			std::string status(bug.child("http-transactions").child("http-transaction").child("httprequest").child("status").child_value());
			int n = status.find("GET ") + 4;
			int k = status.find(" HTTP") - 4;

			// get injected value from status tag
			std::string tmp1(status.substr(n, k));
			std::string tmp2(tmp1.substr(tmp1.find("?") + 1, std::string::npos));

			injection_value = tmp2.c_str();
			insert_error_with_url(url, method, injected_param, error_type, bug_level, injection_value, W3AF_APP_NAME, "", db_handler);
		}
		else {
			std::string base64_text(bug.child("http-transactions").child("http-transaction").child("httprequest").child("body").child_value());
			std::vector<BYTE> vec;

			// decode base64 string
			vec = base64_decode(base64_text);
			std::string tmp(vec.begin(), vec.end());

			injection_value = tmp.c_str();

			insert_error_with_url(url, method, injected_param, error_type, bug_level, injection_value, W3AF_APP_NAME, "", db_handler);
		}

		bug = bug.next_sibling("vulnerability");
	}

	db_handler->commit_transaction();

	return 1;
}