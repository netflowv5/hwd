/*
 * Logger.cpp
 *
 *  Created on: 22 февр. 2016 г.
 *      Author: secator
 */

#include "Logger.h"
#include <iostream>
#include <stdarg.h>
#include <cstring>
#include <syslog.h>
#include <fstream>
#include "Config.h"

using namespace std;

Logger::Logger(const char *data, Config *config_pointer) {
	cout << "Logger instance created. Name of uses program is " << data << "."
			<< endl;
	name_of_program = data;
	if (config_pointer->GetCount("log")) {
		this->type = config_pointer->Get("log", 0);
		cout << "Logger switch out to: " << this->type << "." << endl;
		if (this->type == "syslog") {
			openlog(name_of_program.c_str(), 0, LOG_USER);
		}
		if (this->type == "file") {
			static ofstream realstream;
			string path = "/tmp/hwd.log";
			if (config_pointer->GetCount("logfile")) {
				path = config_pointer->Get("logfile", 0);
			}
			realstream.open(path.c_str(), ofstream::app);
			if (!realstream) {
				this->type = "stdout";
				this->log(
						"Logger->init: can't open log file, switched to stdout.",
						NULL);
			} else
				stream = &realstream;
		}
		this->log("Logger->init: hwd started", NULL);
	}
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
		stream->write(" ", strlen(" "));
		strcat(buffer, "\n");
		stream->write(buffer, strlen(buffer));
		stream->flush();
		return;
	}
	cout << name_of_program << ": " << buffer << endl;
}
