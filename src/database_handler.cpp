#include "sqlite3.h"
#include "database_handler.hpp"
#include "urls.hpp"
#include <iostream>
#include <sstream>
#include <vector>

void database_handler::initialize_database(){
	char* error_msg;
	sqlite3_exec(db, sql_create_stms_1, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_2, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_3, NULL, 0, &error_msg);
	sqlite3_exec(db, sql_create_stms_4, NULL, 0, &error_msg);
	if(error_msg != NULL){
	}
}

void database_handler::open_database(){
	sqlite3_open("database.sqlite", &db);
}

void database_handler::close_database(){
	sqlite3_close(db);
}

int database_handler::insert_url(const char* url,const char* method) {
	char* error_message = NULL;
	std::string stmt;

	stmt += "INSERT INTO URLS (URL, METHOD) VALUES('";
	stmt +=	url; 
	stmt += "','"; 
	stmt += method;
	stmt += "');";

	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		// std::cout << error_message << "\n";
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
		// std::cout << error_message << "\n" ;
		return -1;
	}

	return 1;
}

int database_handler::insert_error(const char* error_type,const char* injection_value, int url_id,const char* tool_name,const char* response){
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
		// std::cout << error_message << "\n" ;
		return -1;
	}

	return 1;
}

int database_handler::insert_parameter(const char* parameter_name, int url_id) {
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
		// std::cout << error_message << "\n" ;
		return -1;
	}
	return 1;
}

urls* database_handler::select_url(const char* url){
	sqlite3_stmt* sqlite_stmt;
	std::string stmt;
	stmt += "SELECT * FROM URLS WHERE URL='";
	stmt += url;
	stmt += "';";

	if(sqlite3_prepare(db, stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK){
		if(sqlite3_step(sqlite_stmt) == SQLITE_ROW){
			urls* url = new urls();
			url->url_id = sqlite3_column_int(sqlite_stmt, 0);
			url->url = (char*) sqlite3_column_text(sqlite_stmt, 1);
			url->method = (char*) sqlite3_column_text(sqlite_stmt, 2);
			url->parent_url_id = sqlite3_column_int(sqlite_stmt, 3);
			return url;
		}
		sqlite3_finalize(sqlite_stmt);
	}

	return NULL;
}
database_handler::~database_handler() {
}