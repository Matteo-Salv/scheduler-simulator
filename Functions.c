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



