#include "users.h"
#include "processes.h"
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>




int load_users(User users[],size_t max_users){
	size_t index = 0;
	struct passwd *passwd;
	while((passwd = getpwent()) != NULL){
		if(index > max_users){return -1;}
		else{
			snprintf(users[index].username,sizeof(users[index].username),"%s",passwd->pw_name);
			snprintf(users[index].home,sizeof(users[index].home),"%s",passwd->pw_dir);
			snprintf(users[index].shell,sizeof(users[index].shell),"%s",passwd->pw_shell);
			users[index].uid = passwd->pw_uid;
			users[index].gid = passwd->pw_gid;
			index++;
		}
	}
	return index;

}

const User *find_user_by_uid(const User users[],size_t users_count,uid_t uid){
	for(size_t i = 0; i < users_count;i++){
		if(users[i].uid == uid){return &users[i];}
	}
	return NULL;
}

void get_user_stats(const Process processes[],size_t process_count,uid_t uid,UserStats *stats){
	stats->process_count = 0;
	stats->running_count = 0;
	stats->sleeping_count = 0;

	for(size_t i = 0; i < process_count; i++){
		if(processes[i].uid != uid){continue;}
		stats->process_count++;
		if(processes[i].state == RUNNING){stats->running_count++;}
		else if(processes[i].state == SLEEP){stats->sleeping_count++;}
	}

}


void print_users(const User users[],size_t count,FILE *log){
	for(size_t i = 0; i < count; i++){
		fprintf(log,"Username: %s\n",users[i].username);
		fprintf(log,"UID: %d\n",users[i].uid);
		fprintf(log,"GID: %d\n",users[i].gid);
		fprintf(log,"Home Directory: %s\n",users[i].home);
		fprintf(log,"Shell: %s\n",users[i].shell);
		fprintf(log,"--------------------------\n");
	}
}