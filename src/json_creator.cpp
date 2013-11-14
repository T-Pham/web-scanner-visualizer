#include "sqlite3.h"
#include "json_creator.hpp"
#include "helper.hpp"
#include "database_handler.hpp"
#include "wapiti_parser.hpp"
#include "w3af_parser.hpp"
#include "arachni_parser.hpp"
#include <fstream>
#include <sstream>
#include <string.h>

// load data from sqlite and create json
void create_json() {
	std::ofstream* file = new std::ofstream;
	database_handler* db_handler = new database_handler();

	file->open("output.js");
	db_handler->open_database();

	create_graph_json(file, db_handler);
	create_params_json(file, db_handler);

	file->close();
	db_handler->close_database();

	delete file;
	delete db_handler;
}

std::string* get_node_info(int current_id, const char* error_type, database_handler* db_handler) {
	std::string* str = new std::string();
	std::stringstream ss;
	// open
	*str += "total:";
	int total = number_of_error(current_id, error_type, db_handler);
	ss << total;
	*str += ss.str();
	ss.str("");

	*str += ", arachni: ";
	int wapiti = number_of_error_by_tool(current_id, ARACHNI_APP_NAME, error_type, db_handler);
	ss << wapiti;
	*str += ss.str();
	ss.str("");

	*str += ", w3af: ";
	int w3af = number_of_error_by_tool(current_id, W3AF_APP_NAME, error_type, db_handler);
	ss << w3af;
	*str += ss.str();
	ss.str("");

	*str += ", wapiti: ";
	int arachni = number_of_error_by_tool(current_id, WAPITI_APP_NAME, error_type, db_handler);
	ss << arachni;
	*str += ss.str();
	ss.str("");

	*str += ", pie: [";

	if (total) {
		ss << wapiti * 100 / total;
		*str += ss.str();
		ss.str("");
		*str += ", ";

		ss << w3af * 100 / total;
		*str += ss.str();
		ss.str("");
		*str += ", ";

		ss << arachni * 100 / total;
		*str += ss.str();
		*str += ", 0";
	}
	else {
		*str += "0, 0, 0, 100";
	}

	// close
	*str += "]";

	return str;
}

