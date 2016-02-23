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

	int log(const char *data, ...);
private:
	std::string name_of_program;
};

#endif /* LOGGER_H_ */
