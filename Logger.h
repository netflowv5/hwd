/*
 * Logger.h
 *
 *  Created on: 22 февр. 2016 г.
 *      Author: secator
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

class Logger {
public:
	Logger(const char *data);
	virtual ~Logger();

	void log(const char *data, ...);
	void init(std::string &type);
private:
	std::string name_of_program;
	std::string type;
};

#endif /* LOGGER_H_ */
