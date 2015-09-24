#include <string.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <jni.h>

#include "resource.h"

void CpuUsage(){
	FILE *stat = fopen("/proc/stat", "r");
	static int  pre_component[7] = { 0, 0, 0, 0, 0, 0, 0 },
				component[7] = { 0, 0, 0, 0, 0, 0, 0 };
	int i;
	char cpu[10];

	// /proc/stat file's content can check in resource header file.
	fscanf(stat,"%s %d %d %d %d %d %d %d", cpu, &component[0], &component[1],
			&component[2], &component[3], &component[4], &component[5],
			&component[6]);

	fclose(stat);

	// Cpu information(used time) is accumulated. So Current value need to calculation.
	resource[0] = 0; // resource[0] : CPU total run time
	for(i=0; i<5; i++){
		resource[0] += (component[i] - pre_component[i]);
	}
	for(i=1; i<3; i++){
		resource[i] = (component[i-1]-pre_component[i-1]) *100 / resource[0];
	}
	// Current value change Previous value
	for(i=0; i<5; i++){
		pre_component[i] = component[i];
	}
}

void MemoryUsage(){
	FILE *mem = fopen("/proc/meminfo", "r");
	char name[2][20], byte[10];
	int data[2];

	// Detail content can check in resource header file.
	fscanf(mem,"%s %d %s", name[0], &data[0], byte);
	fscanf(mem,"%s %d %s", name[1], &data[1], byte);

	resource[4] = data[0];
	resource[5] = data[0] - data[1];
	fclose(mem);
}

void BatteryUsage(){
	/*
	 * /sys/class/power_supply/battery/capacity file using is not good choice.
	 * this path anytime can is changed by vendor or os version.
	 */
	FILE *bat = fopen("/sys/class/power_supply/battery/capacity", "r");
	int data;

	fscanf(bat, "%d", &data);
	resource[6] = data;

	fclose(bat);
}
