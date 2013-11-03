#ifndef JSON_CREATOR_HPP
#define JSON_CREATOR_HPP

#include "database_handler.hpp"
#include <fstream>

class string_set {
public:
	std::string* sqli_injection_value;
	std::string* sqli_params;
	std::string* xss_injection_value;
	std::string* xss_params;
};

void create_json();

void create_graph_json(std::ofstream* file, database_handler* db_handler);

void create_params_json(std::ofstream* file, database_handler* db_handler);

#endif