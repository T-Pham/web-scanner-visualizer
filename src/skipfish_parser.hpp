#ifndef SKIPFISH_PARSE_HPP
#define SKIPFISH_PARSE_HPP

static const char * SKIPFISH_APP_NAME = "SKIPFISH";
static const char * XSS_ERROR1 = "40101";
static const char * XSS_ERROR2 = "40102";
static const char * XSS_ERROR3 = "40105";
static const char * SQLI_ERROR1 = "50103";
static const char * SQLI_ERROR2 = "50201";

int skipfish_parse(const char* filename, database_handler* db_handler);

#endif