#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define BUF_SIZE 64 // Dimensione del buffer
#define NUM_BUFS 16 // Numero di buffer nel ring

// Struttura per rappresentare un buffer nel ring
typedef struct
{
    unsigned char data[BUF_SIZE]; // Buffer di dati
    int bytes;                    // Numero di byte effettivamente presenti nel buffer
} buf_t;

// Array di buffer nel ring
buf_t buffers[NUM_BUFS];
// Indici di lettura e scrittura, e contatore di blocchi nel ring
int read_idx = 0, write_idx = 0, count = 0;
// Flag che indica se la lettura è terminata
bool done = false;
// Mutex per sincronizzazione critica e per il contatore di blocchi
pthread_mutex_t crit_sec, count_lock;
// Variabili di condizione per segnalare lo stato del ring
pthread_cond_t not_full, not_empty;
FILE *src_file, *dst_file;

// Funzione eseguita dal thread di lettura
void *read_func(void *arg)
{
    int n;
    while (!feof(src_file))
    {
        // Blocca la sezione critica
        pthread_mutex_lock(&crit_sec);

        // Attende se il ring è pieno
        if (count >= NUM_BUFS)
            pthread_cond_wait(&not_full, &crit_sec);

        // Legge dal file di origine nel buffer corrente
        n = fread(buffers[write_idx].data, 1, BUF_SIZE, src_file);
        if (n > 0) // Se sono stati letti dei byte
        {
            // Imposta il numero di byte nel buffer corrente
            buffers[write_idx].bytes = n;
            // Passa al prossimo buffer nel ring
            write_idx = (write_idx + 1) % NUM_BUFS;
            // Blocca il mutex per modificare count
            pthread_mutex_lock(&count_lock);
            // Incrementa il contatore di blocchi nel ring
            count++;
            // Sblocca il mutex
            pthread_mutex_unlock(&count_lock);
            // Segnala che il ring non è più vuoto
            pthread_cond_signal(&not_empty);
        }
        // Sblocca la sezione critica
        pthread_mutex_unlock(&crit_sec);
    }
    // Imposta il flag di fine lettura
    done = true;
    // Segnala che il ring non è più vuoto
    pthread_cond_signal(&not_empty);
    pthread_exit(NULL);
}

// Funzione eseguita dal thread di scrittura
void *write_func(void *arg)
{
    while (1)
    {
        // Se la lettura è terminata e non ci sono più blocchi nel ring, esce dal ciclo
        if (done && count == 0)
            break;

        // Blocca la sezione critica
        pthread_mutex_lock(&crit_sec);

        // Se ci sono blocchi nel ring, scrive nel file di destinazione
        if (count > 0)
        {
            // Scrive dal buffer corrente nel file di destinazione
            fwrite(buffers[read_idx].data, 1, buffers[read_idx].bytes, dst_file);
            // Passa al prossimo buffer nel ring
            read_idx = (read_idx + 1) % NUM_BUFS;
            // Blocca il mutex per modificare count
            pthread_mutex_lock(&count_lock);
            // Decrementa il contatore di blocchi nel ring
            count--;
            // Sblocca il mutex
            pthread_mutex_unlock(&count_lock);
            // Segnala che il ring non è più pieno
            pthread_cond_signal(&not_full);
        }
        else
        {
            // Attende se il ring è vuoto
            pthread_cond_wait(&not_empty, &crit_sec);
        }
        // Sblocca la sezione critica
        pthread_mutex_unlock(&crit_sec);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t writer, reader;

    if (argc != 3)
    {
        printf("Uso: %s file-origine file-destinazione\r\n", argv[0]);
        return 0;
    }
    src_file = fopen(argv[1], "rb");
    if (src_file == NULL)
    {
        printf("Errore apertura file %s\r\n", argv[1]);
        return 0;
    }
    dst_file = fopen(argv[2], "wb");
    if (dst_file == NULL)
    {
        printf("Errore apertura file %s\r\n", argv[2]);
        fclose(src_file);
        return -1;
    }

    // Inizializza i mutex e le variabili di condizione
    pthread_mutex_init(&crit_sec, NULL);
    pthread_mutex_init(&count_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    // Avvia i thread di lettura e scrittura
    pthread_create(&reader, NULL, &read_func, NULL);
    pthread_create(&writer, NULL, &write_func, NULL);

    // Attende che i thread terminino
    pthread_join(reader, NULL);
    pthread_join(writer, NULL);

    // Distrugge i mutex e le variabili di condizione
    pthread_mutex_destroy(&crit_sec);
    pthread_mutex_destroy(&count_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    fclose(src_file);
    fclose(dst_file);

    return 0;    
}
