# Base di Dati di un Sistema di Gestione Crociere

Membri del gruppo:
- Ghiraldin Mirco 2102505
- Stevanin Michele 2101741

# 1 Abstract
Il presente progetto propone la progettazione di una base di dati dedicata alla gestione delle crociere organizzate da compagnie marittime. L'obiettivo del sistema è quello di fornire una struttura chiara, coerente e relazionale per rappresentare tutte le informazioni chiave legate all’organizzazione e alla gestione di viaggi crocieristici. La base di dati consente di monitorare le compagnie crocieristiche, ciascuna delle quali gestisce diverse crociere, definite da itinerari, città di partenza e arrivo, date e caratteristiche operative della nave.
Ogni crociera prevede tappe intermedie nei porti, ed è popolata da persone, distinte tra passeggeri e membri dell’equipaggio. Quest’ultima categoria comprende anche figure specifiche come il capitano e gli animatori, modellate tramite generalizzazioni tra le entità. Il sistema gestisce inoltre le cabine, divise per classi di servizio, e gli eventi organizzati a bordo, con relativi animatori, orari, partecipanti e dettagli logistici.
Il progetto adotta un approccio modulare e normalizzato, con particolare attenzione alla rappresentazione gerarchica delle persone e all’ottimizzazione dell'accesso ai dati, facilitando l’analisi e l’elaborazione delle informazioni utili alla gestione delle attività crocieristiche.

# 2 Analisi dei Requisiti

**Compagnie Marittime**

Ogni compagnia è identificata da una partita IVA univoca e include le seguenti informazioni:
- Partita IVA (PK)
- Nome
- Sede
- Recapito

Le compagnie possiedono una o più crociere.

**Crociere**

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

**Porti**

Ogni porto è identificato dalla città in cui si trova:
- Nome della città (PK)

Una crociera può prevedere soste in più porti (tappe).

**Tappe**

Le tappe rappresentano le fermate della crociera nei porti e includono:
- Data e ora di arrivo
- Data e ora di partenza

Ogni tappa è collegata a un porto e a una specifica crociera.

**Persone**

Tutti gli individui a bordo (sia personale che ospiti) sono entità del tipo "Persona", identificata tramite:
- Codice Fiscale (PK)
- Nome
- Cognome
- Sesso

Una persona può essere un ospite o un membro dell’equipaggio.

**Equipaggio**

L’equipaggio rappresenta il personale operativo a bordo. Ogni membro è associato a:
- Stipendio
- Anni di servizio
- Lingue parlate

Un membro dell’equipaggio può essere designato come capitano o come animatore.

**Animatori**

Gli animatori sono una specializzazione dell’equipaggio e dispongono di una o più abilità:
- Abilità specifiche (es. ballo, canto, giochi)

**Eventi**

Durante le crociere vengono organizzati eventi, ciascuno identificato da:
- Nome dell’evento (PK)
- Tipo di evento (PK)
- Età consigliata
- Numero minimo di animatori
- Numero consigliato di partecipanti

Ogni evento è gestito da uno o più animatori e può essere frequentato da più ospiti.

# 3 Progettazione Concettuale
La Figura ?? riporta il diagramma Entità–Relazione (E–R) che riassume i requisiti descritti nella sezione 2.

Nel modello la progettazione concettuale si è basata su una visione gerarchica del personale di bordo, dove la generalizzazione rappresenta il fatto che tutti i membri dell’equipaggio e gli ospiti condividono un insieme comune di attributi base, ma possono essere specializzati in ruoli specifici.

Ogni crociera è organizzata da una compagnia marittima e ha una propria identità definita dal codice IMO. Le crociere sono associate a uno o più porti attraverso una relazione Tappa, che rappresenta gli scali effettuati durante il viaggio. Ogni tappa è caratterizzata da una data e ora di arrivo e partenza, ed è sempre riferita a un porto e a una crociera specifici.

Le persone imbarcate sono suddivise in Equipaggio mediante una generalizzazione parziale. Il personale di bordo può ricoprire ruoli specializzati: ad esempio gli Animatori, sottoinsieme dell’equipaggio, sono incaricati dell’organizzazione e conduzione degli eventi.

Gli Eventi sono attività previste a bordo della crociera. Ogni evento può essere gestito da uno o più animatori (tramite la relazione Gestione). Gli eventi sono identificati in modo univoco dalla combinazione tra nome e tipologia e presentano attributi come età consigliata e limiti sulla partecipazione.

La generalizzazione tra Equipaggio e la specializzazione Animatore è modellata come parziale: non tutti i membri dell’equipaggio sono necessariamente animatori.

Tabella ?? riassume le entità e relazioni individuate nella progettazione concettuale, riportando per ciascuna gli attributi rilevanti e l’identificatore scelto. Per le entità derivate da generalizzazione viene anche specificato il tipo di specializzazione utilizzato.

# 4 Progettazione Logica
In questa sezione viene illustrato il processo di “traduzione” dello schema concettuale in uno schema logico, con l’obiettivo di rappresentare i dati in modo preciso
ed efficiente. Il primo passo consiste nell’analizzare le eventuali ridondanze nel modello, al fine di ottimizzare la struttura complessiva. Successivamente, si procede
con l’eliminazione delle due generalizzazioni. Infine, viene presentato il diagramma
ristrutturato, con una descrizione delle modifiche apportate.

# 4.1 Analisi delle ridondanze
L’attributo Persone_Prenotate in CROCIERA, che memorizza il numero di persone prenotate in quella crociera presenta una ridondanza. Questo valore può essere infatti ottenuto
contando il numero di passeggeri attivi per quella crociera tramite la relazione PARTECIPANTE.
Questo attributo viene modificato ogni volta che si aggiunge una nuova persona alla crociera (circa 400 persone nuove al giorno tra tutte le crociere) e viene visualizzato ogni ora del giorno per monitorare il numero di posti rimanenti. Questo si riassume nelle seguenti due operazioni:

