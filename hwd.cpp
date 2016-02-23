//============================================================================
// Name        : hwd.cpp
// Author      : secator
// Version     :
// Copyright   : 
// Description : Hibernate watch dog, Ansi-style
//============================================================================

#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "PcStat.h"
#include "Logger.h"

using namespace std;

int main() {

	int counter = 0;
	Logger log("hwd"), *plog;
	plog = &log;
	PcStat ps(plog);

	/* Create Cycle */
	while (true) {
		sleep(60);
		if (!ps.ScreenSaverIsLocked()) {
			counter = 0;
			continue;
		}
		if (ps.HddIsChange()) {
			log.log("main: hdd counter change", NULL);
			counter = 0;
			continue;
		}
		if (counter == ps.timer) {
			if (ps.IsNeedProgramRun()) {
				log.log("main: need program run", NULL);
				continue;
			}
			if (ps.IsMusicPlay()) {
				log.log("main: music play", NULL);
				continue;
			}
			counter = 0;
			log.log("main: time to sleep!!!", NULL);
			system("sudo pm-hibernate");
			continue;
		}
		counter++;
	}

}