void create_graph_json(std::ofstream* file, database_handler* db_handler) {
	std::string str1;
	std::string str2;

	sqlite3_stmt* sqlite_stmt;
	if (sqlite3_prepare(db_handler->db, "SELECT * FROM URLS", -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			std::stringstream ss;
			int current_id = (int)sqlite3_column_int(sqlite_stmt, 0);
			char* url = (char*)sqlite3_column_text(sqlite_stmt, 1);
			int parent_id = (int)sqlite3_column_int(sqlite_stmt, 2);

			if (!str1.empty()) {
				str1 += ",";
			}
			str1 += "{node: \"";
			str1 += url;

			str1 += "\", sqli: {";
			std::string* p_str = get_node_info(current_id, "SQL Injection", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}, xss: {";
			p_str = get_node_info(current_id, "Cross Site Scripting", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}, both:{";
			p_str = get_node_info(current_id, "BOTH", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}}";

			if (parent_id > 0) {
				if (!str2.empty()) {
					str2 += ",";
				}
				str2 += "{source: ";
				ss << (parent_id - 1);
				str2 += ss.str();
				str2 += ", target: ";
				ss.str("");
				ss << (current_id - 1);
				str2 += ss.str();
				ss.str("");
				str2 += "}";
			}
		}
	}
	sqlite3_finalize(sqlite_stmt);

	*file << "var dataset = {nodes: [" << str1 << "], edges: [" << str2 << "]};" << std::endl;
	*file << "var sqli_average = " << get_average("SQL Injection", db_handler) << ";" << std::endl;
	*file << "var xss_average = " << get_average("Cross Site Scripting", db_handler) << ";" << std::endl;
	*file << "var both_average = " << get_average("BOTH", db_handler) << ";" << std::endl;
}

std::string* get_params(const char* error_id, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string select_param_stmt("SELECT PARAMETER_NAME FROM PARAMETERS WHERE ERROR_ID==");
	select_param_stmt += error_id;
	select_param_stmt += ";";
	std::string* params = new std::string();

	if (sqlite3_prepare(db_handler->db, select_param_stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			const char* param = (const char*)sqlite3_column_text(sqlite_stmt, 0);

			if (!params->empty()) {
				*params += ", ";
			}
			*params += "\"";
			*params += param;
			*params += "\"";
		}
	}
	sqlite3_finalize(sqlite_stmt);

	return params;
}

string_set* get_url_info(const int current_id, const char* tool_name, database_handler* db_handler) {
	sqlite3_stmt* sqlite_stmt;
	std::string select_error_stmt;
	std::stringstream ss;
	ss << current_id;

	select_error_stmt += "SELECT ERROR_ID, ERROR_TYPE, INJECTION_VALUE, METHOD FROM ERRORS WHERE URL_ID==";
	select_error_stmt += ss.str();
	select_error_stmt += " AND TOOL_NAME='";
	select_error_stmt += tool_name;
	select_error_stmt += "' AND INJECTION_VALUE<>'NULL' AND INJECTION_VALUE<>'';";

	std::string* sqli_injection_value = new std::string();
	std::string* xss_injection_value = new std::string();


	if (sqlite3_prepare(db_handler->db, select_error_stmt.c_str(), -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			const char* error_id = (const char*)sqlite3_column_text(sqlite_stmt, 0);
			const char* error_type = (const char*)sqlite3_column_text(sqlite_stmt, 1);
			const char* injection_value = (const char*)sqlite3_column_text(sqlite_stmt, 2);
			const char* method = (const char*)sqlite3_column_text(sqlite_stmt, 3);

			if (strcmp(error_type, "SQL Injection") == 0) {
				if (!sqli_injection_value->empty()){
					*sqli_injection_value += ", ";
				}
				*sqli_injection_value += "{injection_value: \"";
				*sqli_injection_value += injection_value;
				*sqli_injection_value += "\", method: \"";
				*sqli_injection_value += method;
				*sqli_injection_value += "\", params: [";

				std::string* params = get_params(error_id, db_handler);
				*sqli_injection_value += params->c_str();
				delete params;

				*sqli_injection_value += "]}";
			}
			else {
				if (strcmp(error_type, "Cross Site Scripting") == 0) {
					if (!xss_injection_value->empty()){
						*xss_injection_value += ", ";
					}
					*xss_injection_value += "{injection_value: \"";
					*xss_injection_value += injection_value;
					*xss_injection_value += "\", method: \"";
					*xss_injection_value += method;
					*xss_injection_value += "\", params: [";

					std::string* params = get_params(error_id, db_handler);
					*xss_injection_value += params->c_str();
					delete params;

					*xss_injection_value += "]}";
				}
			}
		}
	}
	sqlite3_finalize(sqlite_stmt);

	// need memcheck here
	string_set* set = new string_set();
	set->sqli_injection_value = sqli_injection_value;
	set->xss_injection_value = xss_injection_value;

	return set;
}

std::string* join_strings(string_set* set) {
	std::string* result = new std::string();
	std::string* sqli_injection_value = set->sqli_injection_value;
	std::string* xss_injection_value = set->xss_injection_value;

	*result += "{sqli: [";
	*result += sqli_injection_value->c_str();
	*result += "], xss: [";
	*result += xss_injection_value->c_str();
	*result += "]}";

	delete sqli_injection_value;
	delete xss_injection_value;

	return result;
}

void create_params_json(std::ofstream* file, database_handler* db_handler) {
	std::string str;
	std::string* result;
	string_set* set;

	sqlite3_stmt* sqlite_stmt;

	if (sqlite3_prepare(db_handler->db, "SELECT * FROM URLS", -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while (sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			int current_id = (int)sqlite3_column_int(sqlite_stmt, 0);
			char* url = (char*)sqlite3_column_text(sqlite_stmt, 1);
			if (!str.empty()) {
				str += ", ";
			}

			str += "{url: \"";
			str += url;
			str += "\", ";

			set = get_url_info(current_id, ARACHNI_APP_NAME, db_handler);
			str += "arachni: ";
			result = join_strings(set);
			str += result->c_str();
			delete result;
			delete set;
			str += ", ";

			set = get_url_info(current_id, W3AF_APP_NAME, db_handler);
			str += "w3af: ";
			result = join_strings(set);
			str += result->c_str();
			delete result;
			delete set;
			str += ", ";

			set = get_url_info(current_id, WAPITI_APP_NAME, db_handler);
			str += "wapiti: ";
			result = join_strings(set);
			str += result->c_str();
			delete result;
			delete set;

			str += "}";
		}
	}
	sqlite3_finalize(sqlite_stmt);

	*file << "var params = {urls: [" << str << "]};";
}

