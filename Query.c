#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

const char *dbname = "crociere";
const char *script_filename = "Crociere.sql";
const char *conninfo_root = "user=postgres host=localhost port=5432";

void exit_nicely(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

// Legge l'intero contenuto di un file in una stringa (malloc, da free dopo)
char* read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Errore apertura file");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        perror("malloc");
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    return buffer;
}

// Esegue una query SQL semplice senza risultati attesi
void exec_sql(PGconn *conn, const char *sql) {
    PGresult *res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore esecuzione SQL: %s\n%s\n", PQerrorMessage(conn), sql);
        PQclear(res);
        exit_nicely(conn);
    }
    PQclear(res);
}

// Esegue e mostra i risultati di una query SELECT
void exec_select(PGconn *conn, const char *sql) {
    PGresult *res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore esecuzione query: %s\n%s\n", PQerrorMessage(conn), sql);
        PQclear(res);
        exit_nicely(conn);
    }
    int nrows = PQntuples(res);
    int nfields = PQnfields(res);

    // Stampiamo intestazioni colonna
    for (int i = 0; i < nfields; i++) {
        printf("%-25s", PQfname(res, i));
    }
    printf("\n");
    for (int i = 0; i < nfields; i++) {
        printf("-------------------------");
    }
    printf("\n");

    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < nfields; c++) {
            printf("%-25s", PQgetvalue(res, r, c));
        }
        printf("\n");
    }
    PQclear(res);
}

#define MAX_QUERIES 10

typedef struct {
    char title[128];
    char sql[2048];
} Query;

int main() {
    PGconn *conn;
    PGresult *res;

    // Connessione a postgres senza database specifico per drop/create db
    conn = PQconnectdb(conninfo_root);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita: %s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    // Proviamo a creare il db "crociere" (ignora errore se già esiste)
    exec_sql(conn, "DROP DATABASE IF EXISTS crociere;");
    exec_sql(conn, "CREATE DATABASE crociere;");

    PQfinish(conn);

    // Ora connettiamo al db crociere
    char conninfo_db[256];
    snprintf(conninfo_db, sizeof(conninfo_db), "user=postgres host=localhost port=5432 dbname=%s", dbname);
    conn = PQconnectdb(conninfo_db);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita al db crociere: %s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    // Leggiamo tutto il file SQL
    char *sqlscript = read_file(script_filename);
    if (!sqlscript) {
        PQfinish(conn);
        return 1;
    }

    // Dividiamo il file in linee per analizzare commenti con titolo query e separare le query
    // Metodo semplice: assumiamo che ogni query da eseguire nel menu sia preceduta da un commento "-- Query X – titolo"
    // Altrimenti, eseguiamo tutto all'inizio (create table, insert) e separiamo solo le query per il menu.

    // Per semplicità: eseguiamo tutto tranne le query dal file all'inizio (dividendo il file al commento "-- Query 1" etc)
    // poi salviamo le query in un array da menu.

    // Troviamo l'inizio delle query:
    char *queries_start = strstr(sqlscript, "-- Query 1");
    if (!queries_start) {
        fprintf(stderr, "Non trovate query nel file\n");
        free(sqlscript);
        PQfinish(conn);
        return 1;
    }

    // Eseguiamo la parte iniziale (da inizio file fino a queries_start)
    size_t init_len = queries_start - sqlscript;
    char *init_part = malloc(init_len + 1);
    if (!init_part) {
        perror("malloc");
        free(sqlscript);
        PQfinish(conn);
        return 1;
    }
    strncpy(init_part, sqlscript, init_len);
    init_part[init_len] = '\0';

    // Eseguiamo tutto il codice iniziale
    res = PQexec(conn, init_part);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore esecuzione iniziale script: %s\n%s\n", PQerrorMessage(conn), init_part);
        PQclear(res);
        free(init_part);
        free(sqlscript);
        PQfinish(conn);
        return 1;
    }
    PQclear(res);
    free(init_part);

    // Ora estraiamo le query per il menu. Dato che le query sono commentate come:
    // -- Query X – Titolo
    // SELECT ...
    // Quindi cerchiamo ogni commento -- Query e prendiamo la query successiva fino al prossimo commento --

    Query queries[MAX_QUERIES];
    int qcount = 0;

    char *p = queries_start;
    while (qcount < MAX_QUERIES && p) {
        Query queries[MAX_QUERIES];
int qcount = 0;

char *p = queries_start;
while (qcount < MAX_QUERIES && p) {
    // Cerchiamo commento
    char *comment_line_start = strstr(p, "-- Query");
    if (!comment_line_start) break;

    // Saltiamo "-- "
    comment_line_start += 3;

    // Leggiamo la riga commento fino a newline
    char *line_end = strchr(comment_line_start, '\n');
    if (!line_end) break;
    size_t len = line_end - comment_line_start;

    // Copiamo titolo, rimuovendo eventuali caratteri speciali
    char titlebuf[128];
    strncpy(titlebuf, comment_line_start, len);
    titlebuf[len] = '\0';

    // Trova trattino (lungo "–" o corto "-")
    char *dash = strstr(titlebuf, "–");
    if (!dash) dash = strchr(titlebuf, '-');

    if (dash) {
        dash++; // salta il trattino
        // salta eventuali spazi subito dopo il trattino
        while (*dash == ' ') dash++;
        // sposta il testo dopo il trattino all'inizio di titlebuf
        memmove(titlebuf, dash, strlen(dash) + 1);
    }

    // Ora leggiamo la query vera: dalla fine della linea di commento fino al prossimo commento "--"
    char *query_start = line_end + 1;
    char *next_comment = strstr(query_start, "\n--");
    size_t query_len;
    if (next_comment)
        query_len = next_comment - query_start;
    else
        query_len = strlen(query_start);

    // Copiamo la query rimuovendo spazi finali e nuove linee
    while(query_len > 0 && (query_start[query_len-1] == '\n' || query_start[query_len-1] == ' ' || query_start[query_len-1] == '\r'))
        query_len--;

    if (query_len > sizeof(queries[qcount].sql) - 1)
        query_len = sizeof(queries[qcount].sql) - 1;

    strncpy(queries[qcount].sql, query_start, query_len);
    queries[qcount].sql[query_len] = '\0';

    strncpy(queries[qcount].title, titlebuf, sizeof(queries[qcount].title));
    queries[qcount].title[sizeof(queries[qcount].title)-1] = '\0';

    qcount++;
    p = next_comment;
}

    }

    free(sqlscript);

    printf("=== MENU QUERY CROCIERE ===\n");
    for (;;) {
        printf("\nScegli una query da eseguire:\n");
        for (int i = 0; i < qcount; i++) {
            printf(" %d) %s\n", i + 1, queries[i].title);
        }
        printf(" 0) Esci\n");
        printf("Scelta: ");

        int scelta = -1;
        if (scanf("%d", &scelta) != 1) {
            printf("Input non valido\n");
            // Puliamo input buffer
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (scelta == 0) {
            break;
        }
        if (scelta < 1 || scelta > qcount) {
            printf("Scelta non valida\n");
            continue;
        }

        printf("\nEsecuzione query: %s\n\n", queries[scelta - 1].title);
        exec_select(conn, queries[scelta - 1].sql);
    }

    PQfinish(conn);
    printf("Programma terminato.\n");
    return 0;
}
