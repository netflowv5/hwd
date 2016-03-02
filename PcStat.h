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
#include "Config.h"

class PcStat {
public:
	PcStat(Logger *log_link);
	virtual ~PcStat();
	void Main();

private:
	bool ScreenSaverIsLocked();
	bool HddIsChange();
	bool IsNeedProgramRun();
	bool IsMusicPlay();
	bool IsSSHDrun();
	bool parse_ss(std::string &data);

	int old_counter, new_counter, program_counter, timer;
	//std::string programs[MAX];
	std::string *programs;
	bool music;
	Logger *log;
	Config *conf;
};

#endif /* PCSTAT_H_ */
