/*
 * Logger.h
 *
 *  Created on: 22 февр. 2016 г.
 *      Author: secator
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>
#include "Config.h"

class Logger {
public:
	Logger(const char *data, Config *config_pointer);
	virtual ~Logger();

	void log(const char *data, ...);
private:
	std::string name_of_program;
	std::string type;
	std::ofstream *stream;
};

#endif /* LOGGER_H_ */
