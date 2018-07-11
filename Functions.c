#include "Header.h"

//rimozione tcb in testa alla coda
tcb* remove_top_tcb(tcb* top){
    top->next->next->prev = top;
    top->next = top->next->next;

    return top;
}

//aggiunta nella lista ready;
tcb* add_ready(tcb* top, tcb* new_tcb){

    //se la lista non è vuota
    if(top->next != top){
        top = insertBackTcb(top, new_tcb);
    }else{
        top->next = new_tcb;
        top->prev = new_tcb;
        new_tcb->next = top;
        new_tcb->prev = top;
    }
}

//per la stampa sul file
void print(FILE *fp, int clock, int core, int id, int stato, pthread_mutex_t* mux){
    pthread_mutex_lock(mux);
    switch(stato){
        case 1:
            fprintf(fp, "core%d,%d,%d,ready\n", core, clock, id);
            break;
        case 2:
            fprintf(fp, "core%d,%d,%d,running\n", core, clock, id);
            break;
        case 3:
            fprintf(fp, "core%d,%d,%d,blocked\n", core, clock, id);
            break;
        case 4:
            fprintf(fp, "core%d,%d,%d,exit\n", core, clock, id);
            break;
        default:
            fprintf(fp, "core%d,%d,%d,UNKNOWN ERROR\n", core, clock, id);
            printf("unknown error!");
            break;
    }
    fprintf(fp, "\n");
    fflush(fp);

    pthread_mutex_unlock(mux);
}

//per controllare l'arrival time delle istruzioni
void arrival_time(FILE* fp, tcb* ready, tcb* tcbs, int clock, int core, pthread_mutex_t* mux){
    tcb* new_tcb;

    //scorro tutti le istruzioni con arrival time < clock
    while(tcbs->next->arr_time <= clock && tcbs->next->arr_time > 0){
        new_tcb = tcbs->next;
        new_tcb->state = 1;             //ready;
        tcbs = remove_top_tcb(tcbs);

        //aggiungo quindi il nuovo tcb nella lista dei ready e scrivo su file
        ready = add_ready(ready, new_tcb);
        print(fp, clock, core, new_tcb->id_task, new_tcb->state, mux);
    }
    new_tcb = NULL;
}

//controllo se è bloccato
void isBlocked(FILE* fp, tcb* blocked, tcb* ready, int length, int clock, int core, pthread_mutex_t* mux){
    tcb* blocked_tcb = blocked->next;

    //per prima cosa decremento tutti i bloccati
    do{
        blocked_tcb->pc->next->lock_time -= length;
        blocked_tcb = blocked_tcb->next;
    }while(blocked != blocked_tcb);

    //successivamente finchè il primo si sblocca
    while(blocked->next->pc->next->lock_time < 1 && blocked->next != blocked){
        blocked->next->pc->next->type_flag = 0;             //sblocco l'istruzione
        tcb* tmp_tcb = blocked->next;
        tmp_tcb->state = 1;                                  //ready
        blocked = remove_top_tcb(blocked);

        //aggiungo quindi il nuovo tcb nella lista dei ready e scrivo su file
        ready = add_ready(ready, tmp_tcb);
        print(fp, clock, core, tmp_tcb->id_task, tmp_tcb->state, mux);
    }
    blocked_tcb = NULL;
}

//rimozione dell'istruzione in testa
inst* remove_top_inst(inst* top){
    top->next = top->next->next;
    top->next->next->prev = top;

    return top;
}



