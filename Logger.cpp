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
#include <syslog.h>
#include <fstream>

using namespace std;

Logger::Logger(const char *data) {
	cout << "Logger instance created. Name of uses program is " << data << "."
			<< endl;
	name_of_program = data;
}

Logger::~Logger() {
	if (type == "syslog") {
		this->log("Logger: syslog close log", NULL);
		closelog();
	}
	if (type == "file") {
		this->log("Logger: close file.", NULL);
		stream->close();
	}
}
void Logger::init(string &type) {
	cout << "Logger switch out to: " << type << "." << endl;
	Logger::type = type;
	if (type == "syslog") {
		openlog(name_of_program.c_str(), 0, LOG_USER);
	}
	if (type == "file") {
		static ofstream realstream;
		realstream.open("/tmp/hwd.log", ofstream::app);
		stream = &realstream;
	}
	log("Logger->init: hwd started", NULL);
}

void Logger::log(const char *data, ...) {
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

	if (type == "off") {
		return;
	}
	if (type == "syslog") {
		syslog(LOG_NOTICE, buffer);
		return;
	}
	if (type == "file") {
		time_t td;
		td = time(NULL);
		stream->write(ctime(&td), strlen(ctime(&td)) - 1);
		stream->write(" ", sizeof(" "));
		strcat(buffer, "\n");
		stream->write(buffer, sizeof buffer);
		stream->flush();
		return;
	}
	cout << name_of_program << ": " << buffer << endl;
}
