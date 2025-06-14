<img src="img/1pagina.png" alt="1Pagina"/>

<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>


# **1 Abstract**
In questo elaborato viene presentato lo sviluppo di una base di dati relazionale pensata per **gestire le attività delle compagnie di crociere**. L’obiettivo è costruire un sistema informativo in grado di organizzare in modo chiaro ed efficiente tutte le informazioni legate alla gestione delle navi, del personale e delle attività a bordo.
Il progetto copre l’intero ciclo di vita di una crociera: si parte dall’identificazione delle compagnie organizzatrici (tramite partita IVA), per arrivare alla gestione delle singole crociere, ciascuna con il proprio itinerario, i porti di partenza e arrivo, le date, le tappe intermedie e altri dettagli tecnici. 
A bordo interagiscono diverse figure: passeggeri, membri dell’equipaggio e animatori, rappresentati nel modello tramite un sistema di generalizzazioni e specializzazioni che permette di descrivere con flessibilità i diversi ruoli.
Il modello affronta anche aspetti legati alle prestazioni del sistema, come ad esempio l’analisi della ridondanza relativa al numero di passeggeri prenotati. Il progetto è stato sviluppato seguendo un approccio modulare, ispirato alle metodologie viste durante il corso, con l’obiettivo di garantire integrità, coerenza e adattabilità in contesti reali di gestione delle crociere.

# **2 Analisi dei Requisiti**

## **Compagnie Marittime**

Ogni compagnia è identificata da una partita IVA univoca e include le seguenti informazioni:
- **Partita IVA** (PK)
- **Nome**
- **Sede**
- **Recapito**

## **Crociere**

Ogni crociera è identificata dal codice IMO e registra:
- **Codice IMO** (PK)
- **Nome della nave**
- **Porto di partenza**
- **Porto di arrivo**
- **Data e ora di partenza**
- **Durata** (in giorni)
- **Numero minimo di membri dell’equipaggio**
- **Numero massimo di passeggeri**
- **Tipologia** (es. mediterranea, fluviale, transatlantica)

Ogni crociera:
- È di proprietà di una **compagnia**
- Parte da un **porto** e arriva in un altro e può prevedere **tappe** intermedie
- Ha a bordo **ospiti** e **membri dell’equipaggio**
- Include **eventi** gestiti da **animatori**

## **Porti**

Ogni porto è identificato dalla città in cui si trova:
- **Nome della città** (PK)
- **Numero massimo di navi**

Una crociera può prevedere soste in più porti (tappe).

## **Tappe**

Le tappe rappresentano le fermate della crociera nei porti e includono:
- **Data e ora di arrivo**
- **Data e ora di partenza**

Ogni tappa è collegata a un porto e a una specifica crociera.

## **Persone**

Tutti gli individui a bordo (sia personale che ospiti) sono entità del tipo "Persona", identificata tramite:
- **Codice Fiscale** (PK)
- **Nome**
- **Cognome**
- **Sesso**

Una persona può essere un ospite o un membro dell’equipaggio.

## **Ospiti** (Passeggeri)

Gli ospiti sono persone che hanno prenotato una crociera:
- **Codice Fiscale** (PK, FK da Persona)
- **Costo della crociera**
- **Crociera** (IMO) (FK)

## **Equipaggio**

L’equipaggio rappresenta il personale operativo a bordo. Ogni membro è associato a:
- **Codice Fiscale** (PK, FK da Persona)
- **ID equipaggio**
- **Stipendio**
- **Anni di servizio**
- **Lingue parlate**

Un membro dell’equipaggio può essere specializzato in animatore.

<br>
<br>
<br>

## **Animatori**

Gli animatori sono una specializzazione dell’equipaggio e dispongono di una o più abilità:
- **Codice Fiscale** (PK, FK da Equipaggio)
- **Abilità specifiche** (es. ballo, canto, giochi)

## **Eventi**

Durante le crociere vengono organizzati eventi, ciascuno identificato da:
- **Nome dell’evento** (PK)
- **Tipo di evento** (PK)
- **Età consigliata**
- **Numero minimo di animatori**
- **Numero consigliato di partecipanti**

Ogni evento è gestito da uno o più animatori e può essere frequentato da più ospiti.

<img src="img/Con_Generalizzazione.png" alt="Grafico1" width="700"/>
(Grafico1)


<br>
<br>
<br>

