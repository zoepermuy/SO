/*
 * AUTORES:
 * Africa Fernandez Loira (africa.fernandez)
 * Zoe Permuy Lobelos (zoe.permuy)
 *
 */

#include "start.h"


void AniadirAlPath(char *dir)
{
    if (dir==NULL || !strcpy("", dir))
        PathPrint();
    else if (PathAdd(dir)==-1)
        perror("Imposible aniadir");
}

void EliminarDelPath(char *dir)
{
    if (dir==NULL || !strcpy("", dir))
        PathPrint();
    else if (PathDel(dir)==-1)
        perror("Imposible eliminar");
}


void MostrarDirActual()
{
   char dir[MAXNOMBRE];
   
    if (getcwd(dir,MAXNOMBRE)==NULL)
    perror("Imposible obtener directorio");
    else
       printf ("%s\n",dir);
}

int ComprobarSegundoPlano (char *tr[])
{
    int i;
    for (i=0; tr[i]!=NULL;i++)
        if (!strcmp(tr[i],"&")){ /*& indica segundo plano*/ 
            tr[i]=NULL;         /*es el ultimo argumento*/
            return i;       /*si solo hay un & no se ejecuta nada en pplano*/
            }
    return 0;
}


void Proceso (char *tr[], int splano)
{
   pid_t pid;
   void Cmd_exec (char **);
   int background=splano || ComprobarSegundoPlano(tr);
   if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
        }
  if (pid==0){  /*proceso hijo*/
    Cmd_exec (tr);
    exit(255); /*por si falla exec*/
    }
  if (!background) 
    waitpid(pid,NULL,0);
}

/*********************************************/
/*************COMANDOS DEL SHELL************************/
void Cmd_fin (char * arg[])  /*todos los cmd_ comparten prototipo*/
{                            /*reciben los mismos parametros aunque no los usen*/
    vaciarMemoria();
    exit(0);
}

void Cmd_autores(char *arg[])
{
    printf ("Autores del shell:\n");
    if (arg[0]==NULL||!strcmp(arg[0], "-n")){
        printf("Nombres:\n- Africa Fernandez\n- Zoe Permuy\n");
    }
    if (arg[0]==NULL||!strcmp(arg[0], "-l"))
        printf("Logins:\n- africa.fernandez\n- zoe.permuy\n");
}

void Cmd_exec (char *arg[])
{
    if (execv(Ejecutable(arg[0]),arg)==-1)
        perror ("Imposible ejecutar");
}

void Cmd_splano (char *arg[])
{
  Proceso (arg,1);
}
void Cmd_pplano (char *arg[])
{
  Proceso(arg,0);
}

void Cmd_chdir (char * arg[])
{
   if (arg[0]==NULL)
      MostrarDirActual();
   else if (chdir(arg[0])==-1)
      perror("Imposible cambiar directorio");
}

void Cmd_pwd(char * arg[])
{
    MostrarDirActual();
}

void Cmd_pid (char * arg[])
{
    if (arg[0]==NULL)
        printf ("El pid del proceso es %d\n",(int) getpid());
    else
        if (!strcmp (arg[0],"-p"))
            printf ("El pid del proceso padre es %d\n",(int) getppid());
}

void Cmd_where (char *args[])
{
    if (args[0]==NULL)
        printf ("uso: where ejecutable. Indica donde ejecutable está en el path\n");
    else
        printf ("%s\n",Ejecutable (args[0]));
}


void Cmd_path(char *arg[])
{
    if (arg[0]==NULL)
        PathPrint();
    else if (!strcmp(arg[0],"-add"))
        AniadirAlPath (arg[1]);
    else if (!strcmp(arg[0],"-del"))
        EliminarDelPath (arg[1]);
    else if (!strcmp(arg[0],"-show"))
        PathPrint ();
    else if (!strcmp(arg[0],"-clear"))
        PathClear ();
    else if (!strcmp(arg[0],"-path"))
        PathAddPath ();
    else if (arg[0][0]=='-')
        EliminarDelPath(arg[0]+1);
    else if (arg[0][0]=='+')
        AniadirAlPath(arg[0]+1);
    else printf ("Opciones validas: -add/+|-del/-|-show|-clear|-path\n");
} 

