#ifndef ARACHNI_PARSER_HPP
#define ARACHNI_PARSER_HPP

#include "database_handler.hpp"

static const char *ARACHNI_APP_NAME = "ARACHNI";
static const char *ARACHNI_XSS_PATTERN = "Cross-Site Scripting";
static const char *ARACHNI_SQLI_PATTERN = "SQL Injection";

int arachni_parse (const char* filename, database_handler* db_handler);

#endif