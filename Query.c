#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define MAX_QUERY_LEN 2048
#define MAX_QUERY_NUM 50

typedef struct {
    char descrizione[256];
    char query[MAX_QUERY_LEN];
} Query;

void check_conn(PGconn *conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

void esegui_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        // Per comandi CREATE/INSERT
        PQclear(res);
        return;
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nella query: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int righe = PQntuples(res);
    int colonne = PQnfields(res);

    for (int i = 0; i < colonne; i++)
        printf("%s\t", PQfname(res, i));
    printf("\n");

    for (int i = 0; i < righe; i++) {
        for (int j = 0; j < colonne; j++)
            printf("%s\t", PQgetvalue(res, i, j));
        printf("\n");
    }

    PQclear(res);
}

int carica_query_da_file(const char *filename, Query queries[], int *query_count, char **contenuto_completo) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);

    *contenuto_completo = malloc(len + 1);
    fread(*contenuto_completo, 1, len, file);
    (*contenuto_completo)[len] = '\0';
    fclose(file);

    // Estrazione delle query per il menu
    char *line = strtok(*contenuto_completo, "\n");
    char buffer_query[MAX_QUERY_LEN] = "";
    int in_query = 0;

    while (line) {
        if (strncmp(line, "-- QUERY:", 9) == 0) {
            if (in_query && *query_count < MAX_QUERY_NUM) {
                strcpy(queries[*query_count].query, buffer_query);
                (*query_count)++;
                buffer_query[0] = '\0';
            }

            in_query = 1;
            strcpy(queries[*query_count].descrizione, line + 9);
            buffer_query[0] = '\0';
        } else if (in_query) {
            strcat(buffer_query, line);
            strcat(buffer_query, "\n");
        }

        line = strtok(NULL, "\n");
    }

    // Aggiungi l'ultima query
    if (in_query && *query_count < MAX_QUERY_NUM) {
        strcpy(queries[*query_count].query, buffer_query);
        (*query_count)++;
    }

    return 1;
}

int main() {
    const char *conninfo = "host=localhost dbname=progetto user=postgres password=tuapassword";
    PGconn *conn = PQconnectdb(conninfo);
    check_conn(conn);

    Query queries[MAX_QUERY_NUM];
    int query_count = 0;
    char *contenuto_sql = NULL;

    if (!carica_query_da_file("script.sql", queries, &query_count, &contenuto_sql)) {
        fprintf(stderr, "Impossibile caricare il file SQL.\n");
        return 1;
    }

    // Esegui tutto il contenuto del file per creare/popolare le tabelle
    PGresult *res = PQexec(conn, contenuto_sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nell'inizializzazione: %s\n", PQerrorMessage(conn));
    } else {
        printf("Tabelle create e popolate.\n");
    }
    PQclear(res);
    free(contenuto_sql);

    int scelta;
    while (1) {
        printf("\n=== Menu Query ===\n");
        for (int i = 0; i < query_count; i++) {
            printf("%d) %s\n", i + 1, queries[i].descrizione);
        }
        printf("0) Esci\n");
        printf("Scegli una query da eseguire (0 per uscire): ");
        scanf("%d", &scelta);
        getchar(); // pulisce newline

        if (scelta == 0) {
            break;
        } else if (scelta >= 1 && scelta <= query_count) {
            // SOSTITUZIONE DELLA QUERY SE CI SONO PARAMETRI
            char query_finale[MAX_QUERY_LEN];
            strcpy(query_finale, queries[scelta - 1].query);

            char *inizio_input = strstr(query_finale, "<");
            char *fine_input = strstr(query_finale, ">");
            if (inizio_input && fine_input && fine_input > inizio_input) {
                char nome_campo[64];
                strncpy(nome_campo, inizio_input + 1, fine_input - inizio_input - 1);
                nome_campo[fine_input - inizio_input - 1] = '\0';

                char valore_input[128];
                printf("Inserisci valore per %s: ", nome_campo);
                scanf(" %[^\n]", valore_input);

                char query_sostituita[MAX_QUERY_LEN];
                *inizio_input = '\0'; // tronca temporaneamente

                char query_sostituita[MAX_QUERY_LEN];
                query_sostituita[0] = '\0';

                strncat(query_sostituita, query_finale, MAX_QUERY_LEN - strlen(query_sostituita) - 1);
                strncat(query_sostituita, "'", MAX_QUERY_LEN - strlen(query_sostituita) - 1);
                strncat(query_sostituita, valore_input, MAX_QUERY_LEN - strlen(query_sostituita) - 1);
                strncat(query_sostituita, "'", MAX_QUERY_LEN - strlen(query_sostituita) - 1);
                strncat(query_sostituita, fine_input + 1, MAX_QUERY_LEN - strlen(query_sostituita) - 1);


                esegui_query(conn, query_sostituita);
            } else {
                esegui_query(conn, query_finale);
            }
        } else {
            printf("Scelta non valida.\n");
        }
    }

    PQfinish(conn);
    return 0;
}

