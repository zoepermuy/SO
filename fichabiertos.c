/*
 * AUTORES:
 * Africa Fernandez Loira (africa.fernandez)
 * Zoe Permuy Lobelos (zoe.permuy)
 *
 */
 
#include "fichabiertos.h"


static fList OpenFiles; //mantener en memoria durante todo el programa

void printModeF(int mode) {
    if (mode != -1) {
        if (O_RDWR == (O_RDWR & mode))
            printf("O_RDWR ");
        else if (O_WRONLY == (O_WRONLY & mode))
            printf("O_WRONLY ");
        else if (O_RDONLY == (O_RDONLY & mode))
            printf("O_RDONLY ");
        //Para estos tres primeros se usa else if porque solo puede ser uno de los tres (RDONLY es 0)

        if (O_CREAT == (O_CREAT & mode))
            printf("O_CREAT ");
        if (O_EXCL == (O_EXCL & mode))
            printf("O_EXCL ");
        if (O_APPEND == (O_APPEND & mode))
            printf("O_APPEND ");
        if (O_TRUNC == (O_TRUNC & mode))
            printf("O_TRUNC ");
    }
}

void inicializarListaF() {
    strcpy(OpenFiles[0].name, "entrada estandar");
    strcpy(OpenFiles[1].name, "salida estandar");
    strcpy(OpenFiles[2].name, "error estandar");
    OpenFiles[0].mode = fcntl(0, F_GETFL);
    OpenFiles[1].mode = fcntl(1, F_GETFL);
    OpenFiles[2].mode = fcntl(2, F_GETFL);
    //OpenFiles[0].offset = -1;
    //OpenFiles[1].offset = -1;
    //OpenFiles[2].offset = -1;
    OpenFiles[0].isInit = true;
    OpenFiles[1].isInit = true;
    OpenFiles[2].isInit = true;

    int i;
    for (i=3; i<20; i++) {
        strcpy(OpenFiles[i].name, "no usado");
        OpenFiles[i].mode = -1;
        //OpenFiles[i].offset = -1;
        OpenFiles[i].isInit = true; //se inicializan primero 20 elementos del array
        }
    while (i<FILESLOTS) {
        OpenFiles[i].isInit = false;
        i++;
    }
}


void abrirArchivo(int df, int mode, char* name) {

    if(df>FILESLOTS) {  //poner tambien =
        printf("Error: No se pudo registrar\n");
        return;
    }

    strcpy(OpenFiles[df].name, name);
    OpenFiles[df].mode = mode;
    //OpenFiles[df].offset = 0;
    OpenFiles[df].isInit = true; //por si esta posicion no estaba inicializada
}

void cerrarArchivo(int df) {
    strcpy(OpenFiles[df].name, "no usado");
    OpenFiles[df].mode = -1;
    //OpenFiles[df].offset = -1;
}

void duplicarArchivo(int dfOG, int dfDEST) {
    if (dfDEST > FILESLOTS) {   //poner tambien =
        printf("Error: No se pudo registrar\n");
        return;
    }
    
    OpenFiles[dfDEST].mode = fcntl(dfDEST, F_GETFL);

    char tmp[MAXNAME]; 
    if (snprintf(tmp, MAXNAME, "Duplicado de %d (%s)", dfOG, OpenFiles[dfOG].name)>=MAXNAME) { //intento de resolver el warning
        printf("Error: No se pudo registrar\n");
        return;
    }
    strcpy(OpenFiles[dfDEST].name, tmp);

    OpenFiles[dfDEST].isInit = true;

}

/* implementación con el campo offset

void posicionarOffset(int df, long nuevaPos) {
    OpenFiles[df].offset = nuevaPos;
    //ESTE CAMPO NO COMPENSA PORQUE SI NO SE ACTUALIZA CADA X TIEMPO TODO, LOS DUPLICADOS NO CONCUERDAN CON LOS ORIGINALES
}

void listarFicherosAbiertos() { //VERSIÓN CON CAMPO OFFSET
    for (int i=0; OpenFiles[i].isInit; i++) { //lista los inicializados, no necesariamente ocupados
        printf("descriptor: %d, ", i);
        if (OpenFiles[i].offset == -1)
            printf("offset: ( ) -> ");
        else 
            printf("offset: (%ld)-> ", (long)OpenFiles[i].offset);
        printf("%s ", OpenFiles[i].name);
        printModeF(OpenFiles[i].mode);
        printf("\n");
    }
}
*/

void listarFicherosAbiertos() {
    off_t currentOffset;
    for (int i=0; OpenFiles[i].isInit; i++) { //lista los inicializados, no necesariamente ocupados
        printf("descriptor: %d ", i);
        currentOffset = lseek(i, 0, SEEK_CUR);
        if (currentOffset == -1)
            printf("offset: ( ) -> ");
        else 
            printf("offset: (%ld)-> ", currentOffset);
        printf("%s ", OpenFiles[i].name);
        printModeF(OpenFiles[i].mode);
        printf("\n");
    }
}