/*
 * Pcstat.cpp
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
#include "Config.h"
#include "DynIPnotifer.h"

using namespace std;

PcStat::PcStat(Logger *log_pointer, Config *config_pointer) {
	this->log = log_pointer;
	this->conf = config_pointer;
	old_counter = 0;
	new_counter = 0;
	program_counter = 0;
	if (conf->GetCount("timer")) {
		this->timer = stoi(conf->Get("timer", 0));
	} else
		timer = 5;
	if (conf->GetCount("music")) {
		if (conf->Get("music", 0) == "on") {
			log->log("PcStat->PcStat: config:", "music is on", NULL);
			this->music = true;
		}
	} else
		music = false;
	if (conf->GetCount("program")) {
		this->programs = new string[conf->GetCount("program")];
		for (int i = 0; i < conf->GetCount("program"); i++) {
			this->programs[i] = conf->Get("program", i);
			this->program_counter++;
		}
	}
	if (conf->GetCount("DynIPnotifer") && conf->Get("DynIPnotifer", 0) == "on")
		log->log("PcStat->PcStat: config:", "DynIPnotifer is on", NULL);
	if (conf->GetCount("xscreensaver")) {
		this->xscreensaver = conf->Get("xscreensaver", 0);
	} else
		this->xscreensaver = "locked";
}

PcStat::~PcStat() {
	delete[] programs;
	delete log;
	delete conf;
	cout << "PcStat terminating..." << endl;
}

void PcStat::Main() {
	DynIPnotifer notifer(conf, log);
	int counter = 0;
	while (true) {
		counter++;
		sleep(60);
		if (!this->ScreenSaverIsLocked()) {
			counter = 0;
			continue;
		}
		if (conf->GetCount("DynIPnotifer")
				&& conf->Get("DynIPnotifer", 0) == "on") {
			if (this->IsSSHDrun()) {
				log->log("PcStat->Main: DynIPnotifer is on and sshd is run.",
				NULL);
				notifer.Main();
				counter = 0;
				continue;
			}
		}
		if (this->HddIsChange()) {
			log->log("PcStat->Main: hdd counter change", NULL);
			counter = 0;
			continue;
		}
		if (counter == timer) {
			if (this->IsNeedProgramRun()) {
				log->log("PcStat->Main: need program run", NULL);
				counter = 0;
				continue;
			}
			if (this->music) {
				if (this->IsMusicPlay()) {
					log->log("PcStat->Main: music play", NULL);
					counter = 0;
					continue;
				}
			}
			counter = 0;
			log->log("PcStat->Main: time to sleep!!!", NULL);
			system("sudo pm-hibernate");
			continue;
		}
	}
}

bool PcStat::IsMusicPlay() {
	int size = sizeof "pamon --device=";
	char file[size + 1] = "pamon --device=";
	if (conf->GetCount("device"))
		strcat(file, conf->Get("device", 0).c_str());
	else
		strcat(file, "0");
	FILE *pipe = popen(file, "r");
	if (!pipe) {
		log->log("PcStat->IsMusicPlay:", "failed open pamon", NULL);
		return false;
	} else {
		char buf[16] = "";
		fread(&buf[0], sizeof buf[0], sizeof(buf), pipe);
		pclose(pipe);
		log->log("PcStat->IsMusicPlay: Buff: |", buf, "|", NULL);
		if (strcmp(buf, ""))
			return true;
	}
	return false;
}

bool PcStat::IsNeedProgramRun() {
	for (int k = 0; k < program_counter; k++) {
		char file[64] = "pgrep ";
		strcat(file, programs[k].c_str());
		FILE *pipe = popen(file, "r");
		if (!pipe) {
			log->log("PcStat->IsNeedProgramRun:", "failed open pgrep", NULL);
			return false;
		} else {
			char buf[16] = "";
			fread(&buf[0], sizeof buf[0], sizeof(buf), pipe);
			pclose(pipe);
			log->log("PcStat->IsNeedProgramRun: pid:", buf, NULL);
			if (!strcmp(buf, ""))
				continue;
			else
				return true;
		}
		return false;
	}
	return false;
}

bool PcStat::IsSSHDrun() {
	char file[] = "pgrep sshd";
	FILE *pipe = popen(file, "r");
	if (!pipe) {
		log->log("PcStat->IsSSHDrun:", "failed open pgrep", NULL);
		return false;
	} else {
		char buf[16] = "";
		fread(&buf[0], sizeof buf[0], sizeof(buf), pipe);
		pclose(pipe);
		log->log("PcStat->IsSSHDrun: pid:", buf, NULL);
		if (strcmp(buf, ""))
			return true;
	}
	return false;
}

bool PcStat::ScreenSaverIsLocked() {
	string xscreensaver_data;
	FILE *xscreensaver_pipe = popen("xscreensaver-command -time", "r");
	if (!xscreensaver_pipe) {
		log->log("PcStat->ScreenSaverIsLocked: popen xscreensaver failed",
		NULL);
		exit(1);
	} else {
		char buf[128] = "";
		//		while(!feof(fp)){
		fread(&buf[0], sizeof buf[0], sizeof(buf), xscreensaver_pipe);
		//		}
		xscreensaver_data = buf;
		pclose(xscreensaver_pipe);
	}
	smatch match;
	regex reg(this->xscreensaver);
	if (regex_search(xscreensaver_data, match, reg))
		return true;
	return false;
}

bool PcStat::HddIsChange() {
	string hdd_data;
	ifstream hdd_stream("/sys/block/sda/stat");
	if (!hdd_stream) {
		log->log("PcStat->HddIsChange: open /sys/block/sda/stat failed", NULL);
		exit(1);
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
