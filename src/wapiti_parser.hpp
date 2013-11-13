#ifndef WAPITI_PARSER_H
#define WAPITI_PARSER_H

#include "database_handler.hpp"
#include <string>

static const char *WAPITI_APP_NAME = "WAPITI";
static const char *WAPITI_XSS_PATTERN = "Cross Site Scripting";
static const char *WAPITI_SQLI_PATTERN = "SQL Injection";

int wapiti_parse(const char* filename, database_handler* db_handler);

int wapiti_tree_parse(const char* filename, database_handler* db_handler);

std::string* get_parameter(std::string* parameters);

#endif