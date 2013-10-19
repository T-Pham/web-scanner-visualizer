#ifndef WAPITI_PARSER_H
#define WAPITI_PARSER_H
#include "database_handler.hpp"
#include <string>

int wapiti_parse(const char* filename, database_handler* db);
int wapiti_tree_parse(const char* filename, database_handler* db);
char* get_parameter(std::string* parameters);

#endif