void Cmd_importpath (char *arg[])
{
    PathAddPath();
}

void Cmd_date (char *arg[])
{
    time_t tmp = time(NULL);

    if(tmp == (time_t)-1){
        perror("Imposible obtener la hora del sistema");
        return;
    }

    struct tm *date = localtime(&tmp);

    if (date==NULL) {
        perror("Imposible obtener la hora del sistema");
        return;
    }

    if (arg[0]==NULL || !strcmp(arg[0], "-d")) {
        printf("%d/%d/%d\n", date->tm_mday, date->tm_mon+1, date->tm_year+1900);
    }
    if (arg[0]==NULL || !strcmp(arg[0], "-t")) {
        printf("%d:%d:%d\n", date->tm_hour, date->tm_min, date->tm_sec);
    }
    
}

void Cmd_sysinfo (char *arg[]) {
    struct utsname i;
    
    if (uname(&i)==-1) {
        perror("Imposible obtener informacion del sistema");
        return;
    }
    
    printf("%s(%s), OS: %s-%s-%s\n", i.nodename, i.machine, i.sysname, i.release, i.version);
}


void Cmd_open (char * tr[])
{  
    int i,df, mode=0;
    
    if (tr[0]==NULL) {
        listarFicherosAbiertos();
        return;
    }

    for (i=1; tr[i]!=NULL; i++)
      if (!strcmp(tr[i],"cr")) mode|=O_CREAT;
      else if (!strcmp(tr[i],"ex")) mode|=O_EXCL;
      else if (!strcmp(tr[i],"ro")) mode|=O_RDONLY; 
      else if (!strcmp(tr[i],"wo")) mode|=O_WRONLY;
      else if (!strcmp(tr[i],"rw")) mode|=O_RDWR;
      else if (!strcmp(tr[i],"ap")) mode|=O_APPEND;
      else if (!strcmp(tr[i],"tr")) mode|=O_TRUNC; 
      else break;
      
    if ((df=open(tr[0],mode,0777))==-1)
        perror ("Imposible abrir fichero");
    else {
        abrirArchivo(df, mode, tr[0]);
        printf ("Anadida entrada %d a la tabla ficheros abiertos\n", df);
}
}


void Cmd_close (char *tr[]) //MODIFICAR EN PRACTICA 2 PARA AÑADIR -f
{ 
    int df;
    
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
        listarFicherosAbiertos(); /*o el descriptor es menor que 0*/
        return;
    }

    
    if (close(df)==-1)
        perror("Imposible cerrar descriptor");
    else
       cerrarArchivo(df);
}

void Cmd_listopen(char *tr[])
{
    listarFicherosAbiertos();
}

void Cmd_dup(char *arg[])
{
    int df, nuevo;
    if (arg[0] == NULL) {   //No hay descriptor que duplicar
        listarFicherosAbiertos();
        return;
    }

    df = atoi(arg[0]); 
    nuevo = dup(df);
    if (nuevo == -1)
        perror("Imposible duplicar descriptor");
    else
        duplicarArchivo(df, nuevo);
}


void Cmd_lseek(char *arg[]) 
{
    int df, pos, ref;
    off_t nueva_pos;

    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {  //No hay suficientes argumentos
        printf("Uso: lseek df pos ref\n");
        return;
    }

    df = atoi(arg[0]);
    pos = atoi(arg[1]);

    if (!strcmp(arg[2], "SEEK_SET"))
        ref = SEEK_SET;
    else if (!strcmp(arg[2], "SEEK_CUR"))
        ref = SEEK_CUR;
    else if (!strcmp(arg[2], "SEEK_END"))
        ref = SEEK_END;
    else {
        printf("Referencia incorrecta. Usar SEEK_SET, SEEK_CUR o SEEK_END\n");
        return;
    }

    nueva_pos = lseek(df, pos, ref);
    if (nueva_pos == -1)
        perror("Error en lseek");
    /*else
        posicionarOffset(df, nueva_pos);*/
}

