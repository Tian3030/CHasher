#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

//----------
//TODO el archivo /proc/kmsg da errores
//----------
void quitHandler(int signo);
void createFile(char * outputFile);
void forking(char * argv,ino_t d_ino);
void recursiveFinding(const char * path);

const char * LITERAL_DOTDIR=".";
const char * LITERAL_2DOTDIR="..";
char absolutePath[256];
int pipefd[2];
char buffer[41];
ino_t inode;
char * args[5];
int fd=-1;
void * ptr;
int status=-1;
int pid=-1;


void quitHandler(int signo){
    if(signo==3 && pid!=-1){
    kill(pid,SIGINT);
    } else if(signo==2){
        ftruncate(fd, 0);
        write(1,"[-] Proccess cancelled\n",24);
        exit(1);
    }
}

int main(int argc, char * argv[]){

    if(argc!=3){
        write(2,"Usage: chasher <Father-node> <hashesfile\n>",42);
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

    if(access(argv[2],F_OK)==0){
        write(2,"The specified output file exists\n>",34);
        exit(-1);
    }
    createFile(argv[2]);

    if(chdir(argv[1])==-1){
        write(2,"Error entering to the specified directory\n>",38);
        exit(-1);
    }
    write(1,"You can use SIGQUIT (Ctrl+\\) to interrupt the wait to read some file.\n",71);
    sleep(2);

    absolutePath[255]='\0';
    buffer[40]='\0';
    args[0]="sha1sum";
    args[1]="-b";
    args[2]="-z";
    args[3]=NULL;
    args[4]=NULL;
    
    inode=-1;
    signal(SIGQUIT,&quitHandler);
    signal(SIGINT,&quitHandler);


    recursiveFinding(LITERAL_DOTDIR);
    close(fd);
    memset(buffer,0,41); //Borra el ultimo hash almacenado en memoria
    write(2,"[+] Exited successfully\n",24);
    return 0;
}

void createFile(char * outputFile){
    fd=open(outputFile,O_RDWR | O_TRUNC | O_CREAT,0640);
    if(fd == -1){
        write(2,"[-] Error creating the output file\n",36);
        exit(-1);
    }
}

void forking(char * argv,ino_t d_ino){
    pipe(pipefd);

        switch((pid=fork())){

            case 0: //Programa hijo
                close(pipefd[0]);
                close(1);
                dup(pipefd[1]);
                args[3]=argv;
                execvp(args[0],args);
            case -1: 
                write(2,"[-] Error during runtime\n",24);
                exit(-1);
            default:
                close(pipefd[1]);
                wait(&status);
                pid=-1;
                
                if(WIFEXITED(status) && WEXITSTATUS(status) == 0){
                    
                    read(pipefd[0],buffer,40);
                    //Escribe en el archivo
                    unsigned long pos=lseek64(fd,40*d_ino,SEEK_SET);
                    int bytes= write(fd,buffer,40);
                    printf("%s/%s\t\t\t\tBYTES: %d\t\t\t\tPOS: %lu\n",absolutePath,argv,bytes,pos);
                    //Deja de escribirs
                }
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
            if(chdir(directory->d_name)==-1){
                fprintf(stderr,"No permission to enter to %s\n",directory->d_name);
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


