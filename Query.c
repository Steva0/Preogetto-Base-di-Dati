#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define MAX_QUERY_LEN 8192
#define MAX_QUERY_NUM 1000


typedef struct {
    char descrizione[256];
    char query[MAX_QUERY_LEN];
} Query;

void checkConn(PGconn *conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione fallita: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

int checkDatabaseExistence(PGconn *conn, const char *dbname) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT 1 FROM pg_database WHERE datname='%s';", dbname);
    PGresult *res = PQexec(conn, query);
    int exists = PQntuples(res) > 0;
    PQclear(res);
    return exists;
}

void createDatabaseIfNotExists(PGconn *conn, const char *dbname) {
    char query[256];
    snprintf(query, sizeof(query), "CREATE DATABASE %s;", dbname);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore nella creazione del database: %s\n", PQerrorMessage(conn));
    } else {
        printf("Database '%s' creato correttamente.\n", dbname);
    }
    PQclear(res);
}

int esegui_script_sql_senza_commenti(PGconn *conn, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Errore nell'apertura del file SQL");
        return 0;
    }

    char *contenuto = malloc(MAX_QUERY_LEN * MAX_QUERY_NUM);
    if (!contenuto) {
        perror("Errore di allocazione");
        fclose(file);
        return 0;
    }
    contenuto[0] = '\0';

    char riga[1024];
    while (fgets(riga, sizeof(riga), file)) {
        char *commento = strstr(riga, "--");
        if (commento) *commento = '\0';  // tronca al commento
        // Rimuove whitespace finali
        size_t len = strlen(riga);
        while (len > 0 && (riga[len - 1] == '\n' || riga[len - 1] == '\r' || riga[len - 1] == ' ' || riga[len - 1] == '\t')) {
            riga[--len] = '\0';
        }
        // Aggiunge la riga al contenuto
        if (strlen(riga) > 0) {
            strcat(contenuto, riga);
            strcat(contenuto, "\n");
        }
    }

    fclose(file);

    // Ora divide il contenuto su ;
    char *query = strtok(contenuto, ";");
    while (query != NULL) {
        // Rimuove whitespace iniziali
        while (*query == ' ' || *query == '\n' || *query == '\r' || *query == '\t') query++;

        if (strlen(query) > 0) {
            char query_finale[MAX_QUERY_LEN];
            snprintf(query_finale, sizeof(query_finale), "%s;", query);

            PGresult *res = PQexec(conn, query_finale);
            if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
                fprintf(stderr, "Errore nell'esecuzione di una query:\n%s\nErrore: %s\n", query_finale, PQresultErrorMessage(res));
                PQclear(res);
                free(contenuto);
                return 0;
            }
            PQclear(res);
        }

        query = strtok(NULL, ";");
    }

    free(contenuto);
    return 1;
}


void esegui_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
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
            strncat(buffer_query, line, MAX_QUERY_LEN - strlen(buffer_query) - 2);
            strncat(buffer_query, "\n", MAX_QUERY_LEN - strlen(buffer_query) - 1);
        }
        line = strtok(NULL, "\n");
    }

    if (in_query && *query_count < MAX_QUERY_NUM) {
        strcpy(queries[*query_count].query, buffer_query);
        (*query_count)++;
    }

    return 1;
}