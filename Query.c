#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define MAX_PARAMS 10
#define MAX_PARAM_LEN 32
#define MAX_QUERIES 20
#define MAX_SQL_LEN 2048

typedef struct {
    char title[128];
    char sql[2048];
    char params[MAX_PARAMS][MAX_PARAM_LEN];
    int param_count;
} Query;


const char *dbname = "crociere";
const char *script_filename = "Crociere.sql";

char user[64] = "";
char password[64] = "";


void exit_nicely(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

int read_config(char* user, size_t ulen, char* password, size_t plen) {
    FILE* file = fopen("config.txt", "r");
    if (!file) {
        perror("Impossibile aprire config.txt");
        return 0;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "user=", 5) == 0) {
            strncpy(user, line + 5, ulen);
            user[strcspn(user, "\r\n")] = '\0'; // Rimuovi newline
        } else if (strncmp(line, "password=", 9) == 0) {
            strncpy(password, line + 9, plen);
            password[strcspn(password, "\r\n")] = '\0';
        }
    }

    fclose(file);
    return strlen(user) > 0 && strlen(password) > 0;
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


int main() {
    PGconn *conn;
    PGresult *res;

    if (!read_config(user, sizeof(user), password, sizeof(password))) {
        fprintf(stderr, "Errore nella lettura del file config.txt\n");
    return 1;
    }

    char conninfo_root[256];
    snprintf(conninfo_root, sizeof(conninfo_root),
            "user=%s password=%s host=localhost port=5432", user, password);

    // Connessione a postgres senza db specifico (root) per drop/create db
    conn = PQconnectdb(conninfo_root);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita: %s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    // Drop e create database "crociere" (ignora errore se già esiste)
    exec_sql(conn, "DROP DATABASE IF EXISTS crociere;");
    exec_sql(conn, "CREATE DATABASE crociere;");

    PQfinish(conn);

    // Connessione al database "crociere"
    char conninfo_db[256];
    snprintf(conninfo_db, sizeof(conninfo_db),
         "user=%s password=%s host=localhost port=5432 dbname=%s",
         user, password, dbname);
    conn = PQconnectdb(conninfo_db);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita al db crociere: %s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    // Leggi l'intero file SQL in memoria
    char *sqlscript = read_file(script_filename);
    if (!sqlscript) {
        PQfinish(conn);
        return 1;
    }

    // Trova la posizione in cui iniziano le query da menu (commento "-- Query 1")
    char *queries_start = strstr(sqlscript, "-- Query 1");
    if (!queries_start) {
        fprintf(stderr, "Non trovate query nel file\n");
        free(sqlscript);
        PQfinish(conn);
        return 1;
    }

    // Esegui la parte iniziale dello script (creazione tabelle, insert, ecc.)
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

    // Estrazione delle query da menu
    Query queries[MAX_QUERIES];
    int qcount = 0;

    char *p = queries_start;
    while (qcount < MAX_QUERIES && p) {
        char *comment_line_start = strstr(p, "-- Query");
        if (!comment_line_start) break;

        // Salta al testo dopo " - " nel commento titolo, se presente
        char *pos = strstr(comment_line_start, " - ");
        if (pos) {
            comment_line_start = pos + 3;
        } else {
            comment_line_start += 3; // dopo "-- "
        }

        // Legge titolo fino a newline
        char *line_end = strchr(comment_line_start, '\n');
        if (!line_end) break;
        size_t len = line_end - comment_line_start;

        char titlebuf[128];
        strncpy(titlebuf, comment_line_start, len);
        titlebuf[len] = '\0';

        // Se il titolo contiene il trattino lungo (–), usa solo la parte dopo
        char *dash = strstr(titlebuf, "–");
        if (dash) {
            dash += 2; // salta spazio dopo dash lungo
            memmove(titlebuf, dash, strlen(dash) + 1);
        }

        // Estrae la query dalla riga dopo il commento fino al prossimo commento "--"
        char *query_start = line_end + 1;
        char *next_comment = strstr(query_start, "\n--");
        size_t query_len = next_comment ? (size_t)(next_comment - query_start) : strlen(query_start);

        // Rimuove spazi e newline finali
        while (query_len > 0 &&
              (query_start[query_len-1] == '\n' || query_start[query_len-1] == ' ' || query_start[query_len-1] == '\r'))
            query_len--;

        if (query_len > sizeof(queries[qcount].sql) - 1)
            query_len = sizeof(queries[qcount].sql) - 1;

        strncpy(queries[qcount].sql, query_start, query_len);
        queries[qcount].sql[query_len] = '\0';

        strncpy(queries[qcount].title, titlebuf, sizeof(queries[qcount].title));
        queries[qcount].title[sizeof(queries[qcount].title)-1] = '\0';

        // Cerca parametri <PARAM> nella query solo se preceduti da apostrofo '
        queries[qcount].param_count = 0;
        char *search_start = queries[qcount].sql;
        while (queries[qcount].param_count < MAX_PARAMS) {
            char *open = strchr(search_start, '<');
            if (!open) break;

            // Controlla che sia preceduto da apostrofo '
            if (open == queries[qcount].sql || *(open - 1) != '\'') {
            search_start = open + 1;
            continue;
            }

            char *close = strchr(open, '>');
            if (!close) break;

            size_t plen = close - open - 1;
            if (plen > 0 && plen < MAX_PARAM_LEN) {
            strncpy(queries[qcount].params[queries[qcount].param_count], open + 1, plen);
            queries[qcount].params[queries[qcount].param_count][plen] = '\0';
            queries[qcount].param_count++;
            }
            search_start = close + 1;
        }

        qcount++;
        p = next_comment;
    }

    free(sqlscript);

    // Loop menu per scelta ed esecuzione query
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
        
        char final_sql[MAX_SQL_LEN];
        strcpy(final_sql, queries[scelta - 1].sql);

        // Se ci sono parametri, chiediamoli all'utente
        if (queries[scelta - 1].param_count > 0) {
            for (int i = 0; i < queries[scelta - 1].param_count; i++) {
                char valore[MAX_PARAM_LEN];
                printf("Inserisci valore per '%s': ", queries[scelta - 1].params[i]);
                scanf("%s", valore);  // per semplicità usiamo scanf %s

                // Sostituisce <PARAM> con il valore
                char *pos = strstr(final_sql, queries[scelta - 1].params[i]);
                if (pos) {
                    char segnaposto[MAX_PARAM_LEN + 2];
                    snprintf(segnaposto, sizeof(segnaposto), "<%s>", queries[scelta - 1].params[i]);

                    char nuova_query[MAX_SQL_LEN];
                    char *inizio = final_sql;
                    char *found;

                    // Cerca e sostituisce tutte le occorrenze
                    nuova_query[0] = '\0';
                    while ((found = strstr(inizio, segnaposto)) != NULL) {
                        strncat(nuova_query, inizio, found - inizio);
                        strcat(nuova_query, valore);
                        inizio = found + strlen(segnaposto);
                    }
                    strcat(nuova_query, inizio);
                    strcpy(final_sql, nuova_query);
                }
            }
        }

        exec_select(conn, final_sql);

    }

    PQfinish(conn);
    printf("Programma terminato.\n");
    return 0;
}
