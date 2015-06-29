/* 
 * Map - Reduce WordCount using plain C
 */

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#include<pthread.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef MAPR_WC_MTRACE
#include <mcheck.h>
#endif

static int usage(char *prog, char *msg){
    if(msg != NULL ){
       fprintf(stderr,"ERROR: %s\n",msg);
    }
    fprintf(stderr,"Usage: %s <file_to_wordcount> <number_of_threads>\n",prog);
    return 1;
}

static int count_words(char *line){
       char *state;
       int wc = 0; /* word count */
       while ( strtok_r(line,"\t ",&state) ){
           line = NULL; /* signal for strtok to use state */
           wc ++ ;
       }

       return wc;
}

static void *map_thread(void *arg){
	pid_t  pid;
	pthread_t  tid;

	pid = getpid();
	tid = pthread_self();

	printf("pid %lu, tid %lu\n",
	       (unsigned long)pid, (unsigned long)tid);
        return NULL;
}

static void trim_nl(char *str){
   char *p = NULL;

   if (strlen(str)<1 ){
        return;
   }
   p = strrchr(str,'\n');
   if ( p!=NULL ){
        *p = '\0';
   }

   p = strrchr(str,'\r');
   if ( p!=NULL ){
        *p = '\0';
   }
}

int main(int argc, char **argv){

    char buf[1024];
    FILE *f;
    int i;
    int n_threads;
    pthread_t *threads;
    int *sums,total;

#ifdef MAPR_WC_MTRACE
    mtrace();
#endif

    if ( argc!=3){
        return usage(argv[0],NULL);
    }
    n_threads = atoi(argv[2]);
    if ( n_threads <= 0 ){
        /* handle zero specialy */
        if ( strcmp(argv[2],"0") ){
		fprintf(stderr,"ERROR: Invalid number of threads int(\"%s\") -> %d\n",
			       argv[2],n_threads);
		return 1;
        }
    }
 
    if (n_threads > 0){
            printf("Using %d map threads\n",n_threads);
	    threads = (pthread_t*)malloc( sizeof(pthread_t) * n_threads );
	    if ( threads == NULL ){
		 perror("malloc threads");
		 return 1;
	    }
            sums = (int*)malloc(sizeof(int)*n_threads);
            if ( sums == NULL ){
		 perror("malloc sums");
                 return 1;
            }
            
    } else {
           printf("Using single threaded model\n");
    }

    printf("Opening '%s' for reading\n",argv[1]);
    f=fopen(argv[1],"r");
    if ( f == NULL ){
         /* TODO: show, which file failed to open */
         perror("fopen");
         return 1;
    }


    for(i=0;i<n_threads;i++){
          if ( pthread_create( threads+i,NULL,map_thread,NULL) ){
               perror("pthread_create");
               return 1;
          }
    }

    total = 0;
    while( fgets(buf,sizeof(buf)-1,f) !=NULL ){
           trim_nl(buf);        
           if ( strlen(buf) == 0 ){
                continue;
           }
           if ( n_threads ){
              fprintf(stderr,"Multi-threading mode not yet implemented\n");
              return 1;
           } else {
              total += count_words(buf);
           }
    }

    fclose(f);f=NULL;
 

    /* join all threads */
    for(i=0;i<n_threads;i++){
          if ( pthread_join( threads[i],NULL) ){
               perror("pthread_join");
               return 1;
          }
    }
    printf("There are total %d words\n",total);
    if ( n_threads ){
	    free(threads); threads = NULL;
            free(sums); sums = NULL;
    }

#ifdef MAPR_WC_MTRACE
    muntrace();
#endif
    return 0;
}
