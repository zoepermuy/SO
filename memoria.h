#ifndef MEMORIA_H
#define MEMORIA_H

#include <sys/types.h>
#include <time.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MALLOC 1
#define SHARED 2
#define CREATESHARED 3
#define MMAP 4

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
int buscarHuecoMemoria(void);
void registrarMemoria(int id, void *mem, size_t tam, int tipo, key_t clave, const char *nombre, int df);
void listarMemoria(void);
void liberarRegistroMemoria(int id);
int buscarBloqueMemoria(size_t tam);
int buscarBloqueShared(key_t clave);
int buscarBloqueMmap(const char *nombre);
int buscarBloqueDireccion(void *dir);

#endif
