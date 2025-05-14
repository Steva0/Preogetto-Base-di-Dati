#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

void esegui_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nella query: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

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
    const char *conninfo = "dbname=TUO_DATABASE user=TUO_UTENTE password=TUAPASSWORD hostaddr=127.0.0.1 port=5432";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Errore di connessione: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    int scelta;
    while (1) {
        printf("\nMenu - Seleziona una query da eseguire:\n");
        printf("1. Crociere con più di 3 tappe\n");
        printf("2. Numero crociere per porto e media prenotazioni\n");
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

    return 0;
}
