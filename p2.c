/*
 * AUTORES:
 * Africa Fernandez Loira (africa.fernandez)
 * Zoe Permuy Lobelos (zoe.permuy)
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#include "start.h"


#define MAXENTRADA 2048

struct COMANDO{
  char * nombre;
  void (*funcion)(char**);
  char * help;
};

/**************************SHELL**************************/


void Cmd_help (char *arg[]);

static struct COMANDO C[]={
   {"fin",Cmd_fin, "\tTermina la ejecucion del shell"},
   {"exit",Cmd_fin, "\tTermina la ejecucion del shell"},
   {"quit",Cmd_fin, "\tTermina la ejecucion del shell"},
   {"bye",Cmd_fin, "\tTermina la ejecucion del shell"},
   {"pid",Cmd_pid, "[-p]\tMuestra el pid del shell o de su proceso padre"},
   {"pwd", Cmd_pwd, "\tMuestra el directorio actual del shell"},
   {"chdir",Cmd_chdir, "[dir]\tCambia (o muestra) el directorio actual del shell"},
   {"authors",Cmd_autores, "[-n|-l]\tMuestra los nombres y/o logins de los autores"},
   {"exec",Cmd_exec, "prog args...\tEjecuta un programa prog con sus argumentos"},
   {"pplano",Cmd_pplano, "prog args...\tCrea un proceso que ejecuta en primer plano prog con argumentos"},
   {"splano",Cmd_splano, "prog args...\tCrea un proceso que ejecuta en segundo plano prog con argumentos"},
   {"path",Cmd_path, "[-add|-del|-clear|-path]..\tManipula o muestra la ruta de busqueda del shell (path)\n"
      "\t-add dir: aniade dir a la ruta de busqueda\n"
      "\t-del dir: elimina dir de la ruta de busqueda\n"
      "\t-clear: vacia la ruta de busqueda\n"
      "\t-path: importa el PATH en la ruta de busqueda\n"
      "\t-show: imprime por pantalla el contenido de PATH\n"
      "\t-clear: borra el contenido de PATH"},
   {"importpath",Cmd_importpath, "\tImporta el PATH en la ruta de busqueda"},
   {"where",Cmd_where, "file\tDice en que directorio esta el fichero file"},
   {"date", Cmd_date, "[-d|-t]\tMuestra la fecha y/o la hora actual"},
   {"sysinfo", Cmd_sysinfo, "\tMuestra informacion de la maquina donde corre el shell"},
   {"help", Cmd_help, "[cmd]\tLista todos los comandos disponibles o da informacion sobre el comando cmd"},
   {"open", Cmd_open, "[file m1 m2]\tAbre el fichero fich y lo añade a la lista de ficheros abiertos del shell\n"
      "\tm1, m2.. es el modo de apertura (or bit a bit de los siguientes)\n"
      "\tcr: O_CREAT\tap: O_APPEND\n"
      "\tex: O_EXCL\tro: O_RDONLY\n"
      "\trw: O_RDWR\two: O_WRONLY\n"
      "\ttr: O_TRUNC"},
   {"close", Cmd_close, "df [-f]\tCierra el descriptor df y lo elimina de la lista de ficheros abiertos\n"
      "\t-f: cierra el descriptor aunque corresponda a un mapeo activo"},
   {"dup", Cmd_dup, "df\tDuplica el descriptor de fichero df y aniade una nueva entrada a la lista ficheros abiertos"},
   {"lseek", Cmd_lseek, "df off ref\tPosiciona el offset en df en off. ref es la referencia:\n"
      "\tSEEK_SET principio fichero\n"
      "\tSEEK_CUR posicion actual\n"
      "\tSEEK_END final del fichero"},
   {"listopen", Cmd_listopen, "Muestra la lista de ficheros abiertos"},
   {"readstr", Cmd_readstr, "df cont\tLee cont bytes del fichero con descriptor df y los muestra en pantalla"},
   {"writestr", Cmd_writestr, "df string\tEscribe el string string en el fichero descrito por df"},
   {"makefile", Cmd_makefile, "[name]\tCrea un fichero de nombre name"},
   {"makedir", Cmd_makedir, "[name]\tCrea un directorio de nombre name"},
   {"delete", Cmd_delete, "[name1 name2 ..]\tBorra ficheros o directorios vacios"},
   {"deltree", Cmd_deltree, "[name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente"},
   {"listfile", Cmd_listfile, "[-long][-link][-acc] n1 n2 ..\tLista ficheros;\n"
      "\t-long: listado largo\n"
      "\t-acc: accesstime\n"
      "\t-link: si es enlace simbolico, el path contenido"},
   {"list", Cmd_list, "[-reca][-recb][-hid][-long][-link][-acc] n1 n2 ..\t Lista contenidos de directorios\n"
      "\t-long: listado largo\n"
      "\t-hid: incluye los ficheros ocultos\n"
      "\t-recb: recursivo (antes)\n"
      "\t-reca: recursivo (despues)\n"
      "\t-acc: accesstime\n"
      "\t-link: si es enlace simbolico, el path contenido"},
   {"allocate", Cmd_allocate, "[-malloc][-shared][-createshared][-mmap] tam\tReserva bloques de memoria\n"},
   {NULL,NULL},
  };




void Cmd_help (char *arg[])
{
   int i;
   if (arg[0]==NULL) { //Listar posibles comandos
      printf("Cmds disponibles: ");
      for (i=0; C[i].nombre!=NULL; i++) {
         printf("%s ", C[i].nombre);
      }
      printf("\n");
   }

   else {
      for (i=0; C[i].nombre!=NULL; i++) {
         if (!strcmp(C[i].nombre, arg[0])) {
            printf("%s:%s\n", C[i].nombre, C[i].help);
            return;
         }
      }
      printf("%s: no encontrado\n", arg[0]);
   }
}

void DecidirComando (char *tr[])
{
  int i;

  if (tr[0]==NULL) return; /*superfluo, por si cambio otras cosas */
  for (i=0; C[i].nombre!=NULL; i++)
    if (!strcmp(C[i].nombre,tr[0])){
        (*C[i].funcion)(tr+1);
       return;
    }
  Cmd_pplano(tr); /*si no es un comando, es un  ejecutable externo: en primer plano*/
}

int TrocearCadena(char * cadena, char * trozos[])
{
  int i=1;
  if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
      return 0;
  while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
     i++;
  return i;
}

void ProcesarEntrada(char * entrada)
{
   char *tr[MAXENTRADA/2];
   if (TrocearCadena(entrada,tr)==0) /*no hay nada*/
      return;
   DecidirComando(tr);
}

int  main(int argc, char *argv[], char *ent[])
{
   inicializarListaF();
   char entrada[MAXENTRADA];

   if (argv[1]==NULL)
        printf ("Ejecutando con path vacio: %s -p para importar el path\n",argv[0]);
   else if (!strcmp(argv[1],"-p"))
        Cmd_importpath(NULL);

   while (1){
      printf ("-> ");
      fgets(entrada,MAXENTRADA,stdin);
      ProcesarEntrada(entrada);
   }
}
