#ifndef HELPER_HPP
#define HELPER_HPP

#include "database_handler.hpp"
#include <string>

void insert_error_with_url(const char * url_with_para, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db);

void insert_error_with_url(const char * url_with_para, const char* param, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db);

std::string* get_parameter(std::string* parameters);

int frequency_of_error_by_type(int url_id, char* type, database_handler* db_handler);

#endif