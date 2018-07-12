#include <stdio.h>
#include <getopt.h>     //unica libreria per getopt
#include <stdlib.h>      //per la gestione delle 'exit'
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "Header.h"

//solo di debug

void printdebug(tcb* tcbs){
    tcb* tmp_tcb;
    inst* tmp_inst;
    tmp_tcb = tcbs->next;
    printf("prev: %d %d\n", tcbs->prev->id_task, tcbs->prev->arr_time);
    while(tmp_tcb != tcbs){
        printf("%d %d\n", tmp_tcb->id_task, tmp_tcb->arr_time);
        tmp_inst = tmp_tcb->pc->next;
        while(tmp_inst != tmp_tcb->pc){
            printf("%d %d\n", tmp_inst->type_flag, tmp_inst->length);
            tmp_inst = tmp_inst->next;
        }
        tmp_tcb = tmp_tcb->next;
    }
}

//metodo per l'inserimento dei tcb in coda
tcb* insertBackTcb(tcb* tcbs, tcb* tmp_tcb){
    tmp_tcb->next = tcbs;                           //il successivo del nuovo tcb sarà la testa
    tmp_tcb->prev = tcbs->prev;                     //il precedente del nuovo tcb sarà il penultimo tcb inserito
    //successivamente aggiorno i puntatori degli elementi già in lista
    tcbs->prev->next = tmp_tcb;
    tcbs->prev = tmp_tcb;
    return tcbs;
}

//metodo per l'inserimento di un'istruzione in coda, logica identica a insertBacktcb
inst* insertBackInst(inst* instructions, inst* tmp_inst){
    tmp_inst->next = instructions;
    tmp_inst->prev = instructions->prev;
    instructions->prev->next = tmp_inst;
    instructions->prev = tmp_inst;
    return instructions;
}

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
            tmp_tcb->tcb_length = 0;

            //controllo che il carattere successivo sia una instruction
            fscanf(stream, " %c", &val);
            if(val!='i'){
                printf("errore di formattazione nel file di input:\n");
                printf("controllare la prima istruzione del task %d\n", tmp_tcb->id_task);
                printf("##################CHIUSURA SIMULATORE##################");
                exit(1);
            }

            //alloco la memoria per la prima istruzione del tcb in questione e controllo che vada a buon fine,
            // se è affermativo faccio puntare la prima istruzione a se stessa
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
            //################################devo ricordarmi di aggiornare anche tcb_length########################
            do{
                inst *tmp_inst = (inst*) malloc (sizeof(inst));
                if(tmp_inst == NULL){
                    printf("si e' verificato un errore durante la lettura del file di input:\n");
                    printf("impossibile allocare la memoria per la lettura delle istruzioni\n");
                    printf("##################CHIUSURA SIMULATORE##################");
                    exit(1);
                }else{

                    //leggo l'istruzione
                    fscanf(stream,",%d,%d ",&tmp_inst->type_flag,&tmp_inst->length);

                    //aggiorno la lunghezza totale del tcb
                    tmp_tcb->tcb_length+= tmp_inst->length;

                    //aggiungo l'istruzione in coda alla lista delle istruzioni
                    tmp_tcb->pc = insertBackInst(tmp_tcb->pc, tmp_inst);

                    //controllo sul file se dopo c'è un task o una instruction
                    val = fgetc(stream);

                    //se non è t o i stampo un errore ed esco
                    if (val != 't' && val != 'i' && val != EOF){
                        printf("errore nella lettura del file! Possibile formattazione errata?\n");
                        printf("valore atteso: 't' oppure 'i', valore letto: %c\n", &val);
//                        printf("ultima istruzione letta: %d %d\n", tmp_inst->type_flag, tmp_inst->length);
//                        printf("ultimo task letto: %d %d\n", tmp_tcb->id_task, tmp_tcb->arr_time);
                        printf("##################CHIUSURA SIMULATORE##################");
                        exit(1);
                    }
                }
            }while(val=='i');

            //aggiungo il tcb in coda alla lista dei tcb
            tasks = insertBackTcb(tasks, tmp_tcb);

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
    const char* file_in;                //per salvare il nome del file di input
    int check = 0;                      //per il controllo del numero di paramentri inseriti
    int next_option = 0;                //variabile utilizzata nello switch del getopt
    int quantum = 10;                   //variabile per il quanto di tempo del round robin, non lo chiedo come parametro ma di default è 10
    bool help = false;                       //0= false, ovvero l'utente non ha usato -h; 1=true
    arguments arg_sched;                //salvo tutti gli argomenti letti in input
    tcb *tcbs = NULL;                   //lista di tutti i tcb

    //per i thread
    pid_t pree;
    pid_t no_pree;
    int status_pree, status_nopree;

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
                help = true;
                print_help();
                printf("chiusura del programma...\n");
                return 0;
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
    if(check!=5 && !help){
        printf("\nparametri inseriti non sufficienti, seguire le istruzioni:\n");
        print_help();
        printf("chiusura del programma...\n");
        return(1);
    }

    //funzione del master, faccio il parsing di tutti i task e istruzioni
    tcbs = master(tcbs, file_in);

    //fork del non preemptive
    no_pree = fork();

    if(no_pree < 0){
        perror("Forking dello scheduler non preemptive fallito!");
        exit(1);
    }else if(no_pree == 0){
        arg_no_pree no_pree;

        no_pree.out_no_pree = arg_sched.out_no_pree;
        no_pree.tcbs = tcbs;
        no_pree.n_core = false;
        pthread_mutex_init(&no_pree.mux_tcbs, NULL);
        pthread_mutex_init(&no_pree.mux_file, NULL);

        pthread_t core0;
        pthread_t core1;
        if(pthread_create(&core0, NULL, scheduler_no_pree, &no_pree) != 0){
            perror("error during the creation of pthread 0!");
        }

        /*
        if(pthread_create(&core1, NULL, scheduler_no_pree, &no_pree) != 0 ){
            perror("error during the creation of pthread 1!");
        }*/

        pthread_join(core0, NULL);
//        pthread_join(core1, NULL);
    }else{
        wait(&status_nopree);
        ;
        //fork del preemptive
        pree = fork();
        if(pree < 0){
            perror("Forking dello scheduler preemptive fallito!");
            return(1);
        }else if(pree == 0){
            pthread_t core2;
            pthread_t core3;
            arg_pree pree;

            pree.out_pree = arg_sched.out_pree;
            pree.tcbs = tcbs;
            pree.n_core = false;
            pree.quantum = quantum;             //il quanto di tempo non viene chiesto, ma è sempre prefissato a priori;
            pthread_mutex_init(&pree.mux_tcbs, NULL);
            pthread_mutex_init(&pree.mux_file, NULL);

            if(pthread_create(&core2, NULL, scheduler_pree, &pree) != 0){
                perror("error during the creation of pthread 2!");
            }

            if(pthread_create(&core3, NULL, scheduler_pree, &pree) != 0){
                perror("error during the creation of pthread 3!");
            }

            pthread_join(core2, NULL);
            pthread_join(core3, NULL);
        }else{
            wait(&status_pree);
        }
    }

    
    //solo di debug
//    printdebug(tcbs);

    printf("##################SIMULATORE TERMINATO CON SUCCESSO#####################\n");
    return 0;
}


