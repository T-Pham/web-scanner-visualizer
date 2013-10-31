#include "sqlite3.h"
#include "json_creator.hpp"
#include "helper.hpp"
#include "database_handler.hpp"
#include "wapiti_parser.hpp"
#include "skipfish_parser.hpp"
#include "arachni_parser.hpp"
#include <fstream>
#include <sstream>

std::string* get_node_info(int current_id, const char* error_type, database_handler* db_handler) {
	std::string* str = new std::string();
	std::stringstream ss;
	// open
	*str += "\"total\":";
	int total = number_of_error(current_id + 1, error_type, db_handler);
	ss << total;
	*str += ss.str();
	ss.str("");

	*str += ", \"wapiti\":";
	int wapiti = number_of_error_by_tool(current_id + 1, WAPITI_APP_NAME, error_type, db_handler);
	ss << wapiti;
	*str += ss.str();
	ss.str("");

	*str += ", \"skipfish\":";
	int skipfish = number_of_error_by_tool(current_id + 1, SKIPFISH_APP_NAME, error_type, db_handler);
	ss << skipfish;
	*str += ss.str();
	ss.str("");


	*str += ", \"arachni\":";
	int arachni = number_of_error_by_tool(current_id + 1, ARACHNI_APP_NAME, error_type, db_handler);
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
		ss << skipfish*100/total;
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
	std::ofstream file;
	database_handler* db_handler = new database_handler();

	file.open("output.js");
	db_handler->open_database();

	std::string str1;
	std::string str2;

	sqlite3_stmt* sqlite_stmt;
	if(sqlite3_prepare(db_handler->db, "SELECT * FROM URLS", -1, &sqlite_stmt, 0) == SQLITE_OK) {
		while(sqlite3_step(sqlite_stmt) == SQLITE_ROW) {
			std::stringstream ss;
			int current_id = (int)sqlite3_column_int(sqlite_stmt, 0) - 1;
			char* url = (char*)sqlite3_column_text(sqlite_stmt, 1);
			int parent_id = (int)sqlite3_column_int(sqlite_stmt, 3) - 1;

			if(!str1.empty()) {
				str1 += ",";
			}
			str1 += "{\"node\": \"";
			str1 += url;

			str1 += "\", \"sql\": {";
			std::string* p_str = get_node_info(current_id + 1, "SQL Injection", db_handler);
			str1 += p_str->c_str();
			delete p_str;
			str1 += "}, \"xss\": {";
			p_str = get_node_info(current_id + 1, "Cross Site Scripting", db_handler);
			str1 += p_str->c_str();
			delete p_str;
			str1 += "}}";

			if(parent_id > -1) {
				if(!str2.empty()) {
					str2 += ",";
				}
				str2 += "{\"source\": ";
				ss << parent_id;
				str2 += ss.str();
				str2 +=  ",\"target\": ";
				ss.str("");
				ss << current_id;
				str2 += ss.str();
				ss.str("");
				str2 += "}";
			}
		}
	}
	sqlite3_finalize(sqlite_stmt);

	file << "var dataset = {\"nodes\": [" << str1 << "], \"edges\": [" << str2 << "]};" << std::endl;
	file << "var sql_average = " << get_average("SQL Injection", db_handler) << ";" << std::endl;
	file << "var xss_average = " << get_average("Cross Site Scripting", db_handler) << ";" << std::endl;
	file.close();

	db_handler->close_database();
	delete db_handler;
}