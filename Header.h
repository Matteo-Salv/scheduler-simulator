#ifndef Header_h
#define Header_h
#include <stdio.h>

typedef struct inst {
    int type_flag;
    int length;
    struct inst* next;      //istruzione successiva
    struct inst* prev;      //istruzione precedente
}inst;

typedef struct tcb {
    int id_task;
    int arr_time;
    inst *pc;                      //program counter, puntatore alla prossima istruzione
    struct tcb* next;              //puntatore al tcb successivo
    struct tcb* prev;              //puntatore al tcb precedente
    int time_blocked;              //se l'istruzione è bloccante, qua specifico per quanto tempo la tcb deve essere bloccata
}tcb;

typedef struct arguments{
    char* out_pree;
    char* out_no_pree;
}arguments;

#endif
