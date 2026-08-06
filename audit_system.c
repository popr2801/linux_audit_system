#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <stdbool.h>
#include <sys/stat.h>

struct proc{
	int pid,gid,ppid;
	//char  *proc_name = NULL,*exe_path = NULL,*cmdline = NULL,*state = NULL;


};
//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
bool verify_dir(char *dir_name){
	int i=0;
	while(dir_name[i]){
		if(!isdigit(dir_name[i++])){return false;}
	}
	return true;
}

char *convert_UID(int UID){
	struct passwd *user = getpwuid(UID);
	if(user != NULL){
		return user->pw_name;
	}
	return "User not found";
}
char *convert_GID(int GID){

	struct group *grp = getgrgid(GID);
	if(grp != NULL){
		return grp->gr_name;
	}
	return "Group not found";
}
void process_monitor(char *dir_path){
	DIR *dir;
	struct dirent *entry;
	dir = opendir(dir_path);
	char path[PATH_MAX];
	if(dir == NULL){
		perror("opendir");
	}
	while((entry = readdir(dir)) != NULL){
		 if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0  || (entry->d_type != DT_DIR) || !(verify_dir(entry->d_name)))
            continue;
			
        	snprintf(path,sizeof(path),"%s/%s",dir_path,entry->d_name);
			printf("PID: %s\n",entry->d_name);
			struct stat buf;
			if(stat(path,&buf) == -1){
				continue;
			}
			printf("Owner: %s\nGroup: %s\nPermissions %o\n------------\n",convert_UID(buf.st_uid),convert_GID(buf.st_gid),buf.st_mode & 0777);
		}


	closedir(dir);

}



int main(){

	char *username = convert_UID(0);
	process_monitor("/proc");

	printf("User: %s\n",username);

	return 0;
}