# **3 Progettazione Concettuale**
La Figura 1 riporta il diagramma Entità–Relazione (E–R) che riassume i requisiti descritti nella sezione 2.
Nel modello la progettazione concettuale si è basata su una visione gerarchica del personale di bordo, dove la generalizzazione rappresenta il fatto che tutti i membri dell’equipaggio e gli ospiti condividono un insieme comune di attributi base, ma possono essere specializzati in ruoli specifici.
Ogni crociera è organizzata da una compagnia marittima e ha una propria identità definita dal codice IMO. Le crociere sono associate a uno o più porti attraverso una relazione Tappa, che rappresenta gli scali effettuati durante il viaggio. Ogni tappa è caratterizzata da una data e ora di arrivo e partenza, ed è sempre riferita a un porto e a una crociera specifici.
Le persone imbarcate sono suddivise in Equipaggio mediante una generalizzazione parziale. Il personale di bordo può ricoprire ruoli specializzati: ad esempio gli Animatori, sottoinsieme dell’equipaggio, sono incaricati dell’organizzazione e conduzione degli eventi.
Gli Eventi sono attività previste a bordo della crociera. Ogni evento può essere gestito da uno o più animatori (tramite la relazione Gestione). Gli eventi sono identificati in modo univoco dalla combinazione tra nome e tipologia e presentano attributi come età consigliata e limiti sulla partecipazione.
La generalizzazione tra Equipaggio e la specializzazione Animatore è modellata come parziale: non tutti i membri dell’equipaggio sono necessariamente animatori.
Tabella 1 riassume le entità e relazioni individuate nella progettazione concettuale, riportando per ciascuna gli attributi rilevanti e l’identificatore scelto. Per le entità derivate da generalizzazione viene anche specificato il tipo di specializzazione utilizzato.
Il presente schema E-R non permette di rappresentare direttamente il seguente vincolo:
se una persona pe è animatore in due crociere cr' e cr'', allora cr' e cr'' appartengono alla stessa Classe:
```math
(cr', pe) ∈ Animatore ∧ (cr'', pe) ∈ Animatore ⇒ (cr'.Classe = cr''.Classe)
```
# **4 Progettazione Logica**
In questa sezione viene illustrato il processo di “traduzione” dello schema concettuale in uno schema logico, con l’obiettivo di rappresentare i dati in modo preciso
ed efficiente. Il primo passo consiste nell’analizzare le eventuali ridondanze nel modello, al fine di ottimizzare la struttura complessiva. Successivamente, si procede
con l’eliminazione delle due generalizzazioni. Infine, viene presentato il diagramma ristrutturato, con una descrizione delle modifiche apportate.


| **Entità** | **Descrizione**                                     | **Attributi**                                                                                                              | **Identificatore**                     |
| ---------- | --------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- | -------------------------------------- |
| Crociera   | Mezzo di trasporto marittimo                        | IMO, Nome nave, Porto partenza, Porto finale, Data/ora partenza, Durata, Min equipaggio, Max passeggeri, Persone prenotate | IMO                                    |
| Porto      | Località in cui la crociera può attraccare          | Città, Numero massimo navi                                                                                                 | Città                                  |
| Persona    | Persone partecipanti alle crociere                  | Codice fiscale (CF), Nome, Cognome, Sesso                                                                                  | CF                                     |
| Passeggero | Passeggeri ospiti della crociera                    | Codice fiscale (CF), Costo, IMO crociera                                                                                   | CF                                     |
| Equipaggio | Persone membri dell’equipaggio                      | Codice fiscale (CF), Id equipaggio, Lingue parlate, Stipendio, Anni di servizio, IMO crociera                              | CF                                     |
| Animatore  | Membri dell’equipaggio che si occupano degli eventi | Codice fiscale (CF), Abilità                                                                                               | CF                                     |
| Compagnia  | Società che possiedono le navi da crociera          | Partita IVA (P.I. compagnia), Nome, Sede, Recapito                                                                         | P.I. compagnia                         |
| Evento     | Eventi ricreativi svolti a bordo delle crociere     | Nome evento, Tipo evento, Età consigliata, Numero minimo animatori, Numero consigliato partecipanti, IMO crociera          | Nome evento, Tipo evento, IMO crociera |

Tabella 1 (Entità)


