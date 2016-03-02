/*
 * Config.h
 *
 *  Created on: 28 февр. 2016 г.
 *      Author: secator
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <map>
#include <string>

class Config {
public:
	Config();
	virtual ~Config();
	int GetCount(std::string iterator);
	std::string Get(std::string iterator, int number_of);
private:
	std::multimap<std::string, std::string> config;
};

#endif /* CONFIG_H_ */
