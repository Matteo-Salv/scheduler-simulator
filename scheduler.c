#include "Header.h"

//ricordo la strategia non preemptive: FCFS
void* scheduler_no_pree(void *params_no_pree){
    int core;
    int clock = 0;
    int flag;
    arg_no_pree* args = params_no_pree;

    //dove salvo il tcb in run
    tcb* run = NULL;

    //preparo la lista dei ready
    tcb* ready = (tcb*) malloc (sizeof(tcb));
    ready->pc = ready;
    ready->prev = ready;

    //preparo la lista dei blocked
    tcb* blocked = (tcb*) malloc (sizeof(tcb));
    blocked->pc = blocked;
    blocked->prev = blocked;

    //apertura in scrittura del file di output
    fp = fopen(args->out_no_pree, "w");

    //funzione per identificare il core
    if(!args->n_core){
        core = 0;
    }else{
        core = 1;
    }

    //se la lista dei tcb è vuota termino il thread
    pthread_mutex_lock(&args->mux_tcbs);
    if(args->tcbs == args->tcbs->next){
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&args->mux_tcbs);

    //inizio quindi con il primo tcb
    pthread_mutex_lock(&args->mux_tcbs);
    do{
        clock++;
        if(args->tcbs->next->arr_time == clock){
            tcb* new_tmp = args->tcbs->next;
            args->tcbs = remove_top_tcb(args->tcbs);

            new_tmp->state = 1;             //ready
            ready = add_ready(ready, new_tmp);

            print(fp, clock, core, new_tmp->id_task, new_tmp->state, &(args->mux_file));
        }
    }while(ready->next == ready);
    pthread_mutex_unlock(&args->mux_file);

    /*                                                                      IN SOSTANZA DA IN FLAG = 0 IN POI /*

}

void* scheduler_pree(void* params_pree){

}