- Operazione 1 (400 al giorno): memorizza una nuova prenotazione in relativa crociera.
- Operazione 2 (24 al giorno): visualizza il numero di prenotazioni attuali in una crociera.

Assumendo i seguenti volumi nella base di dati:

| Concetto | Costrutto | Volume|
|----------|-----------|-------|
| CROCIERA | E | 30 |
| PARTECIPANTE | R | 150000 |
| PASSEGGERO | E | 150000 |

la seguente analisi serve per stabilire se sia utile o meno tenere l’attributo ridondante Persone_Prenotate in CROCIERA.

**CON RIDONDANZA** Analizziamo prima il costo totale con ridondanza.
- Operazione 1:
  |Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
  |---------|-----------|---------|------|-------------|
  |PASSEGGERO | E | 1 | S | × 400 |
  |PARTECIPANTE | R | 1 | S | × 400 |
  |CROCIERA | E | 1 | L | × 400 |
  |CROCIERA | E | 1 | S | × 400 |
- Operazione2:
  |Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
  |---------|-----------|---------|------|-------------|
  |Crociera | E | 1 | L | × 24 |

Assumendo costo doppio per gli accessi in scrittura:

Costo Totale = 400x3x2 + 400 + 24 = 2824

**SENZA RIDONDANZA** Analizziamo il costo totale senza ridondanza.
- Operazione 1:
  |Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
  |---------|-----------|---------|------|-------------|
  |PASSEGGERO | E | 1 | S | × 400 |
  |PARTECIPANTE | R | 1 | S | × 400 |
- Operazione 2 (con circa 150000/30 = 5000 passeggeri al giorno)
  |Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
  |---------|-----------|---------|------|-------------|
  |CROCIERA | E | 1 | L | × 24 |
  |PARTECIPANTE | R | 5000 | L | × 24 |

Assumendo costo doppio per gli accessi in scrittura:

Costo Totale = 400x2x2 + 5001x24 = 121624

L’analisi suggerisce quindi di tenere l’attributo ridondante, ottimizzando così il numero di accessi.

# 4.2 Eliminazioni delle Generalizzazioni
Le generalizzazioni descritte in Sezione 3 vengono eliminate attraverso una ristrutturazione dello schema concettuale, con l’obiettivo di semplificare la successiva implementazione del modello relazionale e ridurre la presenza di valori nulli. Le modifiche vengono applicate come segue:

**PERSONA**. La generalizzazione parziale PERSONA viene sostituita con la relazione IS-EQUIP (vedi Figura ??).
Questa scelta consente di evitare la presenza di valori nulli che si verificherebbero mantenendo un’unica entità PERSONA con tutti gli attributi specifici di EQUIPAGGIO (come Stipendio, Anni_Di_Servizio, Lingue_Parlate).
Separando le informazioni tramite relazioni specializzate, si garantisce che ogni entità contenga esclusivamente gli attributi rilevanti per il proprio ruolo. In accordo con la metodologia vista a lezione, l’identificatore della relazione Equipaggio coincide con quello della corrispondente persona.
Poiché la generalizzazione è parziale (non tutte le persone fanno parte dell’equipaggio), la rimozione dell’entità padre PERSONA non sarebbe corretta.

**EQUIPAGGIO**. Analogamente, la generalizzazione parziale EQUIPAGGIO viene sostituita con la relazione IS-ANIM (vedi Figura ??).
Anche in questo caso, la ristrutturazione consente di evitare valori nulli, in quanto non tutti i membri dell’equipaggio svolgono il ruolo di Animatore.
Essendo la generalizzazione parziale, l’eliminazione dell’entità padre EQUIPAGGIO risulterebbe nuovamente scorretta.

Il diagramma E–R ristrutturato, riportato in Figura ??, riflette tali modifiche rispetto alla versione originale presentata in Figura ??.

# 4.3 Schema Relazionale
Lo schema ristrutturato in Figura ?? contiene solamente costrutti mappabili in corrispettivi dello schema relazionale, detto anche schema logico. Lo schema logico è rappresentato a seguire, dove l’asterisco dopo il nome degli attributi indica quelli che ammettono valori nulli.

- **Porto**(<ins>Città</ins>)
- **Tappa**(<ins>Località</ins>, <ins>Codice</ins>, Data_Ora_Partenza, Data_Ora_Arrivo)
  - Tappa.Località -> Porto.Città
  - Tappa.Codice -> Crociera.IMO
- **Crociera**(<ins>IMO</ins>, Nome_Nave, Min_Equipaggio, Max_Passeggeri, Num_Prenotazioni*, Data_Ora_Partenza*, Porto_Partenza*, Porto_Finale*, Durata*)
- **Compagnia**(<ins>PI_Compagnia</ins>, Nome_Compagnia, Sede, Recapito)
- **Passeggero**(<ins>Persona</ins>, <ins>Costo</ins>)
  - Passeggero.Persona -> Persona.CF
- **Persona**(<ins>CF</ins>, Nome, Cognome, Sesso)
- **Equipaggio**(<ins>Persona</ins>, ID_Equipaggio, Lingue, Stipendio, Anni_Servizio)
  - Equipaggio.Persona -> Persona.CF
- **Animatore**(<ins>Persona</ins>, Abilità)
  - Animatore.Persona -> Equipaggio.Persona
- **Evento**(<ins>Nome_Evento</ins>, <ins>Tipo</ins>, Num_Cons_Partecipanti, Età_Min_Cons, Num_Min_Animatori)
