#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

//----------
//TODO el archivo /proc/kmsg da errores
//----------
void checkEquals(char * file,char * buf1, char * buf2);
void openFile(char * outputFile);
void forking(char * argv,ino_t d_ino);
void recursiveFinding(const char * path);
const char * LITERAL_DOTDIR=".";
const char * LITERAL_2DOTDIR="..";

char absolutePath[256];
int pipefd[2];
char buffer[41];
char buffer2[41];
ino_t inode;
char * args[2];
int fd=-1;
void * ptr;


int main(int argc, char * argv[]){

    if(argc!=3){
        write(2,"Usage: chasher <Father-node> <hashedfiles\n>",43);
        exit(-1);
    }
    ptr=opendir(argv[1]);
    if(ptr==NULL){
        write(2,"Error opening the specified directory\n>",38);
        exit(-1);
    }
    ptr=readdir((DIR*)ptr);
    if(ptr==NULL){
        write(2,"Error reading the specified directory\n>",38);
        exit(-1);
    }

    if(access(argv[2],F_OK)!=0){
        write(2,"The specified hash-content file doesn't exist\n>",47);
        exit(-1);
    }

    openFile(argv[2]);

    if(chdir(argv[1])==-1){
        write(2,"Error entering to the specified directory\n>",38);
        exit(-1);
    }

    absolutePath[255]='\0';
    buffer[40]='\0';
    buffer2[40]='\0';
    args[0]="sha1sum";
    args[1]=NULL;
    args[2]=NULL;
    
    inode=-1;

    recursiveFinding(LITERAL_DOTDIR);

    close(fd);
    write(2,"[+] Exited successfully\n",24);
    return 0;
}

void openFile(char * outputFile){
    fd=open(outputFile,O_RDONLY);
    //printf("%s da %d\n",outputFile,fd);
    if(fd == -1){
        write(2,"[-] Error reading the output file\n",35);
        exit(-1);
    }
}

void forking(char * argv,ino_t d_ino){
    fprintf(stderr,"%s/%s\n",absolutePath,argv);
    pipe(pipefd);

        switch(fork()){
            case 0: //Programa hijo
                close(pipefd[0]);
                close(1);
                dup(pipefd[1]);

                args[1]=strdup(argv);
                execvp(args[0],args);
                exit(0);

            case -1: 
                write(2,"[-] Error during runtime\n",24);
                exit(-1);
            default:
                close(pipefd[1]);
                wait(NULL);
                read(pipefd[0],buffer,40);

                //Escribe en el archivo
                unsigned long pos=lseek64(fd,40*d_ino,SEEK_SET);
                int bytes=read(fd,buffer2,40);

                checkEquals(argv,buffer,buffer2);
                //Deja de escribir
                
                close(pipefd[0]);
                break;
    }
}

void recursiveFinding(const char * path){
    DIR * ptr = opendir(path);
    struct dirent * directory;

    if(ptr!=NULL){
    directory=readdir(ptr);
    }else return;

    getcwd(absolutePath,256);
    for(;directory!=NULL;directory=readdir(ptr)){

        if(strcmp(LITERAL_DOTDIR,directory->d_name)==0 ||strcmp(LITERAL_2DOTDIR,directory->d_name)==0){
            continue;
        }
        
        if(directory->d_type==DT_DIR){
            //printf("d\t%s\n",directory->d_name);
            if(chdir(directory->d_name)==-1){
                fprintf(stderr,"No permission to enter %s\n",directory->d_name);
                continue;
            }
            recursiveFinding(LITERAL_DOTDIR);
            
        } else if(directory->d_type==DT_REG){
            forking(directory->d_name,directory->d_ino);
        }
    }
    closedir(ptr);
    chdir(LITERAL_2DOTDIR);
    getcwd(absolutePath,256);
}

void checkEquals(char * file,char * buf1, char * buf2){
    if(strcmp(buf1,buf2)!=0){    
        printf("%s/%s <----%s != %s\n",absolutePath,file,buf1,buf2);
    }
}
