#ifndef DATABASE_HANDLER_HPP
#define DATABASE_HANDLER_HPP

#include "sqlite3.h"

static const char* sql_create_stms = "DROP TABLE IF EXISTS URLS;\
									 CREATE TABLE URLS (\
									 URL_ID INTEGER PRIMARY KEY,\
									 URL TEXT NOT NULL,\
									 URL_PARENT_ID INTEGER, \
									 UNIQUE(URL));\
									 DROP TABLE IF EXISTS PARAMETERS;\
									 CREATE TABLE PARAMETERS (\
									 PARAMETER_ID INTEGER PRIMARY KEY,\
									 PARAMETER_NAME VARCHAR(255) NOT NULL,\
									 ERROR_ID INTEGER NOT NULL\
									 );\
									 DROP TABLE IF EXISTS ERRORS;\
									 CREATE TABLE ERRORS (\
									 ERROR_ID INTEGER PRIMARY KEY,\
									 ERROR_TYPE VARCHAR(255) NOT NULL,\
									 ERROR_LEVEL INTEGER,\
									 INJECTION_VALUE TEXT,\
									 URL_ID INTEGER,\
								     METHOD VARCHAR(255) NOT NULL,\
									 TOOL_NAME VARCHAR(255) NOT NULL,\
									 RESPONSE TEXT\
									 );";

class database_handler {
public:
	sqlite3* db;

	int initialize_database();

	void open_database();

	void close_database();

	void begin_transaction();

	void commit_transaction();

	int insert_url(const char* url);

	int insert_url_relationship(int parent_url_id, int child_url_id);

	int insert_error(const char* error_type, const char* error_level, const char* injection_value, const char* url, const char* method, const char* tool_name, const char* response);

	int insert_parameter(const char* parameter_name, int url_id);

	void update_parent_url();
};

#endif