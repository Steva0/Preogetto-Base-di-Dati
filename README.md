# Sistema di Gestione Crociere - Database PostgreSQL

## Descrizione
Questo progetto implementa un database PostgreSQL per la gestione di un sistema di crociere, includendo navi, compagnie, porti, personale, passeggeri ed eventi.

## Struttura del Database

### Tabelle Principali

#### Compagnia
- Gestisce le informazioni delle compagnie di crociera
- Attributi: PI (Partita IVA), Nome, Sede, Recapito_Telefonico

#### Porto
- Contiene informazioni sui porti
- Attributi: Città (chiave primaria), Numero_Massimo_Navi

#### Crociera
- Gestisce le informazioni delle crociere
- Include: IMO, Nome_Nave, capacità equipaggio e passeggeri, porti di partenza/arrivo

#### Tappa
- Registra le fermate intermedie delle crociere
- Collegamenti con Crociera e Porto

#### Persona
- Informazioni base delle persone (sia ospiti che equipaggio)
- Attributi: CF, Nome, Cognome, Sesso

#### Ospite
- Dettagli dei passeggeri delle crociere
- Include costo del biglietto

#### Equipaggio e Animatore
- Gestione del personale di bordo
- Include stipendi, anni di servizio e competenze specifiche

#### Evento e ORGANIZZA
- Gestione degli eventi a bordo
- Collegamenti tra eventi e animatori

### Query Principali
- Ricerca crociere per numero di porti toccati
- Visualizzazione crociere per città di partenza
- Analisi costi medi dei biglietti
- Statistiche su animatori ed eventi
- Calcolo percentuale di occupazione delle navi
- Ricerca avanzata per tappe e costi

### Indici
- Indice HASH su Porto_Partenza nella tabella Crociera per ottimizzare le ricerche

## Caratteristiche
- Supporto multi-lingua per l'equipaggio
- Gestione completa degli eventi di bordo
- Tracciamento delle tappe intermedie
- Sistema di prenotazione con gestione costi
- Monitoraggio capacità navi e porti

## Utilizzo
Le query possono essere eseguite sostituendo i placeholder (es. '<PORTO>', '<PREZZO>') con i valori desiderati:

```sql
SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni
FROM Crociera c
WHERE c.Porto_Partenza = 'Genova';
```

## Note di Implementazione
- Il database utilizza vincoli di integrità referenziale
- Sono implementati controlli sulla capacità delle navi
- Le date sono gestite in formato TIMESTAMP
- I costi sono gestiti con precisione decimale (DECIMAL)