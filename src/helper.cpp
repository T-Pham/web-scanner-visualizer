#include "helper.hpp"
#include "database_handler.hpp"
#include <sstream>
#include <string>

// Helper Functions
void insert_error_with_url(const char * url_with_para, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db) {
	std::string url_without_para;
	url_without_para += url_with_para;
	int n = url_without_para.find("?");

	if(n >= 0) {
		url_without_para = url_without_para.substr(0, n);
	}

	int error_id = db->insert_error(bug_type, bug_level, injection_value, url_without_para.c_str(), tool_name, info);

	if(error_id > -1) {
		std::string* parameters = new std::string(injection_value);
		while(!parameters->empty()) {
			std::string* parameter = get_parameter(parameters);
			if(parameter) {
				db->insert_parameter(parameter->c_str(), error_id);
			}
			delete(parameter);
		}
		delete(parameters);
	}
}

std::string* get_parameter(std::string* parameters) {
	if(!parameters || parameters->empty()) {
		return NULL;
	}
	else {
		std::string* parameter = new std::string(parameters->substr(0, parameters->find_first_of("=")));

		int n = parameters->find_first_of("&");
		if(n == std::string::npos){
			parameters->clear();
		}
		else{
			parameters->erase(0, n+1);
		}

		return parameter;
	}
}

int frequency_of_error(int url_id, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string sql_stmt;
	std::stringstream ss;
	ss << url_id;
	sql_stmt += "SELECT COUNT(DISTINCT TOOL_NAME) FROM ERRORS WHERE URL_ID=";
	sql_stmt += ss.str();
	sql_stmt += ";";
	if(sqlite3_prepare(db_handler->db, sql_stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while(sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			return (int) sqlite3_column_int(sqlite_stmt, 0);
		}
	}
	sqlite3_finalize(sqlite_stmt);

	return -1;
}