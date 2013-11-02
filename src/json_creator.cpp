#include "sqlite3.h"
#include "json_creator.hpp"
#include "helper.hpp"
#include "database_handler.hpp"
#include "wapiti_parser.hpp"
#include "skipfish_parser.hpp"
#include "arachni_parser.hpp"
#include <fstream>
#include <sstream>
#include <string.h>

std::string* get_node_info(int current_id, const char* error_type, database_handler* db_handler) {
	std::string* str = new std::string();
	std::stringstream ss;
	// open
	*str += "\"total\":";
	int total = number_of_error(current_id , error_type, db_handler);
	ss << total;
	*str += ss.str();
	ss.str("");

	*str += ", \"wapiti\":";
	int wapiti = number_of_error_by_tool(current_id, WAPITI_APP_NAME, error_type, db_handler);
	ss << wapiti;
	*str += ss.str();
	ss.str("");

	*str += ", \"skipfish\":";
	int skipfish = number_of_error_by_tool(current_id, SKIPFISH_APP_NAME, error_type, db_handler);
	ss << skipfish;
	*str += ss.str();
	ss.str("");


	*str += ", \"arachni\":";
	int arachni = number_of_error_by_tool(current_id, ARACHNI_APP_NAME, error_type, db_handler);
	ss << arachni;
	*str += ss.str();
	ss.str("");

	*str += ", \"pie\": [";

	if(total) {
		ss << wapiti*100/total;
		*str += ss.str();
		ss.str("");
		*str += ", ";
		ss << skipfish*100/total;
		*str += ss.str();
		ss.str("");
		*str += ", ";
		ss << arachni*100/total;
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

void create_graph_json(std::ofstream* file, database_handler* db_handler) {
	std::string str1;
	std::string str2;

	sqlite3_stmt* sqlite_stmt;
	if(sqlite3_prepare(db_handler->db, "SELECT * FROM URLS", -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while(sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			std::stringstream ss;
			int current_id = (int)sqlite3_column_int(sqlite_stmt, 0);
			char* url = (char*)sqlite3_column_text(sqlite_stmt, 1);
			int parent_id = (int)sqlite3_column_int(sqlite_stmt, 3);

			if(!str1.empty()) {
				str1 += ",";
			}
			str1 += "{\"node\": \"";
			str1 += url;

			str1 += "\", \"sqli\": {";
			std::string* p_str = get_node_info(current_id, "SQL Injection", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}, \"xss\": {";
			p_str = get_node_info(current_id, "Cross Site Scripting", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}, \"both\":{";
			p_str = get_node_info(current_id, "BOTH", db_handler);
			str1 += p_str->c_str();
			delete p_str;

			str1 += "}}";

			if(parent_id > 0) {
				if(!str2.empty()) {
					str2 += ",";
				}
				str2 += "{\"source\": ";
				ss << (parent_id - 1);
				str2 += ss.str();
				str2 +=  ",\"target\": ";
				ss.str("");
				ss << (current_id - 1);
				str2 += ss.str();
				ss.str("");
				str2 += "}";
			}
		}
	}
	sqlite3_finalize(sqlite_stmt);

	*file << "var dataset = {\"nodes\": [" << str1 << "], \"edges\": [" << str2 << "]};" << std::endl;
	*file << "var sqli_average = " << get_average("SQL Injection", db_handler) << ";" << std::endl;
	*file << "var xss_average = " << get_average("Cross Site Scripting", db_handler) << ";" << std::endl;
	*file << "var both_average = " << get_average("BOTH", db_handler) << ";" << std::endl;
}

void create_params_json(std::ofstream* file, database_handler* db_handler) {
	std::string str1;

	sqlite3_stmt* sqlite_stmt;
	if(sqlite3_prepare(db_handler->db, "SELECT * FROM URLS", -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while(sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			int current_id = (int)sqlite3_column_int(sqlite_stmt, 0);
			char* url = (char*)sqlite3_column_text(sqlite_stmt, 1);

			sqlite3_stmt* inner_sqlite_stmt;
			std::string inner_sql_stmt;
			std::stringstream ss;
			inner_sql_stmt += "SELECT ERRORS.URL_ID AS URL_ID, ERRORS.ERROR_TYPE, ERRORS.INJECTION_VALUE, PARAMETERS.PARAMETER_NAME FROM (PARAMETERS JOIN ERRORS ON PARAMETERS.ERROR_ID==ERRORS.ERROR_ID) WHERE URL_ID==";
			ss << current_id;
			inner_sql_stmt += ss.str();
			inner_sql_stmt += ";";

			if(!str1.empty()) {
				str1 += ", ";
			}

			std::string sqli_injection_value;
			std::string sqli_params;
			std::string xss_injection_value;
			std::string xss_params;

			if(sqlite3_prepare(db_handler->db, inner_sql_stmt.c_str(), -1, &inner_sqlite_stmt, 0) == SQLITE_OK) {
				while(sqlite3_step(inner_sqlite_stmt) == SQLITE_ROW) {
					const char* error_type = (const char*) sqlite3_column_text(inner_sqlite_stmt, 1);
					const char* injection_value = (const char*) sqlite3_column_text(inner_sqlite_stmt, 2);
					const char* params = (const char*) sqlite3_column_text(inner_sqlite_stmt, 3);

					if(strcmp(error_type, "SQL Injection")) {
						if(!sqli_injection_value.empty()){
							sqli_injection_value += ", ";
						}
						if(!sqli_params.empty()){
							sqli_params += ", ";
						}
						sqli_injection_value += "\"";
						sqli_injection_value += injection_value;
						sqli_injection_value += "\"";
						sqli_params += "\"";
						sqli_params += params;
						sqli_params += "\"";
					}
					else {
						if(strcmp(error_type, "Cross Site Scripting")) {
							if(!xss_injection_value.empty()){
								xss_injection_value += ", ";
							}
							if(!xss_params.empty()){
								xss_params += ", ";
							}
							xss_injection_value += "\"";
							xss_injection_value += injection_value;
							xss_injection_value += "\"";
							xss_params += "\"";
							xss_params += params;
							xss_params += "\"";
						}
					}
				}
			}
			sqlite3_finalize(inner_sqlite_stmt);

			str1 += "{\"url\": \"";
			str1 += url;
			str1 += "\", ";
			
			str1 += "\"sqli\": {";
			str1 += "\"injection_value\": [";
			str1 += sqli_injection_value.c_str();
			str1 += "], \"params\": [";
			str1 += sqli_params.c_str();
			
			str1 += "]}, \"xss\": {";
			str1 += "\"injection_value\": [";
			str1 += xss_injection_value.c_str();
			str1 += "], \"params\": [";
			str1 += xss_params.c_str();
			str1 += "]}";
			
			str1 += "}";
		}
	}
	sqlite3_finalize(sqlite_stmt);
	*file << "var params = {\"urls\": [" << str1 << "]};";
}