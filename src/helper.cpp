#include "helper.hpp"
#include "database_handler.hpp"
#include <string>

// Helper Functions
void insert_error_with_url(const char * url_with_para, const char* bug_type, const char* bug_level, const char* injection_value, const char* tool_name, const char* info, database_handler* db) {
	std::string url_without_para;
	url_without_para += url_with_para;
	int n = url_without_para.find("?");

	if(n >= 0) {
		url_without_para = url_without_para.substr(0, n);
	}

	int error_id = db->insert_error(bug_type, bug_level, injection_value, url_without_para.c_str(), tool_name, info);

	if(error_id > -1) {
		std::string* parameters = new std::string(injection_value);
		while(!parameters->empty()) {
			std::string* parameter = get_parameter(parameters);
			if(parameter) {
				db->insert_parameter(parameter->c_str(), error_id);
			}
			delete(parameter);
		}
		delete(parameters);
	}
}

std::string* get_parameter(std::string* parameters) {
	if(!parameters || parameters->empty()) {
		return NULL;
	}
	else {
		std::string* parameter = new std::string(parameters->substr(0, parameters->find_first_of("=")));

		int n = parameters->find_first_of("&");
		if(n == std::string::npos){
			parameters->clear();
		}
		else{
			parameters->erase(0, n+1);
		}

		return parameter;
	}
}