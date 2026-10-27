/*
 * AUTORES:
 * Africa Fernandez Loira (africa.fernandez)
 * Zoe Permuy Lobelos (zoe.permuy)
 *
 */
 
#ifndef FICHABIERTOS_H
#define FICHABIERTOS_H

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define FILESLOTS 4096
#define MAXNAME 512


/*LISTA ESTÁTICA: preferente para acceso directo*/
typedef struct {
    char name[MAXNAME];
    int mode;
    //off_t offset;
    bool isInit; //para saber si está inicializado
} tFile;

typedef tFile fList[FILESLOTS]; 

/*FUNCIONES*/
void inicializarListaF(void);
void abrirArchivo(int df, int mode, char* name);
void cerrarArchivo(int df);
void duplicarArchivo(int dfOG, int dfDEST);
//void posicionarOffset(int df, long nuevaPos);
void listarFicherosAbiertos();


#endif