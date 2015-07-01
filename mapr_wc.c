/* 
 * Map - Reduce WordCount using plain C
 * Warning: this is just prototype (Proof of Concept)
 */

#define _GNU_SOURCE
#include <assert.h>

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#include<pthread.h>

#include <sys/types.h>
#include <unistd.h>

#include <glib.h>

#ifdef MAPR_WC_MTRACE
#include <mcheck.h>
#endif

pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  q_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t  q_full = PTHREAD_COND_INITIALIZER;

#define MAPR_QUEUE_LIMIT 10

/* we don't use GAsyncQueue, because it has no size limit */
GQueue  queue = G_QUEUE_INIT;

static void enqueue(char *str){
       int err;

       err = pthread_mutex_lock(&q_lock);
       assert_perror(err);

      /* wait until queue size is < MAPR_QUEUE_LIMIT */
      while ( g_queue_get_length (&queue) >= MAPR_QUEUE_LIMIT ){
          err = pthread_cond_wait(&q_full,&q_lock);
          assert_perror(err);
      }
 
       g_queue_push_head ( &queue, str );

       err = pthread_mutex_unlock(&q_lock);
       assert_perror(err);

       /* signal, that queue is no longer empty */
       err = pthread_cond_signal(&q_empty);
       assert_perror(err);
}

static char* dequeue(){
      char *str;
      int err;
      err = pthread_mutex_lock(&q_lock);
      assert_perror(err);

      /* wait as long as queue is empty */
      while ( g_queue_is_empty(&queue) ){
          err = pthread_cond_wait(&q_empty,&q_lock);
          assert_perror(err);
      }
      str =  g_queue_pop_tail(&queue);

      err = pthread_mutex_unlock(&q_lock);
      assert_perror(err);

      /* signal that queue is no longer full */
      err = pthread_cond_signal(&q_full);
      assert_perror(err);
      return str;
}

static int usage(char *prog, char *msg){
    if(msg != NULL ){
       fprintf(stderr,"ERROR: %s\n",msg);
    }
    fprintf(stderr,"Usage: %s <file_to_wordcount> <number_of_threads>\n",prog);
    return 1;
}

/*
 * count words on line string
 * This version is used for both single-threaded and multi-threaded counting
 */
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
        char *str;

	pid = getpid();
	tid = pthread_self();

	printf("pid %lu, tid %lu\n",
	       (unsigned long)pid, (unsigned long)tid);

      
        while ( (str = dequeue()) != NULL ){
//                printf("map_thread while str at 0x%p\n",str);
                *((int*)arg) +=  count_words(str);
                free(str);
        }
        printf("thread ending, counted %d\n", *((int*)arg));
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
          if ( pthread_create( threads+i,NULL,map_thread,sums+i) ){
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
              // FIXME: avoid costly copy
              enqueue( strdup(buf) );
 //             printf("Buffer '%s' sent\n",buf);
           } else {
              total += count_words(buf);
           }
    }

    fclose(f);f=NULL;
 
    /* send special ACK packet (NULL str) to signal end of queue */
    for(i=0;i<n_threads;i++){
//           printf("Enqueing NULL\n");
           enqueue(NULL);
    }
    /* join all threads */
    for(i=0;i<n_threads;i++){
          if ( pthread_join( threads[i],NULL) ){
               perror("pthread_join");
               return 1;
          }
    }
    g_assert (g_queue_is_empty(&queue));

    /* compute total from sums (only in thread version) */
    for(i=0;i<n_threads;i++){
        total += sums[i];
    }
    printf("There are total %d words\n",total);
    if ( n_threads ){
	    free(threads); threads = NULL;
            free(sums); sums = NULL;
            if ( queue.head ){
                 g_list_free( queue.head );
                 queue.head = NULL; queue.tail = NULL;
            }
    }

#ifdef MAPR_WC_MTRACE
    muntrace();
#endif
    return 0;
}
