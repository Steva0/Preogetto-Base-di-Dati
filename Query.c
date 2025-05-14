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
        printf("%-20s", PQfname(res, i));
    }
    printf("\n");

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_fields; j++) {
            printf("%-20s", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res);
}

void query_parametrica(PGconn *conn) {
    char data[20];
    printf("Inserisci la data (YYYY-MM-DD): ");
    scanf("%s", data);

    const char *paramValues[1] = { data };
    PGresult *res = PQexecParams(conn,
        "SELECT p.Nome, p.Cognome, pa.Data_Parto, o.Regione "
        "FROM Parto pa "
        "JOIN Persona p ON pa.Persona = p.CF "
        "JOIN Ostetricia os ON pa.Ospedale = os.Ospedale AND pa.Reparto = os.Reparto "
        "JOIN Reparto r ON os.Ospedale = r.Ospedale AND os.Reparto = r.Codice "
        "JOIN Ospedale o ON r.Ospedale = o.Codice "
        "WHERE pa.Data_Parto > $1 "
        "ORDER BY pa.Data_Parto DESC;",
        1,       // number of parameters
        NULL,    // param types
        paramValues,
        NULL,    // param lengths
        NULL,    // param formats
        0        // text format
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nella query parametrica: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int n_rows = PQntuples(res);
    int n_fields = PQnfields(res);

    for (int i = 0; i < n_fields; i++) {
        printf("%-20s", PQfname(res, i));
    }
    printf("\n");

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_fields; j++) {
            printf("%-20s", PQgetvalue(res, i, j));
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
        printf("\nScegli una query:\n");
        printf("1. Query 1\n");
        printf("2. Query 2 (parametrica)\n");
        printf("3. Query 3\n");
        printf("4. Query 4\n");
        printf("5. Query 5\n");
        printf("0. Esci\n");
        printf("Scelta: ");
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                esegui_query(conn, "QUERY_1_SQL_COMPLETA;");
                break;
            case 2:
                query_parametrica(conn);
                break;
            case 3:
                esegui_query(conn, "QUERY_3_SQL_COMPLETA;");
                break;
            case 4:
                esegui_query(conn, "QUERY_4_SQL_COMPLETA;");
                break;
            case 5:
                esegui_query(conn, "QUERY_5_SQL_COMPLETA;");
                break;
            case 0:
                PQfinish(conn);
                exit(0);
            default:
                printf("Scelta non valida\n");
        }
    }
}
