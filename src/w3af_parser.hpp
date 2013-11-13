#ifndef W3AF_PARSER_HPP
#define W3AF_PARSER_HPP

#include "database_handler.hpp"

static const char* W3AF_APP_NAME = "W3AF";
static const char* W3AF_SQLI_PATTERN = "SQL injection";  
static const char* W3AF_XSS_PATTERN = "Cross site scripting";  

int w3af_parse(const char* filename, database_handler* db_handler);

#endif