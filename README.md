# Base di Dati di un Sistema di Gestione Crociere

Membri del gruppo:
- Ghiraldin Mirco 2102505
- Stevanin Michele 2101741

# Abstract
Questo lavoro sviluppa una base di dati progettata per gestire in modo strutturato e
coerente le informazioni relative alle crociere, ai loro passeggeri, allo staff e
agli eventi organizzati. L’obiettivo principale è fornire un sistema che permetta agli organizzatori delle crociere 
di gestire in maniera ottimale l'organizzazioni dei viaggi, garantendo un accesso organizzato ai dati e facilitando la loro analisi.
Nel constesto delle crociere il database distingue tra le varie `compagnie` che dispongono di `navi` da crociera tenendo traccia delle varie `tappe`(città), data di partenza e di arrivo, monitorando le varie `persone` partecipanto ad ogni viaggio, le quali a loro volta si possono distinguere tra membri dell'`equipaggio` e `passeggeri`. Equipaggio di cui fanno parte anche `Capitano` e gli `animatori`. Il sistema tiene memoria degli `eventi` organizzati durante il ogni crociera con i suoi animatori, il numero di partecipanti e l'orario di inizio e fine.
Sono presenti inoltre le `classi` suddivise per fasce di prezzo. 

# Analisi dei Requisiti
Questa sezione riassume i requisiti a cui deve sottostare la base di dati.

**Crociera**. Ogni crociera è identificato dal prioprio codice `IMO` (International Maritime Organization) e le seguenti informazioni:
    - **Imo** che distingue in modo univoco ogni crociera
    - **Nome nave**
    - **Referente compagnia**
    - **Capitano**
    - **Numero minimo membri equipaggio**
    - **Città di Partenza**
    - **Città di Fine**
    - **Data e orario di partenza**
    - **Durata in giorni**
    - **Numero di passeggeri max**
    - **Età barca**
    - **Ultima ristutturazione**
    - **Categoria crociera** transatlantica, mediterranea, fluviale.

**Porti**. Ogni porto è identificato dalla sua città. 
    - **Nome città**

**Tappa**. Ogni tappa è caratterizzata da data e orario di arrivo e partenza.
    - **Data e ora di arrivo**
    - **Data e ora di partenza**

**Classi**. Ogni classe è caratterizzata dal suo livello e dai vantiggi di quella classe.
    - **Livello classe** economy, business...
    - **Vantaggi**
    
**Cabine**. Ogni cabina è caratterizzata da un costo e dal numero di posti dispobibili.
    - **Costo crociera**
    - **Numero posti**

**Persone**. CF
    - **CF**
    - **Nome**
    - **Cognome**
    - **Sesso**

Le persone possono far parte dell'`equipaggio`.

**Equipaggio**. Ogni equipaggio ha il proprio stipendio, numero identificativo, anni di servizio, lingue parlate.
    - **Lingue parlate**
    - **Stipendio**
    - **ID**
    - **Anni di servizio**

Un mebro dell'equipaggio può essere `animatore`.

**Animatore**. Ogni animatore ha una o più abilità.
    - **Abilità**

**Eventi**. Ogni evento è caratterizzato dal nome dell'evento, tipo di evento, data e ora, luogo, età consigliata, durata, numero di animatori minimo, numero massimo partecipanti.
    - **Nome evento**
    - **Tipo evento**
    - **Data e ora**
    - **Luogo**
    - **Età consigliata**
    - **Durata**
    - **Numero minimo animatori**
    - **Numero massimo partecipanti**