| **Relazione** | **Descrizione**                                     | **Componenti**                | **Attributi**                      |
| ------------- | --------------------------------------------------- | ----------------------------- | ---------------------------------- |
| Tappa         | Tappe portuali fatte da una nave                    | Porto, Crociera               | Data/ora arrivo, Data/ora partenza |
| Partecipante  | Assegnazione di una persona (ospite) a una crociera | Crociera, Passeggero (ospite) | –                                  |
| Staff         | Membri dell’equipaggio assegnati a una crociera     | Crociera, Equipaggio          | –                                  |
| Proprietà     | Navi da crociera possedute da una compagnia         | Crociera, Compagnia           | –                                  |
| Disponibilità | Eventi programmati per una crociera                 | Crociera, Evento              | –                                  |
| Organizza     | Assegnazione di uno o più animatori a un evento     | Evento, Animatore             | –                                  |

Tabella 2 (Relazioni)


<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>


## **4.1 Analisi delle ridondanze**
L’attributo Persone_Prenotate in CROCIERA, che memorizza il numero di persone prenotate in quella crociera, presenta una ridondanza. Questo valore può essere infatti ottenuto
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


La seguente analisi serve per stabilire se sia utile o meno tenere l’attributo ridondante Persone_Prenotate in CROCIERA.

**CON RIDONDANZA** 

Analizziamo prima il costo totale con ridondanza.

- Operazione 1:

|Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
|---------|-----------|---------|------|-------------|
|PASSEGGERO | E | 1 | S | × 400 |
|PARTECIPANTE | R | 1 | S | × 400 |
|CROCIERA | E | 1 | L | × 400 |
|CROCIERA | E | 1 | S | × 400 |
  
- Operazione 2:

|Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
|---------|-----------|---------|------|-------------|
|Crociera | E | 1 | L | × 24 |


Assumendo costo doppio per gli accessi in scrittura:
```math
Costo Totale = 400 * 3 * 2 + 400 + 24 = 2824
```
<br>

**SENZA RIDONDANZA** 

Analizziamo il costo totale senza ridondanza.
- Operazione 1:

|Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
|---------|-----------|---------|------|-------------|
|PASSEGGERO | E | 1 | S | × 400 |
|PARTECIPANTE | R | 1 | S | × 400 |

- Operazione 2 (con circa 150000/30 = 5000 passeggeri al giorno):

|Concetto | Costrutto | Accessi | Tipo | Ripetizioni |
|---------|-----------|---------|------|-------------|
|CROCIERA | E | 1 | L | × 24 |
|PARTECIPANTE | R | 5000 | L | × 24 |

Assumendo costo doppio per gli accessi in scrittura:
```math
Costo Totale = 400 * 2 * 2 + 5001 * 24 = 121624
```
L’analisi suggerisce quindi di tenere l’attributo ridondante, ottimizzando così il numero di accessi.

## **4.2 Eliminazioni delle Generalizzazioni**
Le generalizzazioni descritte in Sezione 3 vengono eliminate attraverso una ristrutturazione dello schema concettuale, con l’obiettivo di semplificare la successiva implementazione del modello relazionale e ridurre la presenza di valori nulli. Le modifiche vengono applicate come segue:

**PERSONA**. La generalizzazione parziale PERSONA viene sostituita con le relazioni **IS-OSPITE** e **IS-EQUIP** (vedi Figura 2), che collega alcuni individui alla relativa specializzazione: EQUIPAGGIO o PASSEGGIERO.
Tale scelta consente di evitare la presenza di valori nulli che si verificherebbero mantenendo un’unica entità PERSONA con tutti gli attributi specifici delle due categorie (ad esempio, Stipendio, Anni_Di_Servizio, Lingue_Parlate per EQUIPAGGIO, Costo per PASSEGGERO).
Separando le informazioni tramite relazioni specializzate, si garantisce che ciascuna entità contenga soltanto gli attributi rilevanti per il proprio ruolo.
In linea con la metodologia adottata a lezione, l’identificatore di EQUIPAGGIO e di PASSEGGERO coincide con quello della rispettiva PERSONA.
Poiché la generalizzazione è parziale da entrambe le parti (non tutte le persone sono membri dell’equipaggio né passeggeri), l’eliminazione dell’entità padre PERSONA non è corretta. Essa viene mantenuta per rappresentare tutte le informazioni comuni (come Nome, Cognome, Data_Nascita), mentre le informazioni specifiche sono distribuite nelle entità figlie.

