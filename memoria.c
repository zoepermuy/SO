#include "memoria.h"


static mList memoria; //mantener en memoria durante todo el programa

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

bool registrarMemoria(void *mem, size_t tam, int tipo, key_t clave, const char *nombre, int df) {
    int id;
    id = buscarHuecoMemoria();
    
    if (id == -1) { //no hay hueco en la lista
        printf("Error: Espacio insuficiente\n");

        switch (tipo) {
            case T_MALLOC:
                free(mem);
                break;
            case T_SHARED:
                shmdt(mem);
                break;
            case T_MMAP:
                munmap(mem, tam);
                close(df);
                break;
        }

        return false;
    }
    
    if (id < 0 || id >= MAX_MEM) {
        printf("Error: índice de memoria fuera de rango\n");
        return false;
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

    return true;
}

void listarMemoria(void) {
    printf("******Lista de bloques asignados para el proceso %d\n", getpid());

    for (int i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL) {
            printf("\t%p \t%ld bytes\n", memoria[i].mem, memoria[i].tam);
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

int buscarBloqueMalloc(size_t tam) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL && memoria[i].tipo == T_MALLOC && memoria[i].tam == tam) {
            return i;
        }
    }
    return -1;
}

void liberarBloqueMemoria(int id) {
    switch (memoria[id].tipo) {
        case T_MALLOC: 
            free(memoria[id].mem);
            break;
        
        case T_SHARED: 
            if (shmdt(memoria[id].mem) == -1) {
                perror("Imposible desasignar memoria compartida");
                return;
            }
            break;
        
        case T_MMAP: 
            if(munmap(memoria[id].mem, memoria[id].tam) == -1) {
                perror("Imposible desasignar fichero mapeado");
                return;
            }

            if(close(memoria[id].df) == -1) {
                perror("Imposible cerrar el descriptor");
            }
            cerrarArchivo(memoria[id].df);
            break;
    }

    liberarRegistroMemoria(id);
}


int buscarBloqueShared(key_t clave) {
    int i;

    for (i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL &&
            memoria[i].tipo == T_SHARED &&
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

void vaciarMemoria() {
    for (int i = 0; i < MAX_MEM; i++) {
        if (memoria[i].mem != NULL) {
            liberarBloqueMemoria(i);
            liberarRegistroMemoria(i); //por si acaso
        }
    }
}