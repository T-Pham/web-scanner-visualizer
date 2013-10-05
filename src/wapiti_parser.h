#ifndef WAPITI_PARSER_H
#define WAPITI_PARSER_H
#include "database_handler.hpp"

int wapiti_parse(char* filename, database_handler* db);
int wapiti_tree_parse(char* filename, database_handler* db);

#endif