#ifndef DATABASE_HANDLER_HPP
#define DATABASE_HANDLER_HPP

static const char* sql_create_stms_1 = "DROP TABLE IF EXISTS URLS;\
							  CREATE TABLE URLS (\
							  URL_ID INTEGER PRIMARY KEY,\
							  URL TEXT NOT NULL,\
							  METHOD VARCHAR(255) NOT NULL,\
							  URL_PARENT_ID INTEGER \
							  );";

static const char* sql_create_stms_2 = "DROP TABLE IF EXISTS URL_RELATIONSHIPS;\
							  CREATE TABLE URL_RELATIONSHIPS (\
							  URL_RELATIONSHIP_ID INTEGER PRIMARY KEY,\
							  PARENT_URL_ID INTEGER NOT NULL,\
							  CHILD_URL_ID INTEGER NOT NULL\
							  );";

static const char* sql_create_stms_3 = "DROP TABLE IF EXISTS PARAMETERS;\
							  CREATE TABLE PARAMETERS (\
							  PARAMETER_ID INTEGER PRIMARY KEY,\
							  PARAMETER_NAME VARCHAR(255) NOT NULL,\
							  URL_ID INTEGER NOT NULL\
							  );";
							  
static const char* sql_create_stms_4 = "DROP TABLE IF EXISTS ERRORS;\
							  CREATE TABLE ERRORS (\
							  ERROR_ID INTEGER PRIMARY KEY,\
							  ERROR_TYPE VARCHAR(255) NOT NULL,\
							  INJECTION_VALUE TEXT,\
							  URL_ID INTEGER NOT NULL,\
							  TOOL_NAME VARCHAR(255) NOT NULL,\
							  RESPONSE TEXT\
							  );";

class database_handler {
	sqlite3* db;
public:
	void initialize_database();

	void open_database();

	void close_database();

	int insert_url(char* url, char* method);

	int insert_url_relationship(int parent_url_id, int child_url_id);

	int insert_error(char* error_type, char* injection_value, int url_id, char* tool_name, char* response);

	int insert_parameter(char* parameter_name, int url_id);

	~database_handler();
};

#endif