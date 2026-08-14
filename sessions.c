#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <utmp.h>

#include "sessions.h"
#include "users.h"

bool convert_name_to_uid(const User users[],size_t user_count,char *username,uid_t *uid){
	for(size_t i = 0; i < user_count; i++){
		if(!strcmp(users[i].username,username)){*uid = users[i].uid;return true;}
	}
	return false;
}


size_t load_sessions(Session sessions[],size_t max_count){
	struct utmp *entry;
	User users[MAX_USERS];
	size_t index = 0,user_count = load_users(users,MAX_USERS);
	setutent();
	while((entry = getutent()) != NULL){
		if(entry->ut_type != USER_PROCESS){continue;}
		if(index >= max_count){break;}
		snprintf(sessions[index].username,sizeof(sessions[index].username),"%s",entry->ut_user);
		snprintf(sessions[index].tty,sizeof(sessions[index].tty),"%s",entry->ut_line);
		snprintf(sessions[index].host,sizeof(sessions[index].host),"%s",entry->ut_host);
		sessions[index].time = entry->ut_time;
		convert_name_to_uid(users,user_count,sessions[index].username,&sessions[index].uid);
		index++;
	}
	endutent();

	return index;
}

size_t get_sessions_by_uid(const Session sessions[],size_t session_count,uid_t uid,Session result[]){
	size_t index = 0;
	for(size_t i = 0; i < session_count; i++){
		if(sessions[i].uid == uid){
			result[index] = sessions[i];
			index++;
		}
	}
	return index;
}

void print_sessions(const Session sessions[],size_t session_count,FILE *log){
	for(size_t i=0;i<session_count;i++){
	fprintf(log,"Username: %s\nTTY: %s\nHost: %s\n",sessions[i].username,sessions[i].tty,sessions[i].host);
	}
}