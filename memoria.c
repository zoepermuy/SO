#include "memoria.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MEM 2048

tMem memoria[MAX_MEM];

void inicializarMemoria() {
    for (int i = 0; i < MAX_MEM; i++) {
        memoria[i].mem = NULL;
        memoria[i].tam = 0;
        memoria[i].fecha = 0;
        memoria[i].tipo = 0;
        memoria[i].clave = 0;
        memoria[i].nombre = NULL;
        memoria[i].df = -1;
    }
}

int buscarHuecoMemoria() {
    for (int i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem == NULL) {
            return i;
        }
    }
    return -1; // No hay hueco disponible
}

void registrarMemoria(int id, void *mem, size_t tam, int tipo, key_t clave, const char *nombre, int df) {
    if (id < 0 || id >= MAX_MEM) {
        printf("Error: índice de memoria fuera de rango\n");
        return;
    }
    memoria[id].mem = mem;
    memoria[id].tam = tam;
    memoria[id].fecha = time(NULL);
    memoria[id].tipo = tipo;
    memoria[id].clave = clave;
    memoria[id].df = df;
    if (nombre == NULL) {
        memoria[id].nombre = NULL;
     } else {
        memoria[id].nombre = strdup(nombre);
    }
}

void listarMemoria(void) {
    printf("******Lista de bloques asignados para el proceso %d\n", getpid());

    for (int i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL) {
            printf("\t%p \t%zu bytes\n", memoria[i].mem, memoria[i].tam);
        }
    }
}

void liberarRegistroMemoria(int id) {
    if (id < 0 || id >= MAX_MEM) {
        return;
    }

    free(memoria[id].nombre);

    memoria[id].mem = NULL;
    memoria[id].tam = 0;
    memoria[id].fecha = 0;
    memoria[id].tipo = 0;
    memoria[id].clave = 0;
    memoria[id].nombre = NULL;
    memoria[id].df = -1;
}

int buscarBloqueMemoria(size_t tam) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL && memoria[i].tipo == 1 && memoria[i].tam == tam) {
            return i;
        }
    }
    return -1;
}

int buscarBloqueShared(key_t clave) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL &&
            (memoria[i].tipo == 2 ||
             memoria[i].tipo == 3) &&
            memoria[i].clave == clave) {
            return i;
        }
    }
    return -1;
}

int buscarBloqueMmap(const char *nombre) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if(memoria[i].mem != NULL && memoria[i].tipo == 4 && strcmp(memoria[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

int buscarBloqueDireccion(void *dir) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem == dir) {
            return i;
        }
    }
    return -1;
}