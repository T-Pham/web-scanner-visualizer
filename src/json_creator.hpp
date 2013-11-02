#ifndef JSON_CREATOR_HPP
#define JSON_CREATOR_HPP

#include "database_handler.hpp"
#include <fstream>


void create_json();

void create_graph_json(std::ofstream* file, database_handler* db_handler);

void create_params_json(std::ofstream* file, database_handler* db_handler);

#endif