void Cmd_readstr(char *arg[]) {
    if (arg[0]==NULL || arg[1]==NULL) {
        printf("Parametros incorrectos\n");
        return;
    }
    int df, cont;

    df = atoi(arg[0]);
    cont = atoi(arg[1]);

    //por si acaso porque en el manual hacen referencia a la implementacion
    if (cont > SSIZE_MAX) {
        printf("Error: demasiado grande\n");
        return;
    }

    char *leido = malloc(cont + 1);
    if (leido == NULL) {
        perror("Error en malloc");
        return;
    }

    ssize_t leidoBytes = read(df, leido, (size_t) cont);

    if (leidoBytes == -1) {
        perror("Imposible leer");
        free(leido);
        return;
    } else {
        leido[cont]='\0'; //por si acaso
        printf("%s\n", leido);
        //posicionarOffset(df, (long)cont);
    }

    free(leido);
}


void Cmd_writestr (char *arg[]) {
    if (arg[0]==NULL || arg[1]==NULL) {
        printf("Parametros incorrectos\n");
        return;
    }

    int df = atoi(arg[0]);
    int i = 1;
    while (arg[i]!=NULL) { //si es más de una palabra
        size_t cont = strlen(arg[i]);
        char *escrito = arg[i];
    
        ssize_t escritoBytes = write(df, escrito, cont);
        if (escritoBytes==-1) {
            perror("Imposible escribir");
            return;
        }

        // espacio entre palabras (no tras la última)
        if (arg[i + 1] != NULL) {
            if (write(df, " ", 1) == -1) {
                perror("Imposible escribir");
                return;
            }
        }
        // si usase la implementacion de posicionar offset haria falta un acumulador pero descarté la idea

    i++;
    }
}

void Cmd_makefile(char *arg[]) {
    if (arg[0]==NULL) {
        MostrarDirActual();
        return;
    }

    int df;

    if ((df=open(arg[0],O_CREAT|O_EXCL|O_RDWR,0777))==-1)
        perror("Imposible crear archivo");
    else 
        close(df);
    
}

void Cmd_makedir(char *arg[]) {
    if (arg[0]==NULL) {
        MostrarDirActual();
        return;
    }

    if (mkdir(arg[0], 0777)==-1) {
        perror("Imposible crear directorio");
    }
}


int EsDirectorio (char * dir)          /*para saber si algo es directorio o no*/
{
  struct stat s;
  if (lstat(dir,&s)==-1)       /*si no puedo acceder: para mi no es directorio*/
        return 0;
  return (S_ISDIR(s.st_mode));
}


void Cmd_delete (char *arg[]) {
    if (arg[0]==NULL) {
        MostrarDirActual();
        return;
    }

    int i=0;
    while (arg[i]!=NULL) { //bucle porque acepta más de un argumento
        if (EsDirectorio(arg[0])) {
            if (rmdir(arg[0])==-1)
                perror("Imposible eliminar directorio");
        } else {
            if (unlink(arg[0])==-1) //borra solo este link, si hay más no borra el archivo
                perror("Imposible eliminar fichero");
        }
        i++;
    }
}

