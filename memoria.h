#ifndef MEMORIA_H
#define MEMORIA_H

#include "fichabiertos.h"

#include <sys/types.h>
#include <time.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define T_MALLOC 1
#define T_SHARED 2
#define T_MMAP 3

#define MAX_MEM 2048

typedef struct {
    void * mem;
    size_t tam;
    time_t fecha;
    int tipo;
    key_t clave;
    char * nombre;
    int df;
} tMem;

typedef tMem mList[MAX_MEM];


void inicializarMemoria(void);
//int buscarHuecoMemoria(void);
bool registrarMemoria(void *mem, size_t tam, int tipo, key_t clave, const char *nombre, int df);
void listarMemoria(void);
void liberarRegistroMemoria(int id);
void liberarBloqueMemoria(int id);
int buscarBloqueMalloc(size_t tam);
int buscarBloqueShared(key_t clave);
int buscarBloqueMmap(const char *nombre);
int buscarBloqueDireccion(void *dir);

#endif
