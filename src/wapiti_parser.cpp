#include "pugixml.hpp"
#include "database_handler.hpp"
#include "urls.hpp"
#include <iostream>
#include <string>

int wapiti_parse(char* filename, database_handler* db) {
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node bugTypeList = doc.child("report").child("bugTypeList");

	pugi::xml_node bugType = bugTypeList.child("bugType");
	while(bugType) {
		const char* str_bugType = bugType.first_attribute().value();
		pugi::xml_node bugList = bugType.child("bugList");
		while(bugList) {
			pugi::xml_node bug = bugList.child("bug");
			while(bug){
				const char* url = bug.child("url").first_child().value();
				const char* parameter = bug.child("parameter").first_child().value();
				const char* info = bug.child("info").first_child().value();
				
				std::string url_str;
				url_str += url;
				int n = url_str.find("?");

				if(n >= 0) {
					url_str = url_str.substr(0, n);
				}

				urls* url_row = db->select_url(url_str.c_str());
				if(url_row) {
					db->insert_error(str_bugType, parameter, url_row->url_id, "WAPITI", "");
				}
				else {
					db->insert_error(str_bugType, parameter, -1, "WAPITI", "");
				}
				delete url_row;
				bug = bug.next_sibling("bug");
			}
			bugList = bugList.next_sibling("bugList");
		}
		bugType = bugType.next_sibling("bugType");
	}

	return 1;
}

int wapiti_tree_parse(char* filename, database_handler* db){
	pugi::xml_document doc;
	
	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node browedURLS = doc.child("root").child("browsed");

	pugi::xml_node URL = browedURLS.child("url_data");
	while(URL) {
		std::string str;
		str += URL.first_attribute().value();
		int n = str.find("?");
		
		if(n >= 0) {
			str = str.substr(0, str.find("?"));
		}

		db->insert_url(str.c_str(), "");
		URL = URL.next_sibling("url_data");
	}
}

