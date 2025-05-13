# Base di Dati di un Sistema di Gestione Crociere

Membri del gruppo:
- Ghiraldin Mirco 2102505
- Stevanin Michele 2101741

# Abstract
Il presente progetto propone la progettazione di una base di dati dedicata alla gestione delle crociere organizzate da compagnie marittime. L'obiettivo del sistema è quello di fornire una struttura chiara, coerente e relazionale per rappresentare tutte le informazioni chiave legate all’organizzazione e alla gestione di viaggi crocieristici. La base di dati consente di monitorare le compagnie crocieristiche, ciascuna delle quali gestisce diverse crociere, definite da itinerari, città di partenza e arrivo, date e caratteristiche operative della nave.
Ogni crociera prevede tappe intermedie nei porti, ed è popolata da persone, distinte tra passeggeri e membri dell’equipaggio. Quest’ultima categoria comprende anche figure specifiche come il capitano e gli animatori, modellate tramite generalizzazioni tra le entità. Il sistema gestisce inoltre le cabine, divise per classi di servizio, e gli eventi organizzati a bordo, con relativi animatori, orari, partecipanti e dettagli logistici.
Il progetto adotta un approccio modulare e normalizzato, con particolare attenzione alla rappresentazione gerarchica delle persone e all’ottimizzazione dell'accesso ai dati, facilitando l’analisi e l’elaborazione delle informazioni utili alla gestione delle attività crocieristiche.

# Analisi dei Requisiti
Compagnie Marittime
Ogni compagnia è identificata da una partita IVA univoca e include le seguenti informazioni:
- Partita IVA (PK)
- Nome
- Sede
- Recapito

Le compagnie possiedono una o più crociere.

Crociere
Ogni crociera è identificata dal codice IMO e registra:
- Codice IMO (PK)
- Nome della nave
- Porto di partenza
- Porto di arrivo
- Data e ora di partenza
- Durata (in giorni)
- Numero minimo di membri dell’equipaggio
- Numero massimo di passeggeri
- Tipologia (es. mediterranea, fluviale, transatlantica)

Ogni crociera può avere più tappe intermedie e prevede eventi a bordo.

Porti
Ogni porto è identificato dalla città in cui si trova:
-Nome della città (PK)

Una crociera può prevedere soste in più porti (tappe).

Tappe
Le tappe rappresentano le fermate della crociera nei porti e includono:
- Data e ora di arrivo
- Data e ora di partenza

Ogni tappa è collegata a un porto e a una specifica crociera.

Persone
Tutti gli individui a bordo (sia personale che ospiti) sono entità del tipo "Persona", identificata tramite:
- Codice Fiscale (PK)
- Nome
- Cognome
- Sesso

Una persona può essere un ospite o un membro dell’equipaggio.

Equipaggio
L’equipaggio rappresenta il personale operativo a bordo. Ogni membro è associato a:
- Stipendio
- Anni di servizio
- Lingue parlate

Un membro dell’equipaggio può essere designato come capitano o come animatore.

Capitano
Ogni crociera è diretta da un capitano, che è un membro dell’equipaggio.

Animatori
Gli animatori sono una specializzazione dell’equipaggio e dispongono di una o più abilità:
- Abilità specifiche (es. ballo, canto, giochi)

Eventi
Durante le crociere vengono organizzati eventi, ciascuno identificato da:
- Nome dell’evento (PK)
- Tipo di evento (PK)
- Età consigliata
- Numero minimo di animatori
- Numero consigliato di partecipanti

Ogni evento è gestito da uno o più animatori e può essere frequentato da più ospiti.


