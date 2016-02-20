/*
 * ScreenSaver.h
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: secator
 */

#ifndef PCSTAT_H_
#define PCSTAT_H_

#include <string>

#define MAX 20

class PcStat {
public:
	PcStat();
	virtual ~PcStat();

	bool ScreenSaverIsLocked();
	bool HddIsChange();
	bool IsNeedProgramRun();

	int timer;
	int program_counter;

private:
	bool parse_ss(std::string data);
	int old_counter, new_counter;
	int parse_hdd(std::string data);

	std::string programs[MAX];
};

#endif /* PCSTAT_H_ */
