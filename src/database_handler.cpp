#include "sqlite3.h"
#include "database_handler.hpp"
#include <iostream>
#include <sstream>
#include <string>

int database_handler::initialize_database(){
	char* error_message;
	sqlite3_exec(db, sql_create_stms, NULL, 0, &error_message);

	if(error_message) {
		sqlite3_free(error_message);
		return -1;
	}

	return 1;
}

void database_handler::open_database(){
	sqlite3_open("database.sqlite", &db);
}

void database_handler::close_database(){
	sqlite3_close(db);
}

void database_handler::begin_transaction(){
	char* error_message;
	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, 0, &error_message);

	if(error_message) {
		sqlite3_free(error_message);
	}
}

void database_handler::commit_transaction(){
	char* error_message;
	sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, 0, &error_message);

	if(error_message) {
		sqlite3_free(error_message);
	}
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
		sqlite3_free(error_message);
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
		sqlite3_free(error_message);
		return -1;
	}

	return 1;
}

int database_handler::insert_error(const char* error_type, const char* injection_value, const char* url, const char* tool_name, const char* response) {
	char* error_message = NULL;
	std::string stmt;

	stmt += "INSERT INTO ERRORS (ERROR_TYPE, INJECTION_VALUE, URL_ID, TOOL_NAME, RESPONSE) VALUES('";
	stmt += error_type;
	stmt += "','";
	stmt += injection_value;
	stmt += "', (SELECT URL_ID FROM URLS WHERE URL='";
	stmt += url;
	stmt += "'),'";
	stmt += tool_name;
	stmt += "','";
	stmt += response;
	stmt += "');";

	sqlite3_exec(db, stmt.c_str(), NULL, 0, &error_message);

	if(error_message){
		sqlite3_free(error_message);
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
		sqlite3_free(error_message);
		return -1;
	}
	return 1;
}

void database_handler::update_parent_url() {
	sqlite3_stmt* sqlite_stmt1;
	sqlite3_stmt* sqlite_stmt2;

	if(sqlite3_prepare(db, "SELECT * FROM URLS", -1, &sqlite_stmt1, 0) == SQLITE_OK) {
		while(sqlite3_step(sqlite_stmt1) == SQLITE_ROW) {
			char* child_id = (char*)sqlite3_column_text(sqlite_stmt1, 0);
			std::string url = (char*)sqlite3_column_text(sqlite_stmt1, 1);
			url.erase(0, (url.find_first_of("//") + 2));

			int n = url.find_last_of("/");
			if(n != std::string::npos && n == url.length() - 1){
				url.erase(n, 1);
				n = url.find_last_of("/");
			}

			while(n != std::string::npos) {
				bool brk = false;

				url.erase(n, std::string::npos);
				std::string select_stmt;
				select_stmt += "SELECT * FROM URLS WHERE URL='http://";
				select_stmt += url;
				select_stmt += "' OR URL='http://";
				select_stmt += url;
				select_stmt += "/';";

				if(sqlite3_prepare(db, select_stmt.c_str(), -1, &sqlite_stmt2, 0) == SQLITE_OK) {
					if(sqlite3_step(sqlite_stmt2) == SQLITE_ROW){
						char* parent_id = (char*)sqlite3_column_text(sqlite_stmt2, 0);
						std::string update_stmt;
						update_stmt += "UPDATE URLS SET URL_PARENT_ID=";
						update_stmt += parent_id;
						update_stmt += " WHERE URL_ID=";
						update_stmt += child_id;
						update_stmt += ";";

						char* error_message = NULL;
						sqlite3_exec(db, update_stmt.c_str(), NULL, 0, &error_message);

						if(error_message){
							sqlite3_free(error_message);
						}
						brk = true;
					}
				}

				sqlite3_finalize(sqlite_stmt2);
				n = url.find_last_of("/");

				if(brk) {
					break;
				}
			}
		}
	}

	sqlite3_finalize(sqlite_stmt1);
}

database_handler::~database_handler() {
}