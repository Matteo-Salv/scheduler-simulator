#include <stdio.h>
#include <getopt.h>     //unica libreria per getopt
#include <stdlib.h>      //per la gestione delle 'exit'
#include "Header.h"

tcb* master(tcb *tasks, const char* input){
    char val;          //variabile dove salvo temporaneamente il carattere letto da file
    FILE *stream;
    stream= fopen(input, "r");

    if(stream == NULL){
        printf("si e' verificato un errore all'apertura del file di input\n");
        printf("si controlli la correttezza del nome inserito\n");
        printf("##################CHIUSURA SIMULATORE#####################");
        exit(1);
    }

    while(!feof(stream)){   //fino alla fine del file
        val = fgetc(stream);    //leggo il primo carattere

        //controllo quindi che il primo carattere sia una stringa
        if(val !='t'){
            printf("valore letto: %c\n", val);
            printf("errore di formattazione all'inizio del file di input\n");
            printf("######################CHIUSURA SIMULATORE########################");
            exit(1);
        }

        //alloco la testa della lista dei TCB
        tasks = (tcb*) malloc (sizeof(tcb));

        //controllo che sia andato tutto a buon fine, se è affermativo faccio puntare la sentinella a se stessa
        if(tasks == NULL){
            printf("impossibile allocare la memoria per i task\n");
            printf("##################CHIUSURA SIMULATORE##################");
            exit(1);
        }else{
            tasks->next = tasks;
            tasks->prev = tasks;
        }

        //inizio quindi con la lettura
        do{
            tcb *tmp_tcb = (tcb*) malloc (sizeof(tcb));     //creo un task temporaneo dove salvare ciò che ricevo in input

            //controllo che tmp_tcb venga allocato correttamente
            if(tmp_tcb == NULL){
                printf("si e' verificato un errore durante la lettura del file di input:\n");
                printf("impossibile allocare la memoria per la lettura\n");
                printf("##################CHIUSURA SIMULATORE##################");
                exit(1);
            }

            //leggo quindi il primo task
            fscanf(stream, ",%d,%d", &tmp_tcb->id_task, &tmp_tcb->arr_time);

            //controllo che il carattere successivo sia una instruction
            fscanf(stream, " %c", &val);
            if(val!='i'){
                printf("errore di formattazione nel file di input:\n");
                printf("controllare la prima istruzione del task %d\n", tmp_tcb->id_task);
                printf("##################CHIUSURA SIMULATORE##################");
                exit(1);
            }

            //alloco la memoria per la prima istruzione del tcb in questione e controllo che vada a buon fine, se è affermativo faccio puntare la prima istruzione a se stessa
            tmp_tcb->pc = (inst*) malloc (sizeof(inst));
            if(tmp_tcb->pc == NULL){
                printf("si e' verificato un errore durante la lettura del file di input:\n");
                printf("impossibile allocare la memoria per la lettura delle istruzioni\n");
                printf("##################CHIUSURA SIMULATORE##################");
                exit(1);
            }else{
                tmp_tcb->pc->prev = tmp_tcb->pc;
                tmp_tcb->pc->next = tmp_tcb->pc;
            }

            //inizio quindi la lettura delle instruction
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++DA QUA, DEVO CREARMI I METODI DI GESTIONE DELLA LISTA+++++++++++++++++++++++++++++++++++++++++

        }while(val == 't');

    }

    return tasks;
}

//print help
void print_help(){
    printf("########################UTILIZZO#########################\n");
    printf("NB i campi -op -on -i sono richiesti:\n");
    printf("-op [nome file dove scrivere l'output della preemptive]\n");
    printf("-on [nome file dove scrivere l'output della non-preemptive]\n");
    printf("-i [nome file di input dei job]\n");
    printf("-h o --help visualizza questo messaggio di aiuto\n");
    printf("Si noti che i file di output, se non sono già presenti, verranno creati in automatico\n");
    printf("###################CHIUSURA SIMULATORE####################\n");
}

//main
int main(int argc, const char * argv[]) {
    const char* program_name = argv[0]; //salvo il nome del programma
    const char* file_in;                //per salvare il nome del file di input
    int check = 0;                      //per il controllo del numero di paramentri inseriti
    int next_option = 0;                //variabile utilizzata nello switch del getopt
    int quantum = 10;                   //variabile per il quanto di tempo del round robin, non lo chiedo come parametro ma di default è 10
    int help = 0;                       //0= false, ovvero l'utente non ha usato -h; 1=true
    arguments arg_sched;                //salvo tutti gli argomenti letti in input
    tcb *tcbs = NULL;                   //lista di tutti i tcb

    //short e long options:
    const char* const short_options = "op: on: i: h";
    const struct option long_options[] = {
        {NULL,                      0,  NULL,   'o'},
        {"output-preemption",       1,  NULL,   'p'},
        {"output-no-preemption",    1,  NULL,   'n'},
        {"input",                   1,  NULL,   'i'},
        {"help",                    0,  NULL,   'h'},
        {NULL,                      0,  NULL,    0 }
    };

    //se gli argomenti non sono corretti, stampo la guida ed esco
    if (argc == 1) {
        print_help();
        return(0);
    }

    //switch per recuperare i vari argomenti, check controlla che ci siano tutti
    do{
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch(next_option){
            case 'o':
                check++;
                continue;
            case 'p':
                check++;
                arg_sched.out_pree = optarg;
                break;
            case 'n':
                check++;
                arg_sched.out_no_pree = optarg;
                break;
            case 'i':
                check++;
                file_in = optarg;
                break;
            case 'h':
                help = 1;
                print_help();
                printf("chiusura del programma...\n");
                return 0;
                break;
            case '?':
                print_help();
                return 0;
            case -1:
                break;
            default:
                printf("ERRORE NELLA LETTURA DEGLI ARGOMENTI\n");
                return 0;
        }
    }while(next_option!=-1);

    //controllo se sono stati inseriti tutti i parametri, check = 5 solo se ci sono tutti (3 argomenti + due volte -o)
    if(check!=5 && help==0){
        printf("\nparametri inseriti non sufficienti, seguire le istruzioni:\n");
        print_help();
        printf("chiusura del programma...\n");
        return(1);
    }

    //funzione del master, faccio il parsing di tutti i task e istruzioni
    tcbs = master(tcbs, file_in);

    printf("##################SIMULATORE TERMINATO CON SUCCESSO#####################\n");
}


