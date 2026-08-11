#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdio.h>


typedef struct{
	char hostname[256];
	char kernel[256];
	char os_name[128];
	char architecture[64];
	char current_user[64];

	char cpu_model[256];
	unsigned int cpu_cores;
	
	unsigned long total_ram;
	unsigned long available_ram;
	double memory_usage;
	unsigned long uptime;
}SystemInfo;


double get_memory_usage(unsigned long *total_ram,unsigned long *available_ram);
int get_system_info(SystemInfo *info);
void print_system_info(SystemInfo *info,FILE *output);

#endif
