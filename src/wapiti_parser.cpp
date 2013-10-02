#include "pugixml.hpp"
#include <iostream>
#include <string>
#include "sqlite3.h"

const char* sql_stms = "DROP TABLE IF EXISTS URLS;\
					   CREATE TABLE URLS (\
					   URL_ID INTEGER PRIMARY KEY,\
					   URL TEXT NOT NULL,\
					   METHOD VARCHAR(255) NOT NULL,\
					   URL_PARENT_ID INTEGER \
					   );\
					   DROP TABLE IF EXISTS PARAMETERS;\
					   CREATE TABLE PARAMETERS (\
					   PARAMETER_ID INTEGER PRIMARY KEY,\
					   PARAMETER_NAME VARCHAR(255) NOT NULL,\
					   URL_ID INTEGER NOT NULL\
					   );\
					   DROP TABLE IF EXISTS ERRORS;\
					   CREATE TABLE ERRORS (\
					   ERROR_ID INTEGER PRIMARY KEY,\
					   ERROR_TYPE VARCHAR(255) NOT NULL,\
					   INJECTION_VALUE TEXT,\
					   URL_ID INTEGER NOT NULL,\
					   TOOL_NAME VARCHAR(255) NOT NULL,\
					   RESPONSE TEXT\
					   );";

void database_prepare(){
	sqlite3* db;
	char* error_msg;

	sqlite3_open("myDatabase", &db);
	sqlite3_exec(db, sql_stms, NULL, 0, &error_msg);
	sqlite3_close(db);
}

int wapiti_parse(char filename[]) {
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

