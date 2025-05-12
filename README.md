# Base di Dati di un Sistema di Gestione Crociere

Membri del gruppo:
- Ghiraldin Mirco 2102505
- Stevanin Michele 2101741

# Abstract
Il presente progetto propone la progettazione di una base di dati dedicata alla gestione delle crociere organizzate da compagnie marittime. L'obiettivo del sistema è quello di fornire una struttura chiara, coerente e relazionale per rappresentare tutte le informazioni chiave legate all’organizzazione e alla gestione di viaggi crocieristici. La base di dati consente di monitorare le compagnie crocieristiche, ciascuna delle quali gestisce diverse crociere, definite da itinerari, città di partenza e arrivo, date e caratteristiche operative della nave.
Ogni crociera prevede tappe intermedie nei porti, ed è popolata da persone, distinte tra passeggeri e membri dell’equipaggio. Quest’ultima categoria comprende anche figure specifiche come il capitano e gli animatori, modellate tramite generalizzazioni tra le entità. Il sistema gestisce inoltre le cabine, divise per classi di servizio, e gli eventi organizzati a bordo, con relativi animatori, orari, partecipanti e dettagli logistici.
Il progetto adotta un approccio modulare e normalizzato, con particolare attenzione alla rappresentazione gerarchica delle persone e all’ottimizzazione dell'accesso ai dati, facilitando l’analisi e l’elaborazione delle informazioni utili alla gestione delle attività crocieristiche.

# Analisi dei Requisiti
Questa sezione riassume i requisiti a cui deve sottostare la base di dati.

**Compagnia**. Entità che rappresenta le aziende crocieristiche:
- **Partita Iva Compagnia** (PK)
- **Nome**
- **Sede**
- **Contatto** 

**Crociera**. Identificata in modo univoco dal codice IMO, include:
- **IMO** (PK)
- **Nome nave**
- **Numero minimo membri equipaggio**
- **Città di Partenza**
- **Città di Fine**
- **Data e orario di partenza**
- **Durata in giorni**
- **Numero di passeggeri max**
- **Età barca**
- **Ultima ristutturazione**
- **Categoria crociera** transatlantica, mediterranea, fluviale

**Porti**. Ogni porto è identificato dalla sua città: 
- **Nome città** (PK)

**Tappa**. Ogni tappa rappresenta una sosta della crociera in un porto:
- **Data e ora di arrivo**
- **Data e ora di partenza**

**Classi**. Ogni classe è caratterizzata dal suo livello e dai vantiggi di quella classe:
- **Livello classe** economy, business...     (PK)
- **Vantaggi**                                (PK)
    
**Cabine**. Ogni cabina è caratterizzata da un costo e dal numero di posti dispobibili:
- **Costo crociera** (PK)
- **Numero posti** (PK)

**Persone**. 
- **CF** (PK)
- **Nome**
- **Cognome**
- **Sesso**

Le persone possono far parte dell'`equipaggio`.

**Equipaggio**. Ogni equipaggio ha il proprio stipendio, numero identificativo, anni di servizio, lingue parlate:
- **IDEquipaggio**
- **Lingue parlate**
- **Stipendio**
- **Anni di servizio**

Un mebro dell'equipaggio può essere `animatore`.

**Animatore**. Ogni animatore ha una o più abilità:
- **Abilità**

**Eventi**. Ogni evento è caratterizzato dal nome dell'evento, tipo di evento, data e ora, luogo, età consigliata, durata, numero di animatori minimo, numero massimo partecipanti.
- **Nome evento** (PK)
- **Data e ora** (PK)
- **Tipo evento**
- **Luogo**
- **Età consigliata**
- **Durata**
- **Numero minimo animatori**
- **Numero massimo partecipanti**