void auxDeltreeRec (char *arg) {
    if (EsDirectorio(arg)) {
        struct dirent *readingDir;
        DIR *point = opendir(arg);
        if (point == NULL) {
            perror("Imposible eliminar");
            return;
        }
        errno=0; //para distinguir el NULL de readdir de final del de error
        while ((readingDir = readdir(point))!=NULL) {
            
            if (strcmp(readingDir->d_name, ".") && strcmp(readingDir->d_name, "..")){ //si es distinto de 0 son distintos
                char ruta[MAXNAME];
                snprintf(ruta, sizeof(ruta), "%s/%s", arg, readingDir->d_name);
                ruta[MAXNAME-1] = '\0'; //por si acaso
                auxDeltreeRec(ruta);
            }

            errno=0;

        }

        if (errno!=0) { //si se ha salido del bucle por un error
                perror("Imposible eliminar");
            }

        if(closedir(point)==-1) {
            perror("Imposible eliminar");
            return;
        }

        if(rmdir(arg)==-1) {
            perror("Imposible eliminar");
            return;
        }


    } else {
        if (unlink(arg)==-1) {
            perror("Imposible eliminar fichero");
            return;
        }
    }
}

void Cmd_deltree (char *arg[]) {
    if (arg[0]==NULL) {
        MostrarDirActual();
        return;
    }

    int i=0;
    while (arg[i]!=NULL) { //bucle porque acepta más de un argumento
        auxDeltreeRec(arg[i]);
        i++;
    }
}


