/*
 * AUTORES:
 * Africa Fernandez Loira (africa.fernandez)
 * Zoe Permuy Lobelos (zoe.permuy)
 *
 */
 
#ifndef START_H
#define START_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "path.h"
#include "fichabiertos.h"
#include "memoria.h"

#define MAXNOMBRE 1024

void Cmd_fin (char * arg[]);
void Cmd_autores(char *arg[]);
void Cmd_exec (char *arg[]);
void Cmd_splano (char *arg[]);
void Cmd_pplano (char *arg[]);
void Cmd_chdir (char * arg[]);
void Cmd_pwd(char * arg[]);
void Cmd_pid (char * arg[]);
void Cmd_path (char * arg[]);
void Cmd_importpath (char *arg[]);
void Cmd_where (char *args[]);
void Cmd_date (char *arg[]);
void Cmd_sysinfo (char *arg[]);
void Cmd_open (char *arg[]);
void Cmd_close (char *arg[]);
void Cmd_listopen (char *arg[]);
void Cmd_dup(char *arg[]);
void Cmd_lseek(char *arg[]);
void Cmd_readstr(char *arg[]);
void Cmd_writestr(char *arg[]);
void Cmd_makefile(char *arg[]);
void Cmd_makedir(char *arg[]);
void Cmd_delete(char *arg[]);
void Cmd_deltree(char *arg[]);
void Cmd_listfile(char *arg[]);
void Cmd_list(char *arg[]);
void Cmd_allocate(char *arg[]);
void Cmd_deallocate(char *arg[]);
void Cmd_memfill(char *arg[]);
void Cmd_memdump(char *arg[]);



#endif
