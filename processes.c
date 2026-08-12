#include "processes.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>

enum State parse_state(char state){
    switch (state) {
        case 'R':
            return RUNNING;

        case 'S':
            return SLEEP;
        case 'I':
        	return IDLE;
        case 'D':
            return UNINTERRUPTIBLE_SLEEP;

        case 'Z':
            return ZOMBIE;

        case 'T':
        case 't':
            return STOPPED;

        default:
            return UNKNOWN;
    }
}

const char *convert_state_enum(enum State state){
	switch (state){
	case RUNNING:
		return "Running";
	case SLEEP:
        return "Sleeping";

    case IDLE:
        return "Idle";

    case UNINTERRUPTIBLE_SLEEP:
        return "Uninterruptible Sleep";

    case ZOMBIE:
        return "Zombie";

    case STOPPED:
        return "Stopped";

    case UNKNOWN:
    default:
        return "Unknown";

}
}



int load_process_by_pid(pid_t pid, Process *process){
	char path[64],line[256],state;
	FILE *file;
	unsigned int ppid;
	unsigned int uid;
	unsigned int gid;
	snprintf(path,sizeof(path),"/proc/%d/status",(int)pid);
	
	if((file = fopen(path,"r")) == NULL){return -1;}

	process->pid = pid;

	while(fgets(line,sizeof(line),file) != NULL){
		if(sscanf(line,"Name: %255s",process->proc_name) == 1){continue;}
		if(sscanf(line,"State: %c",&state) == 1){
			process->state = parse_state(state);
			continue;
		}
		if(sscanf(line,"PPid: %u",&ppid) == 1){
			process->ppid = (pid_t)ppid;
			continue;
		}
		if(sscanf(line,"Uid: %u",&uid) == 1){
			process->uid = (uid_t)uid;
			continue;
		}
		if(sscanf(line,"Gid: %u",&gid) == 1){
			process->gid = (gid_t)gid;
			continue;
		}
	}
	fclose(file);
	return 0;
}

int load_processes(Process process[],size_t max_processes){
	
}

int print_process(Process *process, FILE *log){
	if(process == NULL || log == NULL){return -1;}

	struct passwd *pw = getpwuid(process->uid);

	fprintf(log,"PID: %d\n",process->pid);
	fprintf(log,"Name: %s\n",process->proc_name);
	fprintf(log,"State: %s\n",convert_state_enum(process->state));
	fprintf(log,"PPID: %d\n",process->ppid);
	fprintf(log,"UID: %u (%s)\n",(unsigned)process->uid,pw ? pw->pw_name : "unknown");
	fprintf(log,"-----------------------------\n");

	return 0;
}
