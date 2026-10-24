#ifndef MEMORIA_H
#define MEMORIA_H

#include <sys/types.h>
#include <time.h>
#include <sys/shm.h>

#define MALLOC 1
#define SHARED 2
#define CREATESHARED 3
#define MMAP 4

typedef struct {
    void * mem;
    size_t tam;
    time_t fecha;
    int tipo;
    key_t clave;
    char * nombre;
} tMem;


void inicializarMemoria(void);
int buscarHuecoMemoria(void);
void registrarMemoria(int id, void *mem, size_t tam, int tipo, key_t clave, const char *nombre);
void listarMemoria(void);

#endif