**EQUIPAGGIO**. Analogamente, la generalizzazione parziale EQUIPAGGIO viene sostituita con **IS-ANIM** (vedi Figura 2).
Anche in questo caso, la ristrutturazione consente di evitare valori nulli, in quanto non tutti i membri dell’equipaggio svolgono il ruolo di Animatore.
Essendo la generalizzazione parziale, l’eliminazione dell’entità padre EQUIPAGGIO risulterebbe nuovamente scorretta.

Il diagramma E–R ristrutturato, riportato in **Figura 2** riflette tali modifiche rispetto alla versione originale presentata in **Figura 1**.

<div align="center">

<img src="img/Senza_Generalizzazione.png" alt="Grafico2" width="700"/>

(Grafico2)

</div>

## **4.3 Schema Relazionale**
Lo schema ristrutturato in Figura 2 contiene solamente costrutti mappabili in corrispettivi dello schema relazionale, detto anche schema logico. Lo schema logico è rappresentato a seguire, dove l’asterisco dopo il nome degli attributi indica quelli che ammettono valori nulli.

- **Crociera**(<ins>IMO</ins>, Nome_Nave, Min_Equipaggio, Max_Passeggeri, Num_Prenotazioni, Porto_Partenza*, Porto_Finale*, Data_Ora_Partenza*, Durata*, PI_Compagnia)
  - Crociera.PI_Compagnia -> Compagnia.PI
  - Crociera.Porto_Partenza -> Porto.Città
  - Crociera.Porto_Finale -> Porto.Città
- **Porto**(<ins>Città</ins>, Numero_Massimo_Navi*)
- **Tappa**(<ins>IMO</ins>, <ins>Città</ins>, <ins>Data_Ora_Partenza</ins>, Data_Ora_Arrivo)
  - Tappa.IMO -> Crociera.IMO
  - Tappa.Città -> Porto.Città
- **Compagnia**(<ins>PI</ins>, Nome, Sede, Recapito_Telefonico*)
- **Persona**(<ins>CF</ins>, Nome, Cognome, Sesso)
- **Ospite**(<ins>CF</ins>, Costo*, IMO_Crociera)
  - Ospite.CF -> Persona.CF
  - IMO_Crociera -> Crociera.IMO
- **Equipaggio**(<ins>CF</ins>, IDequipaggio, Lingue_Parlate*, Stipendio*, Anni_Servizio*, IMO_Crociera*)
  - Equipaggio.CF -> Persona.CF
  - IMO_Crociera -> Crociera.IMO
- **Animatore**(<ins>CF</ins>, Abilità*)
  - Animatore.CF -> Equipaggio.CF
- **Evento**(<ins>Nome</ins>, <ins>Tipologia</ins>, <ins>IMO_Crociera</ins>, Num_Cons_Partecipanti*, Num_Min_Animatori*, Età_Consigliata*)
  - Evento.IMO_Crociera -> Crociera.IMO
- **ORGANIZZA**(<ins>CF_Animatore</ins>, <ins>Nome_Evento</ins>, <ins>Tipologia_Evento</ins>, <ins>IMO_Crociera_Evento</ins>)
  - Organizza.CF_Animatore -> Animatore.CF
  - Organizza.Nome_Evento -> Evento.Nome
  - Organizza.Tipologia_evento -> Evento.Tipologia
  - Organizza.IMO_Crociera_Evento -> Evento.IMO_Crociera

# **5 Implementazione in PostgreSQL e Definizione delle Query**
Il file Crociere.sql contiene il codice SQL necessario per la creazione e il popolamento delle tabelle del database. 
Questo file include inoltre una serie di query per l’estrazione dei dati e 
un indice creato specificamente per migliorare le prestazioni di una di queste interrogazioni.

## **5.1 Definizione delle Query**
Di seguito vengono presentate e descritte le query con i relativi output generati e
viene motivato l’utilizzo dell’indice proposto.

**Query 1** Trovare le crociere che toccano più di un numero di porti diversi indicato dall'utente e indicarne la città di partenza, di arrivo e il numero di tappe. Ordinate in modo decrescente dalla crociera con il maggior numero di tappe. Nel nostro caso abbiamo selezionato 2.
```sql
SELECT C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale, COUNT(DISTINCT T.Città) AS Numero_Tappe
FROM Crociera C
JOIN Tappa T ON C.IMO = T.IMO
GROUP BY C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale
HAVING COUNT(DISTINCT T.Città) > '<NUMERO TAPPE>'
ORDER BY Numero_Tappe DESC;
```
Estratto dell’output:

