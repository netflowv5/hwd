/*
 * Config.cpp
 *
 *  Created on: 28 февр. 2016 г.
 *      Author: secator
 */

#include "Config.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

Config::Config() {
	int size = sizeof getenv("HOME");
	char home[size + 7] = "";
	strcpy(home, getenv("HOME"));
	strcat(home, "/.hwdrc");

	string data;
	ifstream stream(home);
	if (!stream) {
		cout << "Config->Config: Open config failed";
		return;
	}
	smatch match;
	regex reg("(.+)=(.+)");
	while (!stream.eof()) {
		getline(stream, data, '\n');
		if (regex_search(data, match, reg)) {
			config.insert(pair<string, string>(match[1], match[2]));
		}
	}
	stream.close();
}

Config::~Config() {
	cout << "Config terminating..." << endl;
}

int Config::GetCount(string iterator) {
	return config.count(iterator);
}

string Config::Get(string iterator, int number_of) {
	if (!config.count(iterator))
		return "";
	auto range = config.equal_range(iterator);
	auto it = range.first;
	for (int i = 0; i < number_of; i++) {
		it++;
	}
	return it->second;
}
