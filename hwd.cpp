//============================================================================
// Name        : hwd.cpp
// Author      : secator
// Version     :
// Copyright   : 
// Description : Hibernate watch dog, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include <string>
#include <regex>
#include <unistd.h>
#include "PcStat.h"

using namespace std;

int main() {

	int counter = 0;
	/* Create HddStat, ScreenSaver Objects */
	PcStat ps;
	//ps.IsNeedProgramRun();
	/* Create Cycle */
	while (true) {
		sleep(60);
		if (!ps.ScreenSaverIsLocked()) {
			counter = 0;
			continue;
		}
		if (ps.HddIsChange()) {
			/*Debug*/cout << "change!!!";
			counter = 0;
			continue;
		}
		if (counter == ps.timer) {
			if (ps.IsNeedProgramRun()) {
				/*Debug*/cout << "Need program run" << endl;
				continue;
			}
			counter = 0;
			/*Debug*/cout << "Time to sleep!!!" << endl;
			system("sudo pm-hibernate");
			continue;
		}
		counter++;
	}

}
