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
    // Controlliamo se il database esiste provando a eseguire una query su di esso
    PGresult *res = PQexec(conn, "SELECT 1;");
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        PQclear(res);
        return 1; // Il database esiste
    } else {
        PQclear(res);
        return 0; // Il database non esiste
    }
}

void createDatabaseIfNotExists(PGconn *conn, const char *dbname) {
    // Proviamo a connetterci al database, se fallisce lo creiamo
    char query[256];
    snprintf(query, sizeof(query), "CREATE DATABASE %s;", dbname);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore nella creazione del database: %s", PQerrorMessage(conn));
        PQclear(res);
        exit(1);
    }

    PQclear(res);
    printf("Database '%s' creato con successo.\n", dbname);
}

int checkScriptExecution() {
    // Esegui il comando psql e cattura l'output
    FILE *fp = popen("psql -U postgres -d crociere -f Crociere.sql", "r");
    if (fp == NULL) {
        fprintf(stderr, "Errore nell'aprire il comando psql.\n");
        return 0;  // Errore nell'aprire il comando
    }

    // Leggi l'output del comando psql e verifica se ci sono errori
    char buffer[256];
    int success = 1;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "ERROR") != NULL) {
            fprintf(stderr, "Errore nell'esecuzione dello script SQL: %s", buffer);
            success = 0;  // C'è stato un errore
            break;
        }
    }

    fclose(fp);
    return success;
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

    // Stampa intestazioni
    for (int i = 0; i < n_fields; i++) {
        printf("%-25s", PQfname(res, i));
    }
    printf("\n");

    // Stampa dati
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_fields; j++) {
            printf("%-25s", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res);
}

int main() {
    const char *conninfo = "user=postgres host=localhost port=5432";
    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione al server di database fallita: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Controlla se il database esiste, se non esiste lo crea
    if (!checkDatabaseExistence(conn, "crociere")) {
        printf("Il database 'crociere' non esiste. Creazione in corso...\n");
        createDatabaseIfNotExists(conn, "crociere");
    }

    // Dopo aver creato o verificato il database, prova a connetterti di nuovo
    PQfinish(conn); // Chiudiamo la connessione al server di gestione per creare il database
    conn = PQconnectdb("user=postgres dbname=crociere host=localhost port=5432");
    checkConn(conn);

    // Esegui lo script SQL per popolare il database
    printf("Esecuzione dello script SQL per popolare il database...\n");
    if (!checkScriptExecution()) {
        fprintf(stderr, "Errore durante l'esecuzione dello script SQL.\n");
        PQfinish(conn);
        return 1;
    }
    printf("Script SQL eseguito correttamente.\n");

    // Menu interattivo per l'esecuzione delle query 
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
                    "SELECT C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale, COUNT(DISTINCT T.Città) AS Numero_Tappe "
                    "FROM Crociera C "
                    "JOIN Tappa T ON C.IMO = T.IMO "
                    "GROUP BY C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale "
                    "HAVING COUNT(DISTINCT T.Città) > 3 "
                    "ORDER BY Numero_Tappe DESC;"
                );
                break;
            case 2: {
                char citta[50];
                printf("Inserisci la città di partenza: ");
                scanf("%s", citta); // Usa %s se la città è una parola, oppure fgets() per nomi composti
            
                char query[512];
                snprintf(query, sizeof(query),
                    "SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni "
                    "FROM Crociera c "
                    "WHERE c.Porto_Partenza = '%s';",
                    citta);
            
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
