/* 
 * Map - Reduce WordCount using plain C
 */

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>

#include<pthread.h>

#include <sys/types.h>
#include <unistd.h>


int usage(char *prog, char *msg){
    if(msg != NULL ){
       fprintf(stderr,"ERROR: %s\n",msg);
    }
    fprintf(stderr,"Usage: %s <number_of_threads>\n",prog);
    return 1;
}

void *map_thread(void *arg){
	pid_t  pid;
	pthread_t  tid;

	pid = getpid();
	tid = pthread_self();

	printf("pid %lu, tid %lu\n",
	       (unsigned long)pid, (unsigned long)tid);
        return NULL;
}

int main(int argc, char **argv){

    int i;
    int n_threads;
    pthread_t *threads;

    if ( argc!=2){
        return usage(argv[0],NULL);
    }
    n_threads = atoi(argv[1]);
    if ( n_threads <= 0 ){
        fprintf(stderr,"ERROR: Invalid number of threads int(\"%s\") -> %d\n",
                       argv[1],n_threads);
        return 1;
    }
    threads = (pthread_t*)malloc( sizeof(pthread_t) * n_threads );
    if ( threads == NULL ){
         perror("malloc");
         return 1;
    }

    for(i=0;i<n_threads;i++){
          if ( pthread_create( threads+i,NULL,map_thread,NULL) ){
               perror("pthread_create");
               return 1;
          }
    }

    /* join all threads */
    for(i=0;i<n_threads;i++){
          if ( pthread_join( threads[i],NULL) ){
               perror("pthread_join");
               return 1;
          }
    }
    printf("All done.\n");

    return 0;
}
