#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void checkConn(PGconn *conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Errore di connessione: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

int checkDatabaseExistence(PGconn *conn, const char *dbname) {
    char conninfo[256];
    snprintf(conninfo, sizeof(conninfo), "user=postgres dbname=%s host=localhost port=5432", dbname);
    PGconn *testConn = PQconnectdb(conninfo);
    int exists = (PQstatus(testConn) == CONNECTION_OK);
    PQfinish(testConn);
    return exists;
}

void createDatabaseIfNotExists(PGconn *conn, const char *dbname) {
    char query[256];
    snprintf(query, sizeof(query), "CREATE DATABASE %s;", dbname);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore nella creazione del database: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
    printf("Database '%s' creato con successo.\n", dbname);
}

char* leggi_file_sql(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Errore nell'apertura del file %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = (char*)malloc(size + 1);
    if (!content) {
        fprintf(stderr, "Errore nell'allocazione della memoria\n");
        fclose(file);
        return NULL;
    }

    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);

    return content;
}

int esegui_script_sql_righe(PGconn *conn, const char *filename) {
    char *script = leggi_file_sql(filename);
    if (!script) return 0;

    char *comando = strtok(script, ";");
    while (comando != NULL) {
        while (*comando == '\n' || *comando == ' ') comando++;  // Skip spazi iniziali
        if (strlen(comando) > 0) {
            PGresult *res = PQexec(conn, comando);
            if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
                fprintf(stderr, "Errore nel comando SQL:\n%s\n%s\n", comando, PQerrorMessage(conn));
                PQclear(res);
                free(script);
                return 0;
            }
            PQclear(res);
        }
        comando = strtok(NULL, ";");
    }

    free(script);
    return 1;
}

void checkResult(PGresult *res, PGconn *conn) {
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore nell'esecuzione della query: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
}

void esegui_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query);
    checkResult(res, conn);

    int n_rows = PQntuples(res);
    int n_fields = PQnfields(res);

    for (int i = 0; i < n_fields; i++) {
        printf("%-25s", PQfname(res, i));
    }
    printf("\n");

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_fields; j++) {
            printf("%-25s", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res);
}

int main() {
    const char *dbname = "crociere";
    const char *script_filename = "Crociere.sql";

    const char *conninfo_root = "user=postgres host=localhost port=5432";
    PGconn *conn = PQconnectdb(conninfo_root);
    checkConn(conn);

    if (!checkDatabaseExistence(conn, dbname)) {
        printf("Il database '%s' non esiste. Creazione in corso...\n", dbname);
        createDatabaseIfNotExists(conn, dbname);
    }
    PQfinish(conn);

    char conninfo_db[256];
    snprintf(conninfo_db, sizeof(conninfo_db), "user=postgres dbname=%s host=localhost port=5432", dbname);
    conn = PQconnectdb(conninfo_db);
    checkConn(conn);

    printf("Esecuzione dello script SQL da file '%s'...\n", script_filename);
    if (!esegui_script_sql_righe(conn, script_filename)) {
        fprintf(stderr, "Errore durante l'esecuzione dello script SQL.\n");
        PQfinish(conn);
        return 1;
    }
    printf("Script SQL eseguito correttamente.\n");

    int scelta;
    while (1) {
        printf("\nMenu - Seleziona una query da eseguire:\n");
        printf("1. Crociere con più di 3 tappe\n");
        printf("2. Visualizza crociere in partenza da una certa città\n");
        printf("3. Crociere con media costo biglietti > 500€\n");
        printf("4. Animatori con più di 2 eventi\n");
        printf("5. Percentuale occupazione crociere\n");
        printf("0. Esci\n");
        printf("Scelta: ");
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                esegui_query(conn,
                    "SELECT c.imo, c.nome_nave, c.porto_partenza, c.porto_finale, COUNT(DISTINCT t.città) AS numero_tappe "
                    "FROM crociera c "
                    "JOIN tappa t ON c.imo = t.imo "
                    "GROUP BY c.imo, c.nome_nave, c.porto_partenza, c.porto_finale "
                    "HAVING COUNT(DISTINCT t.città) > 3 "
                    "ORDER BY numero_tappe DESC;"
                );
                break;
            case 2: {
                char citta[50];
                printf("Inserisci la città di partenza: ");
                scanf("%s", citta);

                char query[512];
                snprintf(query, sizeof(query),
                    "SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni "
                    "FROM Crociera c "
                    "WHERE c.Porto_Partenza = '%s';", citta);

                esegui_query(conn, query);
                break;
            }
            case 3:
                esegui_query(conn,
                    "SELECT O.IMO_Crociera, C.Nome_Nave, AVG(O.Costo) AS Media_Costo "
                    "FROM Ospite O "
                    "JOIN Crociera C ON O.IMO_Crociera = C.IMO "
                    "GROUP BY O.IMO_Crociera, C.Nome_Nave "
                    "HAVING AVG(O.Costo) > 500 "
                    "ORDER BY Media_Costo DESC;"
                );
                break;
            case 4:
                esegui_query(conn,
                    "SELECT A.CF, P.Nome, P.Cognome, COUNT(*) AS Num_Eventi "
                    "FROM Animatore A "
                    "JOIN Persona P ON A.CF = P.CF "
                    "JOIN ORGANIZZA O ON A.CF = O.CF_Animatore "
                    "GROUP BY A.CF, P.Nome, P.Cognome "
                    "HAVING COUNT(*) > 2 "
                    "ORDER BY Num_Eventi DESC;"
                );
                break;
            case 5:
                esegui_query(conn,
                    "SELECT IMO, Nome_Nave, "
                    "ROUND((Num_Prenotazioni * 100.0) / Max_Passeggeri, 2) AS Percentuale_Occupazione "
                    "FROM Crociera "
                    "WHERE Max_Passeggeri > 0 "
                    "ORDER BY Percentuale_Occupazione DESC;"
                );
                break;
            case 0:
                PQfinish(conn);
                printf("Connessione chiusa. Uscita.\n");
                return 0;
            default:
                printf("Scelta non valida. Riprova.\n");
        }
    }

    PQfinish(conn);
    return 0;
}
