#include <sstream>
#include "sqlite3.h"
#include "database_handler.hpp"
#include <iostream>

void database_handler::initialize_database(){
	char* error_msg;
	sqlite3_exec(db, sql_create_stms_1, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_2, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_3, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_4, NULL, 0, &error_msg);
}

void database_handler::open_database(){
	sqlite3_open("database.sqlite", &db);
}

void database_handler::close_database(){
	sqlite3_close(db);
}

int database_handler::insert_url(char* url, char* method) {
	char* error_message = NULL;
	std::string stmt;

	stmt += "INSERT INTO URLS (URL, METHOD) VALUES('";
	stmt +=	url; 
	stmt += "','"; 
	stmt += method;
	stmt += "');";
	
	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		std::cout << error_message << "\n";
		return -1;
	}

	return 1;
}

int database_handler::insert_url_relationship(int parent_url_id, int child_url_id) {
	char* error_message = NULL;
	std::stringstream ss1;
	std::stringstream ss2;
	std::string stmt;
	ss1 << parent_url_id;
	ss2 << child_url_id;
	
	stmt += "INSERT INTO URL_RELATIONSHIPS (PARENT_URL_ID, CHILD_URL_ID) VALUES('";
	stmt += ss1.str();
	stmt += "','";
	stmt += ss2.str();
	stmt += "');";

	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		std::cout << error_message << "\n" ;
		return -1;
	}

	return 1;
}

int database_handler::insert_error(char* error_type, char* injection_value, int url_id, char* tool_name, char* response){
	char* error_message = NULL;
	std::stringstream s_url_id;
	std::string stmt;
	s_url_id << url_id;
	
	stmt += "INSERT INTO ERRORS (ERROR_TYPE, INJECTION_VALUE, URL_ID, TOOL_NAME, RESPONSE) VALUES('";
	stmt += error_type;
	stmt += "','";
	stmt += injection_value;
	stmt += "',";
	stmt += s_url_id.str();
	stmt += ",'";
	stmt += tool_name;
	stmt += "','";
	stmt += response;
	stmt += "');";

	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		std::cout << error_message << "\n" ;
		return -1;
	}

	return 1;
}

int database_handler::insert_parameter(char* parameter_name, int url_id) {
	char* error_message = NULL;
	std::stringstream s_url_id;
	std::string stmt;
	s_url_id << url_id;
	
	stmt += "INSERT INTO PARAMETERS (PARAMETER_NAME, URL_ID) VALUES('";
	stmt += parameter_name;
	stmt += "',";
	stmt += s_url_id.str();
	stmt += ");";

	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		std::cout << error_message << "\n" ;
		return -1;
	}
	return 1;
}

database_handler::~database_handler() {
}