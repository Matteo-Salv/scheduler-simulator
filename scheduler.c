#include "Header.h"

//ricordo la strategia non preemptive: FCFS
void* scheduler_no_pree(void *params_no_pree){
    int core;
    int clock = 0;
    int flag;
    arg_no_pree* args = (arg_no_pree*) params_no_pree;
    FILE *fp;

    //dove salvo il tcb in run
    tcb* run = NULL;

    //preparo la lista dei ready
    tcb* ready = (tcb*) malloc (sizeof(tcb));
    ready->next = ready;
    ready->prev = ready;

    //preparo la lista dei blocked
    tcb* blocked = (tcb*) malloc (sizeof(tcb));
    blocked->next = blocked;
    blocked->prev = blocked;

    //apertura in scrittura del file di output
    fp = fopen(args->out_no_pree, "w");

    //funzione per identificare il core
    if(!args->n_core){
        core = 0;
        args->n_core = true;
    }else{
        core = 1;
    }

    //se la lista dei tcb è vuota termino il thread
    pthread_mutex_lock(&args->mux_tcbs);
    if(args->tcbs == args->tcbs->next){
        printf("#########ESCO DAll'INIZIO DI NON PREEMPTIVE");
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
    pthread_mutex_unlock(&args->mux_tcbs);

    //inizio col primo processo
    do{
        run = ready->next;
        run->state = 2;                 //run
        print(fp, clock, core, run->id_task, run->state, &(args->mux_file));
        printf("tcb %d %d\n", run->id_task, run->arr_time);

        //lo rimuovo dalla lista dei ready;
        ready = remove_top_tcb(ready);

        //avvio quindi il processo
        do{
            //se l'istruzione è bloccante
            if(run->pc->next->type_flag == 1){
                printf("istruzione bloccante\n");
                //calcolo il tempo di blocco, aggiorno lo stato e scrivo su file
                run->pc->next->lock_time = (rand() % run->pc->next->length)+1;
                run->state = 3;             //blocked
                print(fp,clock,core,run->id_task, run->state, &(args->mux_file));

                blocked = add_ready(blocked, run);

                flag = 1;

            //altrimenti se l'istruzione non è bloccante
            }else{
                printf("istruzione non bloccante\n");
                flag = 0;
                clock += run->pc->next->length;

                //se ci sono ancora istruzioni in ready
                pthread_mutex_lock(&args->mux_tcbs);
                if(args->tcbs != args->tcbs->next){
                    printf("sono presenti istruzioni in ready\n");
                    arrival_time(fp, ready, args->tcbs, clock, core, &(args->mux_file));
                }
                pthread_mutex_unlock(&args->mux_tcbs);

                //se sono presenti istruzioni bloccate
                if(blocked->next != blocked){
                   printf("sono presenti istruzioni bloccate\n");
                    isBlocked(fp, blocked, ready, run->pc->next->length, clock, core, &(args->mux_file));
                }

                run->pc = remove_top_inst(run->pc);

                //se ho finito tutte le istruzioni in run
                if(run->pc->next == run->pc){
                    printf("ho finito tutte le istruzioni in run");
                    run->state = 4;
                    print(fp, clock, core, run->id_task, run->state, &(args->mux_file));
                }
            }
        }while(flag == 0 && run->pc->next != run->pc);      //finchè sono presenti istruzioni non bloccanti

        //finchè la lista ready è vuota incremento il clock
        while(ready == ready->next){

            //se ho terminato i jobs e la lista blocked è vuota
            pthread_mutex_lock(&args->mux_tcbs);
            bool isTrue = false;
            if(args->tcbs == args->tcbs->next){
                isTrue = true;
            }
            pthread_mutex_unlock(&args->mux_tcbs);

            if(blocked == blocked->next && isTrue){
//                sleep(5);
                printf("#######TERMINO NON DAL FONDO DEL NON PREEMPTIVE##########");
                pthread_exit(NULL);
            }

            //incremento il clock
            clock++;

            //controllo se ci sono nuovi tcb da processare
            pthread_mutex_lock(&args->mux_tcbs);
            if(args->tcbs != args->tcbs->next){
                arrival_time(fp, ready, args->tcbs, clock, core, &(args->mux_file));
            }
            pthread_mutex_unlock(&args->mux_tcbs);

            //controllo i blocked
            if(blocked != blocked->next){
                isBlocked(fp,blocked,ready, 1,clock,core, &(args->mux_file));
            }

        }


    }while(ready != ready->next);       //finchè sono presenti tcb da mettere in run;

    printf("###########HO TERMINATO LO SCHEDULER NON PREEMPTIVE############# core%d\n", core);
    return NULL;
}

void* scheduler_pree(void* params_pree){
    return NULL;
}
