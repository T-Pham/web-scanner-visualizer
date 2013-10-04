#include "pugixml.hpp"
#include "database_handler.hpp"
#include <iostream>
#include <string>

int wapiti_parse(char* filename) {
	pugi::xml_document doc;

	if(!doc.load_file(filename)) {
		return -1;
	}

	pugi::xml_node bugTypeList = doc.child("report").child("bugTypeList");

	pugi::xml_node bugType = bugTypeList.child("bugType");
	while(bugType) {
		pugi::xml_node bugList = bugType.child("bugList");
		while(bugList) {
			pugi::xml_node bug = bugList.child("bug");
			while(bug){
				//std::cout << bug.child("url").first_child().value() << "\n";
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
		db->insert_url((char *) URL.attribute("uri").value(), NULL);
		URL = URL.next_sibling("url_data");
	}
}