<img src="img/q1.png" alt="Query1" width="500"/>

(Query1)

**QUERY 2** Visualizzare tutte le crociere in partenza da una citta' inserita dall'utente, ordinate in modo crescente per il numero IMO della crociera. Nella nostra query abbiamo usato la città di Genova.

```sql
SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni
FROM Crociera c
WHERE c.Porto_Partenza = '<PORTO>';
```
Estratto dell’output:

<img src="img/q2.png" alt="Query2" width="400"/>

(Query2)

**Query 3** Trovare le crociere che hanno una media del costo dei biglietti superiore ad un importo indicato. Vengono mostrare in ordine decrescente dalla più costosa fino al valore selezionato dall'utente. Nel nostro esempio abbiamo selezionato 500 euro.
```sql
SELECT O.IMO_Crociera, C.Nome_Nave, AVG(O.Costo) AS Media_Costo
FROM Ospite O
JOIN Crociera C ON O.IMO_Crociera = C.IMO
GROUP BY O.IMO_Crociera, C.Nome_Nave
HAVING AVG(O.Costo) > '<PREZZO>'
ORDER BY Media_Costo DESC;
```
Estratto dell’output:

<img src="img/q3.png" alt="Query3" width="400"/>

(Query3)

**Query 4** Visualizzare il numero di eventi organizzati per crociera e la media degli eventi a cui ogni animatore deve partecipare, ordinati in modo decrescente dalla crociera con la più alta media di eventi che deve fare ogni animatore.
```sql
SELECT 
    C.Nome_Nave,
    C.IMO,
    COUNT(DISTINCT A.CF) AS Num_Animatori,
    ROUND(CAST(COUNT(O.CF_Animatore) AS DECIMAL) / NULLIF(COUNT(DISTINCT A.CF), 0), 2) AS Media_Eventi_Per_Animatore
FROM Crociera C
JOIN Evento E ON C.IMO = E.IMO_Crociera
JOIN ORGANIZZA O ON E.Nome = O.Nome_Evento 
                 AND E.Tipologia = O.Tipologia_Evento 
                 AND E.IMO_Crociera = O.IMO_Crociera_Evento
JOIN Animatore A ON A.CF = O.CF_Animatore
GROUP BY C.Nome_Nave, C.IMO
ORDER BY Media_Eventi_Per_Animatore DESC;
```
Estratto dell’output:

<img src="img/q4.png" alt="Query4" width="450"/>

(Query4)

**Query 5** Trovare, per ogni crociera, la percentuale di occupazione rispetto alla capacità massima (Num_Prenotazioni / Max_Passeggeri) sopra una soglia percentuale specificata dall'utente e visualizzarle in ordine decrescente. Nel nostro caso abbiamo selezionato 0.
```sql
SELECT IMO, Nome_Nave,
    ROUND((Num_Prenotazioni * 100.0) / Max_Passeggeri, 2) AS Percentuale_Occupazione
FROM Crociera
WHERE Max_Passeggeri > 0
  AND (Num_Prenotazioni * 100.0) / Max_Passeggeri > '<PERCENTUALE_MINIMA>'
ORDER BY Percentuale_Occupazione DESC;
```
Estratto dei primi 25 elemementi dell'output:

<img src="img/q5.png" alt="Query5" width="300"/>

**Query 6** Trovare le crociere che hanno un numero minimo di tappe e una media del costo degli ospiti inferiore a un certo importo. Nel nostro esempio abbiamo selezionato 4 porti e 600 euro.
```sql
SELECT C.IMO, C.Nome_Nave, COUNT(DISTINCT T.Città) AS Numero_Tappe, AVG(O.Costo) AS Media_Costo
FROM Crociera C
JOIN Tappa T ON C.IMO = T.IMO
JOIN Ospite O ON C.IMO = O.IMO_Crociera
GROUP BY C.IMO, C.Nome_Nave
HAVING COUNT(DISTINCT T.Città) >= '<NUM_MIN_TAPPE>'
   AND AVG(O.Costo) <= '<MAX_COSTO>'
ORDER BY Numero_Tappe DESC, Media_Costo ASC;
```
Estratto dell’output:

<img src="img/q6.png" alt="Query6" width="400"/>

(Query6)

