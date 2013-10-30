#include "sqlite3.h"
#include "json_creator.hpp"
#include "helper.hpp"
#include "database_handler.hpp"
#include "wapiti_parser.hpp"
#include "skipfish_parser.hpp"
#include "arachni_parser.hpp"
#include <fstream>
#include <sstream>

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

			// open sql
			str1 += "\", \"sql\": {\"total\":";
			ss << number_of_error(current_id + 1, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			str1 += ", \"wapiti\":";
			ss << number_of_error_by_tool(current_id + 1, WAPITI_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			str1 += ", \"skipfish\":";
			ss << number_of_error_by_tool(current_id + 1, SKIPFISH_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			
			str1 += ", \"arachni\":";
			ss << number_of_error_by_tool(current_id + 1, ARACHNI_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			// close sql, open xss
			str1 += "},  \"xss\": {\"total\": ";
			ss << number_of_error(current_id + 1, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			str1 += ", \"wapiti\":";
			ss << number_of_error_by_tool(current_id + 1, WAPITI_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			str1 += ", \"skipfish\":";
			ss << number_of_error_by_tool(current_id + 1, SKIPFISH_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");

			
			str1 += ", \"arachni\":";
			ss << number_of_error_by_tool(current_id + 1, ARACHNI_APP_NAME, "SQL Injection", db_handler);
			str1 += ss.str();
			ss.str("");
			
			// close xss
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

	file << "var dataset = {\"nodes\": [" << str1 << "], \"edges\": [" << str2 << "]};";
	file.close();
	
	db_handler->close_database();
	delete db_handler;
}