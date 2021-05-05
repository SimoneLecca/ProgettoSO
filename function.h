void start(); //inizializza le variabili

void print_home(); // stampa la schermata iniziale
void print_box(); // stampa il bordo

void inizializza_coda(); // inizalizza il semaforo 

void end_game(); // stampa il bordo

void controllore(); // legge dal buffer, stampa a video, controlla le collisioni

void aggiornaPunteggio(int punt);

void* t_giocatore(void* arg); // thread che si occupa dell aggiornamendo delle cordinate della navicella del giocatore
void* t_missile(void* arg); // thread che aggiorna le posizioni di un missile lanciato dal giocatore
void* t_lanciatore_missili(void* arg);// lancia i missisili

void* t_generatore_astronavi(void* arg);
void* t_astronave1 (void* arg);


struct oggetto elimina();
///void aggiungi_job (struct* oggetto);

//funzione per generare gli id degli oggetti in gioco
int generatorId();