char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo (mode_t m, char *permisos)
{
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

void auxListfile (char *name, char *ruta, int listLong, int listAcc, int listLink) {
    struct stat thisFile;
    
    if(lstat(ruta, &thisFile)==-1) {
        perror("Imposible listar fichero");
        return;
    }

    time_t timeL;
    if (listAcc) { //cojo fecha de ultimo acceso o de ultima modificacion
        timeL = thisFile.st_atime;
    } else
        timeL = thisFile.st_mtime;

    struct tm *tInf = localtime(&timeL);
    
    if (listAcc||listLong) {
        printf("%d/%d/%d-%d:%d:%d ", tInf->tm_mday, tInf->tm_mon+1, tInf->tm_year+1900, tInf->tm_hour, tInf->tm_min, tInf->tm_sec);
    }

    if (listLong) {
        char permisos[12];

        //Conseguir ID de grupo y eso
        struct passwd *usuario = getpwuid(thisFile.st_uid);
        struct group *grupo = getgrgid(thisFile.st_gid);

        printf("%3ld (%20ld) %7s %7s %12s", (long)thisFile.st_nlink, (long)thisFile.st_ino,
                (usuario==NULL)?"desconocido":usuario->pw_name, (grupo==NULL)?"desconocido":grupo->gr_name,
               ConvierteModo(thisFile.st_mode, permisos));
    }
        
    printf("%8ld %s ", thisFile.st_size, name); //esto lo hace siempre al margen de los flags

    if (listLink && (thisFile.st_mode&S_IFMT)==S_IFLNK) {

        ssize_t nullterm;
        char archivoApuntado[MAXNAME];
        

        nullterm=readlink(name, archivoApuntado, MAXNAME-1);

        if (nullterm == -1 || nullterm >= MAXNAME) {
            perror("Imposible acceder al archivo linkado");
            return;
        }

        archivoApuntado[nullterm]='\0'; //readlink devuelve numero de bytes puestos en el buffer

        printf("-> %s", archivoApuntado);

    }

    printf("\n");
}

void Cmd_listfile (char *arg[]) {

    int i=0, listLong=0, listLink=0, listAcc=0, recibido=0;
    while (arg[i]!=NULL) {
        if (!(strcmp(arg[i], "-long")))
            listLong=1;
        else if (!(strcmp(arg[i], "-acc")))
            listAcc=1;
        else if (!(strcmp(arg[i], "-link")))
            listLink=1;
        else
            recibido++; //ha entrado un string distinto de estos flags
        i++;
    }

    if (recibido==0) {
        MostrarDirActual();
    } else {

        for(int j=0; j<recibido; j++) { //j=1 para que acceda al primer name
            auxListfile(arg[(i-recibido)+j], arg[(i-recibido)+j], listLong, listAcc, listLink);
            //se le pasa dos veces el nombre porque se pretende reutilizar la funcion auxiliar para el listado recursivo
        }

    }
}

void auxListRec(char *arg, int listLong, int listAcc, int listLink, int listHid, int listReca, int listRecb) {
    if (!EsDirectorio(arg)) {
        auxListfile(arg, arg, listLong, listAcc, listLink); 
        return;
    }

    
    struct dirent *readingDir;
    DIR *point = opendir(arg);
    if (point == NULL) {
        perror("Imposible listar");
        return;
    }
    errno = 0; //para distinguir el NULL de readdir de final del de error

    if (listRecb) {
        //primero tiene que bajar todos los subdirectorios para poder imprimir su cabecera
        while ((readingDir = readdir(point))!=NULL) {
            if (readingDir->d_name[0]=='.') {
                if (!listHid || (!strcmp(readingDir->d_name, ".") || !strcmp(readingDir->d_name, ".."))) { //si es . o .. no puedo acceder o hay recursividad infinita
                    continue;
                }
                //cualquier otro archivo que empiece por ., si está activado listHid, se trata como los no ocultos
            }
            char ruta[MAXNAME];
            snprintf(ruta, sizeof(ruta), "%s/%s", arg, readingDir->d_name);
            if (EsDirectorio(ruta))
                auxListRec(ruta, listLong, listAcc, listLink, listHid, listReca, listRecb);

            errno = 0; //por si se altera con la llamada recursiva
        }

        if (errno!=0) { //si se ha salido del bucle por un error
            perror("Imposible listar");
            closedir(point);
            return;
        }


        rewinddir(point); //volver al principio para la segunda pasada
    }

    printf("************%s\n", arg);

    while ((readingDir = readdir(point))!=NULL) {

        
        char ruta[MAXNAME];
        snprintf(ruta, sizeof(ruta), "%s/%s", arg, readingDir->d_name);

        if (readingDir->d_name[0]=='.') {
            if (!listHid)
                continue;
            else if (!strcmp(readingDir->d_name, ".") || !strcmp(readingDir->d_name, "..")) { //si es . o .. no puedo acceder o hay recursividad infinita
                auxListfile(readingDir->d_name, ruta, listLong, listAcc, listLink);
                continue;
            }
            //cualquier otro archivo que empiece por ., si está activado listHid, se trata como los no ocultos
        }

    
        auxListfile(readingDir->d_name, ruta, listLong, listAcc, listLink);
        if (listReca && EsDirectorio(ruta)) {
           auxListRec(ruta, listLong, listAcc, listLink, listHid, listReca, listRecb);
        }
        
        
        errno=0;

    }

    if (errno!=0) { //si se ha salido del bucle por un error
            perror("Imposible listar");
        }

    if(closedir(point)==-1) {
        perror("Imposible listar");
        return;
    }
}

void Cmd_list (char *arg[]) {

    int i=0, listLong=0, listLink=0, listAcc=0, listHid=0, listReca=0, listRecb=0, recibido=0;
    while (arg[i]!=NULL) {
        if (!(strcmp(arg[i], "-long")))
            listLong=1;
        else if (!(strcmp(arg[i], "-acc")))
            listAcc=1;
        else if (!(strcmp(arg[i], "-link")))
            listLink=1;
        else if (!(strcmp(arg[i], "-hid")))
            listHid=1;
        else if (!(strcmp(arg[i], "-reca")))
            listReca=1;
        else if (!(strcmp(arg[i], "-recb"))) {
            listRecb=1;
            listReca=0; //no van a ser las dos a la vez
        } else
            recibido++; //ha entrado un string distinto de estos flags
        i++;
    }

    if (recibido==0) {
        MostrarDirActual();
    } else {

        for(int j=0; j<recibido; j++) { //j=1 para que acceda al primer name
            auxListRec(arg[(i-recibido)+j], listLong, listAcc, listLink, listHid, listReca, listRecb);
        }

    }
}


void * ObtenerMemoriaShmget (key_t clave, size_t tam)
{
    void * p;
    int aux,id,flags=0777; /*los 9 bits menos significativos de los flags:permisos*/
    struct shmid_ds s;

    if (tam)     /*IMPORTANTE: tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s); /* si no es crear, necesitamos el tamano, que es s.shm_segsz*/
    if (!registrarMemoria(p, s.shm_segsz, T_SHARED, clave, NULL, -1)) {
        if (tam)
            shmctl(id, IPC_RMID, NULL); //borrar la clave si el tamaño es dif de 0, porque estaba creando
        errno = 0; //para aclarar que fue porque no hay espacio
        return NULL;
    }

    return (p);
}

void do_AllocateCreateShared (char *tr[])
{
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		listarMemoria();
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);
   tam=(size_t) strtoul(tr[1],NULL,10);

   if (tam==0) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
   }
   if ((p=ObtenerMemoriaShmget(cl,tam))!=NULL)
		printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
   else if (errno) //si errno!=0 porque si es 0 dio error por el tema del espacio en la lista
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_AllocateShared (char *tr[])
{
   key_t cl;
   void *p;

   if (tr[0]==NULL) {
		listarMemoria();
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);

   if ((p=ObtenerMemoriaShmget(cl,0))!=NULL)
		printf ("Asignada memoria compartida de clave %lu en %p\n",(unsigned long) cl, p);
   else if (errno) //errno distinto de 0, si errno es 0 es porque no hay hueco en la lista
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_AllocateMalloc(char *tr[]) {
    size_t tam;
    void *mem;
    
    if (tr[0] == NULL) {
            listarMemoria();
        } else {
            tam = atol(tr[0]);
            if (tam==0) {
                printf("No se asignan bloques de 0 bytes\n");
                return;
            }

            mem = malloc(tam);
            if (mem == NULL) {
                perror("Imposible reservar memoria");
                return;
            } else {
                if (registrarMemoria(mem, tam, T_MALLOC, 0, NULL, -1))
                    printf("Asignados %zu bytes en %p\n", tam, mem);
            }            
        }
}

void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED) {
        close(df);
        return NULL;
    }

    if(!registrarMemoria(p, s.st_size, T_MMAP, 0, fichero, df)) {
        errno=0;
        return NULL;
    }
    abrirArchivo(df, modo, fichero); //lista de ficheros abiertos
    return p;
}

