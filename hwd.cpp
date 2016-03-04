//============================================================================
// Name        : hwd.cpp
// Author      : secator
// Version     :
// Copyright   : 
// Description : Hibernate watch dog, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <cstring>
#include "PcStat.h"
#include "Logger.h"
#include "Config.h"

using namespace std;

PcStat *pps;

void signal_handler(int sig) {
	delete pps;
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		if (!strcmp(argv[1], "-d")) {
			int pid = fork();
			if (pid == -1) {
				cout << "Error: Start Daemon failed." << endl;
				return -1;
			} else if (!pid) {
				cout << "Demonize..." << endl;
				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);
				signal(SIGHUP, signal_handler);
				signal(SIGTERM, signal_handler);
				Config *pconf = new Config;
				Logger *plog = new Logger("hwd", pconf);
				pps = new PcStat(plog, pconf);
				pps->Main();
				return 0;
			} else
				return 0;
		}
		cout << "Usage: hwd [options]" << endl << "  -d\t daemon mode." << endl;
	} else {
		signal(SIGINT, signal_handler);
		signal(SIGHUP, signal_handler);
		signal(SIGTERM, signal_handler);
		Config *pconf = new Config;
		Logger *plog = new Logger("hwd", pconf);
		pps = new PcStat(plog, pconf);
		pps->Main();
		return 0;
	}
}

