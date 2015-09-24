#ifndef __RESOURCE_H
#define __RESOURCE_H

/* - resource array index discription
 * [0] : cpu_total
 * [1] : cpu_user percentage	(%)
 * [2] : cpu_nice percentage	(%)
 * [3] : cpu_system percentage	(%)
 * [4] : MemTotal				(gㄱ)
 * [5] : MemUse					(KB)
 * [6] : Battery percentage		(%)
 *

 * - /proc/stat
 * 	[0]CPU 		: Cpu core number
 * 	[1]User 	: User mode run time
 *  [2]System 	: System mode run time
 *  [3]Nice		: User mode run time in low authority.
 *  [4]Idle		: None I/O complete time
 *  [5]Wait		: I/O complete time....	(Not used)
 *

 * - /proc/meminfo
 * 	[0]MemTotal	: Max RAM size(Physical + a few reserved bits - kernel binary code)
 * 	[1]MemFree	: LowFree + HighFree
 *
 * - /sys/class/power_supply/battery/capacity
 * 	[0]Battery : rate
 */


float resource[10];

void CpuUsage();
void MemoryUsage();
void BatteryUsage();

#endif
