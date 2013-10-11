#include "pugixml.hpp"
#include "database_handler.hpp"
#include <iostream>
#include <string>

int wapiti_parse(char* filename, database_handler* db) {
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node bugTypeList = doc.child("report").child("bugTypeList");

	pugi::xml_node bugType = bugTypeList.child("bugType");

	db->begin_transaction();

	while(bugType) {
		const char* str_bugType = bugType.first_attribute().value();
		pugi::xml_node bugList = bugType.child("bugList");
		while(bugList) {
			pugi::xml_node bug = bugList.child("bug");
			while(bug){
				const char* parameter = bug.child("parameter").first_child().value();
				const char* info = bug.child("info").first_child().value();

				std::string url_without_para;
				url_without_para += bug.child("url").first_child().value();
				int n = url_without_para.find("?");

				if(n >= 0) {
					url_without_para = url_without_para.substr(0, n);
				}

				db->insert_error(str_bugType, parameter, url_without_para.c_str(), "WAPITI", "");

				bug = bug.next_sibling("bug");
			}
			bugList = bugList.next_sibling("bugList");
		}
		bugType = bugType.next_sibling("bugType");
	}

	db-> commit_transaction();

	return 1;
}

int wapiti_tree_parse(char* filename, database_handler* db){
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node browedURLS = doc.child("root").child("browsed");

	pugi::xml_node URL = browedURLS.child("url_data");

	db->begin_transaction();
	while(URL) {
		std::string str;
		str += URL.first_attribute().value();
		int n = str.find("?");

		if(n >= 0) {
			str = str.substr(0, str.find("?"));
		}

		db->insert_url(str.c_str(), "GET");
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

		db->insert_url(str.c_str(), "POST");
		form =  form.next_sibling("form");
	}
	db->commit_transaction();

	db->update_parent_url();
}