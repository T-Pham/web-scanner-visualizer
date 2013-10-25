#include "pugixml.hpp"
#include "database_handler.hpp"
#include "wapiti_parser.h"
#include <iostream>
#include <string>

const char *WAPITI_APP_NAME = "WAPITI";

int wapiti_parse(const char* filename, database_handler* db_handler) {
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node bugTypeList = doc.child("report").child("bugTypeList");

	pugi::xml_node bugType = bugTypeList.child("bugType");

	db_handler->begin_transaction();

	while(bugType) {
		const char* bug_type = bugType.first_attribute().value();
		pugi::xml_node bugList = bugType.child("bugList");
		while(bugList) {
			pugi::xml_node bug = bugList.child("bug");
			while(bug){
				const char* bug_level = bug.attribute("level").value();
				const char* injection_value = bug.child("parameter").first_child().value();
				const char* info = bug.child("info").first_child().value();

				insert_error_with_url(bug.child("url").first_child().value(), bug_type, bug_level, injection_value, WAPITI_APP_NAME, info, db_handler);

				bug = bug.next_sibling("bug");
			}
			bugList = bugList.next_sibling("bugList");
		}
		bugType = bugType.next_sibling("bugType");
	}

	db_handler-> commit_transaction();

	return 1;
}

int wapiti_tree_parse(const char* filename, database_handler* db_handler){
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node browedURLS = doc.child("root").child("browsed");

	pugi::xml_node URL = browedURLS.child("url_data");

	db_handler->begin_transaction();
	while(URL) {
		std::string str;
		str += URL.first_attribute().value();
		int n = str.find("?");

		if(n >= 0) {
			str = str.substr(0, str.find("?"));
		}

		db_handler->insert_url(str.c_str(), "GET");
		URL = URL.next_sibling("url_data");
	}

	pugi::xml_node form = doc.child("root").child("forms").child("form");

	while(form) {
		std::string str;
		str += form.attribute("url").value();

		int n = str.find("?");
		if(n >= 0) {
			str = str.substr(0, str.find("?"));
		}

		db_handler->insert_url(str.c_str(), "POST");
		form =  form.next_sibling("form");
	}
	db_handler->commit_transaction();

	db_handler->update_parent_url();

	return 1;
}
