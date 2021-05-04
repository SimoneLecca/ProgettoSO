#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

//definizione movimento
#define PASSO 1
#define DESTRA 67
#define SINISTRA 68
#define SPACE 32
#define SU 65
#define GIU 66

//grandezza area gioco e limiti
#define MINX 2
#define MINY 2
#define MAXX 150
#define MAXY 38

// delay
#define COUNTDOWN_DELAY 1 // delay durante il countdown nella schermata iniziale
#define DELAY 50000 // delay dei processi/thread

// indicano le tipologie di oggetti a schermo
#define GAMER 1 // identifica la tipologia della navicella mossa dal giocatore
#define MISSILE 2
#define BORDO 3
#define ASTRONAVE1 4 // identifica l'astronave di lv1
#define ASTRONAVE2 5 // identifica l'astronave di lv2
#define BOMBA 6

//NEMICI
#define N_ASTONAVI_NEMICHE 5	//n di astronavi nemiche totali
#define DELAY_ASTRONAVI 2000000 //delay tra un'astronave nemica e l'altra

// dimensione buffer
#define DIM_BUFFER 50

#define OBJ_ON_SCREEN 50 // NUMERO OGGETTI MASSIMI A SCERMO

pthread_mutex_t mutex_scrn; // mutex per accesso alla risorsa condivisa screen
pthread_mutex_t mutex_buffer; // mutex per l'accesso al buffer
pthread_mutex_t mutex_collision; // mutex per l'accesso alla matrice per la gestione delle collisioni
pthread_mutex_t mutex_fire; // mutex per l'accesso alla variabile utilizzata per quando si sprara
pthread_mutex_t mutex_boom; // mutex per l'accesso alla variabile utilizzata per il lancio di bombe
pthread_mutex_t mutex_id; // mutex per l'accesso alla variabile utilizzata per la creazione degli id

sem_t buffer_count; // semaforo per l'accesso al buffer

// struttura che identifica un oggeto
struct oggetto{
	int id;
	int tipo;
	char* sprite[2];
	int dim;
	int dimy;
	int x;
	int y;
	int vite;
	
};  

struct Node {
    pthread_t info;
    struct Node *next;
};

void aggiungi_job (struct oggetto);
void stampaOggetto(struct oggetto);
void cancellaOggetto(struct oggetto, int);
int isColliding(struct oggetto, struct oggetto[]);

int counter; // contatore per il buffer
struct oggetto buffer[DIM_BUFFER]; // BUFFER
int collision_m [OBJ_ON_SCREEN][2];

int fire[2];// casin0000
int boom[3];// 0/1, x, y

int newId;//varibile per generare gli id degli oggetti in gioco
bool end_player;	//astronave del giocatore distrutta
bool end_enemis;	//le astronavi nemiche sono tutte distrutte
bool end_layer;		//le astronavi nemiche sono arrivate all'ultimo layer e vincono

