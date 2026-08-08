#ifndef PROCESSES_H
#define PROCESSES_H

enum State{
	RUNNING,
	SLEEP,
	UNINTERRUPTABLE_SLEEP,
	ZOMBIE,
	STOPPED,
	UNKNOWN
};


typedef struct{
	int pid;
	int gid;
	int ppid;
	char *proc_name;
	char *exe_path;
	char *cmdline;
	enum State state;
}Process;

const char *convert_state_enum(enum State state);

int load_processes(Process processes[],int max_processes);

Process *load_process_by_pid(int pid);

int detect_new_processes(
	const Process process[],
	int process_count,
	Process new_processes[],
	int max_new_processes);

#endif