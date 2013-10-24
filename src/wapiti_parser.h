#ifndef WAPITI_PARSER_H
#define WAPITI_PARSER_H
#include "database_handler.hpp"
#include <string>

int wapiti_parse(const char* filename, database_handler* db);
int wapiti_tree_parse(const char* filename, database_handler* db);
std::string* get_parameter(std::string* parameters);
void insert_error_with_url(const char * url_with_para, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db);

#endif