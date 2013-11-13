#include "helper.hpp"
#include "database_handler.hpp"
#include <sstream>
#include <string.h>

// Helper Functions
void insert_error_with_url(const char * url_with_para, const char* method, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db) {
	std::string url_without_para;
	url_without_para += url_with_para;
	int n = url_without_para.find("?");

	if (n >= 0) {
		url_without_para = url_without_para.substr(0, n);
	}

	int error_id = db->insert_error(bug_type, bug_level, injection_value, url_without_para.c_str(), method, tool_name, info);

	if (error_id > -1) {
		std::string* parameters = new std::string(injection_value);
		while (!parameters->empty()) {
			std::string* parameter = get_parameter(parameters);
			if (parameter) {
				db->insert_parameter(parameter->c_str(), error_id);
			}
			delete(parameter);
		}
		delete(parameters);
	}
}

// Override
void insert_error_with_url(const char* url_with_para, const char* method, const char* param, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db) {
	std::string url_without_para;
	url_without_para += url_with_para;
	int n = url_without_para.find("?");

	if (n >= 0) {
		url_without_para = url_without_para.substr(0, n);
	}

	int error_id = db->insert_error(bug_type, bug_level, injection_value, url_without_para.c_str(), method, tool_name, info);

	if (error_id > -1) {
		db->insert_parameter(param, error_id);
	}
}

std::string* get_parameter(std::string* parameters) {
	if (!parameters || parameters->empty()) {
		return NULL;
	}
	else {
		std::string* parameter = new std::string(parameters->substr(0, parameters->find_first_of("=")));

		int n = parameters->find_first_of("&");
		if (n == std::string::npos){
			parameters->clear();
		}
		else{
			parameters->erase(0, n + 1);
		}

		return parameter;
	}
}

int number_of_error(int url_id, const char * type, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string sql_stmt;
	std::stringstream ss;
	ss << url_id;

	sql_stmt += "SELECT COUNT(*) FROM ERRORS WHERE URL_ID=";
	sql_stmt += ss.str();

	if (strcmp(type, "BOTH") != 0) {
		sql_stmt += " AND ERROR_TYPE='";
		sql_stmt += type;
		sql_stmt += "'";
	}
	sql_stmt += ";";

	int result = -1;
	if (sqlite3_prepare(db_handler->db, sql_stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			result = (int)sqlite3_column_int(sqlite_stmt, 0);
		}
	}
	sqlite3_finalize(sqlite_stmt);

	return result;
}

int number_of_error_by_tool(int url_id, const char* tool, const char* type, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string sql_stmt;
	std::stringstream ss;
	ss << url_id;

	sql_stmt += "SELECT COUNT(*) FROM ERRORS WHERE URL_ID=";
	sql_stmt += ss.str();
	sql_stmt += " AND TOOL_NAME='";
	sql_stmt += tool;

	if (strcmp(type, "BOTH") != 0) {
		sql_stmt += "' AND ERROR_TYPE='";
		sql_stmt += type;

	}
	sql_stmt += "';";

	int result = -1;
	if (sqlite3_prepare(db_handler->db, sql_stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			result = (int)sqlite3_column_int(sqlite_stmt, 0);
		}
	}
	sqlite3_finalize(sqlite_stmt);

	return result;
}

int get_average(const char* error_type, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string str;
	str += "SELECT AVG(COUNT) FROM (SELECT URL_ID, COUNT(*) AS COUNT FROM ERRORS WHERE URL_ID<>'NULL'";
	if (strcmp(error_type, "BOTH") != 0) {
		str += " AND ERROR_TYPE=='";
		str += error_type;
		str += "'";
	}
	str += " GROUP BY URL_ID);";
	int result = -1;

	if (sqlite3_prepare(db_handler->db, str.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			result = (int)sqlite3_column_int(sqlite_stmt, 0);
		}
	}
	sqlite3_finalize(sqlite_stmt);

	return result;
}