## **5.2 Creazione degli indici**
Si suppone di voler ottimizzare la Query 2, per la quale occorre considerare:
1) Condizione di Join: p.Città = c.Porto_Partenza
2) Group by sulla colonna p.Città e aggregazione COUNT(c.IMO), AVG(c.Num_Prenotazioni)
3) Ordinamento sul numero di crociere in partenza (alias Numero_Crociere)

Per il punto 1, è opportuno creare un indice sulla colonna Porto_Partenza della tabella Crociera, poiché questa viene utilizzata in un join basato sull’uguaglianza con Porto(Città) e successivamente per il raggruppamento dei risultati. Inoltre, Porto_Partenza non è una chiave primaria e non è automaticamente indicizzata dal sistema.
```sql
CREATE INDEX idx_porto_partenza ON Crociera (Porto_Partenza);
```
Questo indice consente:
1) Un accesso rapido alle tuple della tabella Crociera che partono da una certa città
2) Un’efficiente esecuzione del join con la tabella Porto usando l’equivalenza sulla città,
3) Una scansione ordinata utile all'ottimizzazione del GROUP BY, permettendo di aggregare le tuple con lo stesso valore di **Porto_Partenza** con complessità inferiore.

Per quanto riguarda il punto 3, l’ordinamento viene effettuato su un attributo derivato (**Numero_Crociere, alias di COUNT(c.IMO)**), quindi non può essere direttamente indicizzato. Tuttavia, un ordinamento efficace viene facilitato dalla pre-aggregazione ottimizzata tramite l’indice sul campo di raggruppamento (**Porto_Partenza**).

```plaintext
Nota: la colonna Città in Porto è chiave primaria, quindi PostgreSQL crea automaticamente un indice B+ Tree su di essa. Non è necessario creare un ulteriore indice su Porto(Città) per il join.
```
# **6 Applicazione Software**

Il file **Query.c** implementa un programma in linguaggio C che consente di connettersi a un database PostgreSQL contenente i dati relativi alla gestione delle crociere. La connessione avviene in locale, e per semplificare la portabilità è stata configurata senza password, a condizione che il sistema sia impostato per l’autenticazione locale tramite modalità peer o trust.
Lo scopo principale del programma è eseguire e visualizzare i risultati di diverse query SQL predefinite, come descritto nella Sezione 5 del progetto.
All’avvio, il programma presenta un’interfaccia testuale interattiva con un menu numerato che elenca le interrogazioni disponibili. L’utente può selezionare la query desiderata digitando il numero corrispondente.
Un aspetto centrale del programma è la sua modularità e flessibilità. Le query non sono scritte direttamente nel codice sorgente, ma vengono lette da un file esterno (Crociere.sql) insieme ai rispettivi titoli. Questo consente di modificare, aggiungere o rimuovere query SQL semplicemente aggiornando il file .sql, senza dover ricompilare il programma: il menu si aggiornerà automaticamente.
Il programma supporta anche le query parametriche, in cui i parametri vengono indicati nel file SQL con una sintassi come &lt;PORTO&gt;. Quando l’utente seleziona una query di questo tipo, il programma richiede interattivamente i valori da sostituire per ciascun parametro. Ogni query può contenere fino a un massimo di 25 parametri.
Inoltre, Query.c gestisce anche l’inizializzazione del database: all’avvio viene eseguito uno script SQL che crea e popola automaticamente tutte le tabelle necessarie.
In sintesi, le interrogazioni SQL sono completamente separate dal codice sorgente e gestite esternamente: è quindi possibile modificarle dinamicamente aggiornando il file Crociere.sql, con effetto immediato sul menu delle scelte del programma.

---
**Note:**

Per facilitare la portabilità del programma è stato aggiunto un file config.txt esterno in cui è richiesto di inserire nome utente e password di postgreSQL che verrà utilizzato dal codice c.

Nei computer con sistema operativo linux è necessario linkare il pacchetto di postgresql quindi serve compilare con il comando:
```bash
- gcc -o query Query.c -I/usr/include/postgresql -lpq
```
Nei computer con sistema operativo windows è necessario invece usare il comando: 
```bash
- gcc -o query.exe Query.c -IC:/PostgreSQL/include -LC:/PostgreSQL/lib -lpq
```
Bisogna aggiungere -I e -lpq per indicare al compilatore dove trovare gli header di PostgreSQL e per linkare la libreria client libpq necessaria alle funzioni di connessione e gestione del database.