#ifndef DAEMON_H
#define DAEMON_H

int daemonize();
int load_files(FILE **log,FILE **pid_file);


#endif