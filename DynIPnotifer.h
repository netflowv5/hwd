/*
 * DynIPnotifer.h
 *
 *  Created on: 26 февр. 2016 г.
 *      Author: secator
 */

#ifndef DYNIPNOTIFER_H_
#define DYNIPNOTIFER_H_

#include <string>
#include "Config.h"
#include "Logger.h"

class DynIPnotifer {
public:
	DynIPnotifer(Config *conf, Logger *log_link);
	virtual ~DynIPnotifer();
	void Main();

private:
	bool SendMail();
	std::string GetIP();

	Logger *log;
	std::string getip, ip;
	std::string smtp, to, from, password;
	int getiptimer, counter;
};

#endif /* DYNIPNOTIFER_H_ */
