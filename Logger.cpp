/*
 * Logger.cpp
 *
 *  Created on: 22 февр. 2016 г.
 *      Author: secator
 */

#include "Logger.h"
#include <iostream>
#include <stdarg.h>
#include <string.h>

using namespace std;

Logger::Logger(const char *data) {
	cout << "Logger instance created. Name of uses program is " << data
			<< ". Default and current out is stdout." << endl;
	name_of_program = data;
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

int Logger::log(const char *data, ...) {
	char buffer[512] = "";
	strcat(buffer, data);
	char *p;
	va_list arg_list;
	va_start(arg_list, data);
	while ((p = va_arg(arg_list, char*)) != NULL) {
		strcat(buffer, " ");
		strcat(buffer, p);
	}
	va_end(arg_list);
	cout << name_of_program << ": " << buffer << endl;
	return 0;
}
