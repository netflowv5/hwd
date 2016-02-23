/*
 * ScreenSaver.cpp
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: secator
 */

#include "PcStat.h"

#include <string>
#include <iostream>
#include <regex>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

PcStat::PcStat(Logger *log_link) {
	log = log_link;
	old_counter = 0;
	new_counter = 0;
	program_counter = 0;
	music = false;

	string data;
	ifstream stream("/home/secator/.hwdrc");
	if (!stream) {
		log->log("PcStat->PcStat: Open config failed", NULL);
		exit(1);
	}
	int counter = 0;
	smatch match;
	regex reg_timer("timer=([0-9]{1,3})");
	regex reg_program("p=(.+)");
	regex reg_music("music=on");
	regex reg_config("config=(.+)");

	while (!stream.eof()) {
		if (counter > MAX)
			break;
		getline(stream, data, '\n');
		if (regex_search(data, match, reg_config)){
					string type = match[1];
					log->init(type);
					log->log("PcStat->Pcstat: config: config type:", type.c_str(), NULL);
					continue;
		}
		if (regex_search(data, match, reg_timer)) {
			timer = stoi(match[1]);
			continue;
		}
		if (regex_search(data, match, reg_program)) {
			programs[program_counter] = match[1];
			program_counter++;
			continue;
		}
		if (regex_search(data, match, reg_music)) {
			log->log("PcStat->PcStat: config:", "music on", NULL);
			music = true;
			continue;
		}
		counter++;
	}
	stream.close();
}

PcStat::~PcStat() {
	// TODO Auto-generated destructor stub
}

bool PcStat::IsMusicPlay() {
	FILE *pipe = popen("pamon --device=2", "r");
	if (!pipe) {
		log->log("PcStat->IsMusicPlay:", "failed open pamon", NULL);
		return EXIT_FAILURE;
	} else {
		char buf[1] = "";
		fread(&buf[0], sizeof buf[0], sizeof(buf), pipe);
		pclose(pipe);
		log->log("PcStat->IsMusicPlay: Buff:", buf, NULL);
		if (strcmp(buf, ""))
			return true;
	}
	return false;
}

bool PcStat::IsNeedProgramRun() {
	int status = -1;
	pid_t pid;
	char exec_arg[64] = "";
	for (int k = 0; k < program_counter; k++) {
		strcpy(exec_arg, programs[k].c_str());
		log->log("PcStat->IsNeedProgramRun:", exec_arg, NULL);
		pid = fork();
		if (pid == 0)
			execlp("pgrep", "pgrep", exec_arg, NULL);
		else if (pid < 0) {
			log->log("PcStat->IsNeedProgramRun: pgrep exec failed", NULL);
			return false;
		} else
			waitpid(pid, &status, 0);
		char cstatus[16] = "";
		sprintf(cstatus, "%d", status);
		log->log("PcStat->IsNeedProgramRun: Status:", cstatus, NULL);
		if (status == 0) {
			return true;
		}
	}
	return false;
}

bool PcStat::ScreenSaverIsLocked() {
	/* Read data from xscreensaver pipe */
	string xscreensaver_data;
	FILE *xscreensaver_pipe = popen("xscreensaver-command -time", "r");
	if (!xscreensaver_pipe) {
		log->log("PcStat->ScreenSaverIsLocked: popen xscreensaver failed",
		NULL);
		return EXIT_FAILURE;
	} else {
		char buf[128] = "";
		//		while(!feof(fp)){
		fread(&buf[0], sizeof buf[0], sizeof(buf), xscreensaver_pipe);
		//		}
		xscreensaver_data = buf;
		pclose(xscreensaver_pipe);
	}
	if (parse_ss(xscreensaver_data))
		return true;
	return false;
}

bool PcStat::HddIsChange() {
	/* Read hdd stats */
	string hdd_data;
	ifstream hdd_stream("/sys/block/sda/stat");
	if (!hdd_stream) {
		log->log("PcStat->HddIsChange: open /sys/block/sda/stat failed", NULL);
		return EXIT_FAILURE;
	}
	getline(hdd_stream, hdd_data, '\0');
	hdd_stream.close();
	smatch str_match;
	regex reg("[0-9]{1,20}");
	regex_search(hdd_data, str_match, reg);
	new_counter = stoi(str_match[0]);
	hdd_data = str_match[0];
	log->log("PcStat->HddIsChange: counter:", hdd_data.c_str(), NULL);
	if (old_counter == 0) {
		old_counter = new_counter;
		return false;
	}
	if (old_counter == new_counter)
		return false;
	old_counter = 0;
	return true;
}

bool PcStat::parse_ss(string &data) {
	/*
	 //########################### Regex for minute parse ###############################
	 smatch str_match;
	 regex reg("n blanked.+"
	 "([0-9]{2}) "
	 "([0-9]{2})"
	 ":([0-9]{2})");
	 if (regex_search(data, str_match, reg)) {
	 Debugcout << "finded:" << str_match.size() << endl;
	 Debugfor (size_t i = 0; i < str_match.size(); i++) {
	 Debugcout << str_match[i] << endl;
	 Debug}
	 } else {
	 Debugcout << "not blanked" << endl;
	 return EXIT_FAILURE;
	 }
	 int day, hour, minute;
	 day = stoi(str_match[1]);
	 hour = stoi(str_match[2]);
	 minute = stoi(str_match[3]);
	 Debugcout << "int day:" << day << " int hour:" << hour << " int minute:"
	 << minute << endl;
	 //##################################################################################
	 */
	smatch match;
	regex reg("locked");
	cout << data;
	if (regex_search(data, match, reg))
		return true;
	return false;
}