void do_AllocateMmap(char *arg[])
{ 
    char *perm;
    void *p;
    int protection=0;
     
    if (arg[0]==NULL) {
        listarMemoria();
        return;
    }

    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
     }
     if ((p=MapearFichero(arg[0],protection))!=NULL)
        printf ("fichero %s mapeado en %p\n", arg[0], p);
     else if (errno) //si errno es 0 fallo por no haber espacio en la lista
        perror ("Imposible mapear fichero");
            
}


void Cmd_allocate(char *arg[]) {
    
    if (arg[0] == NULL) {
        listarMemoria();
        return;
    }

    if (!(strcmp(arg[0], "-malloc"))) {
        do_AllocateMalloc(arg+1);

    } else if (!(strcmp(arg[0], "-createshared"))){
        do_AllocateCreateShared(arg+1);
    
    } else if (!(strcmp(arg[0], "-shared"))){
        do_AllocateShared(arg+1)

    } else if (!(strcmp(arg[0], "-mmap"))){
        do_AllocateMmap(arg+1);

    } else {
        printf("uso: allocate [-malloc|-shared|-createshared|-mmap]\n");
        return;
    }
}

void do_DeallocateMalloc(char *arg[]) {
    size_t tam;
     if (arg[0] == NULL) {
            listarMemoria();
            return;
        } else {
            tam = atol(arg[0]); //si es negativo no va a haber bloque asignado de ese tamaño
            int id = buscarBloqueMalloc(tam);
            if (id == -1) {
                printf("No hay bloque de ese tamano asignado con malloc\n");
                return;
            }
            liberarBloqueMemoria(id);            
        }
}

