/*
 * PcStat.h
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: secator
 */

#ifndef PCSTAT_H_
#define PCSTAT_H_

#include <string>
#include "Logger.h"

#define MAX 20

class PcStat {
public:
	PcStat(Logger *log_link);
	virtual ~PcStat();
	void Main();

	int program_counter;
private:

	bool ScreenSaverIsLocked();
	bool HddIsChange();
	bool IsNeedProgramRun();
	bool IsMusicPlay();
	bool parse_ss(std::string &data);
	int old_counter, new_counter;

	int timer;
	std::string programs[MAX];
	bool music;
	Logger *log;
};

#endif /* PCSTAT_H_ */