void do_DeallocateShared (char *arg[]) {
    key_t clave;

    if (arg[0] == NULL)  {
        listarMemoria();
        return;
    } else {
        clave = atol(arg[0]);
        int id = buscarBloqueShared(clave);
        if (id == -1) {
            printf("No hay bloque de esa clave mapeado en el proceso\n");
            return;
        }
        liberarBloqueMemoria(id);
    }

}

void do_DeallocateMmap (char *arg[]) {
    char *nombre;

    if (arg[0] == NULL) {
        listarMemoria();
        return;
    }
    
    nombre = arg[1];
    int id = buscarBloqueMmap(nombre);

    if (id == -1) {
        printf("Fichero %s no mapeado\n", nombre);
        return;
    }

    liberarBloqueMemoria(id);
}

void do_SharedDelkey (char *args[])
{
   key_t clave;
   int id;
   char *key=args[0];

   if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
   }
   if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
   }
   if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida");
}

void * CadenatoPointer (char * s)
{
  void *p;
  sscanf(s,"%p",&p);
  return p;
}

void do_DeallocateAddr(char *arg[]) {
    
    void *dir = CadenaToPointer(arg[0]);
    if (dir==NULL) { //sscanf fallido
        printf("Direccion invalida\n");
        return;
    }

    int id = buscarBloqueDireccion(dir);

    if (id == -1) {
        printf("Direccion %p no asignada con malloc, shared o mmap\n", dir);
        return;
    }

    liberarBloqueMemoria(id);
}

void Cmd_deallocate(char *arg[]) {

    if (arg[0] == NULL) {
        listarMemoria();
        return;

    } else if (!(strcmp(arg[0], "-malloc"))) {
        do_DeallocateMalloc(arg+1);

    } else if (!(strcmp(arg[0], "-shared"))) {
        do_DeallocateShared(arg+1);

    } else if (!(strcmp(arg[0], "-mmap"))) {
        do_DeallocateMmap(arg+1);

    } else if (!(strcmp(arg[0], "-delkey"))) {
        do_SharedDelkey(arg+1);

    } else {
        do_DeallocateAddr(arg);
    }
}

void Cmd_memfill(char *arg[]) {
    void *dir;
    size_t cont, i;
    unsigned char valor; //hexadecimal
    unsigned char *p; //unsigned representa byte para trabajar entre 0 y 255

    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: memfill addr cont hcode\n");
        return;
    }

    if (sscanf(arg[0], "%p", &dir) != 1) {
        printf("Direccion no valida\n");
        return;
    }

    cont = atoi(arg[1]);
    valor = (unsigned char) atoi(arg[2]); 
    p = (unsigned char *) dir; //acceder a cada byte individual

    for (i = 0; i < cont; i++) {
        p[i] = valor;
    }

    printf("Rellenados %zu bytes desde la direccion %p con el valor (%02X) en decimal y %c en caracter\n", cont, dir, valor, valor);
}

void Cmd_memdump(char *arg[]) {
    void *dir;
    size_t cont, i;
    unsigned char *p;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Uso: memdump addr cont\n");
        return;
    }

    if (sscanf(arg[0], "%p", &dir) != 1) {
        printf("Direccion no valida\n");
        return;
    }

    cont = atoi(arg[1]);
    p = (unsigned char *) dir;

    printf("Volcando %zu bytes desde la direccion %p\n", cont, dir);

    printf("%p ->", dir);
    for (i = 0; i < cont; i++) {
        printf(" %c ", p[i]);  //simbolo que corresponde al numero
    }
    printf("\n");

    printf("%p ->", dir);
    for (i = 0; i < cont; i++) {
        printf(" %02X", p[i]);   //valor hexadecimal que corresponde al numero
    }
    printf("\n");
}