-- Tabella Compagnia
CREATE TABLE Compagnia (
    PI CHAR(10) PRIMARY KEY,               -- Codice fiscale della compagnia
    Nome VARCHAR(100) NOT NULL,            -- Nome della compagnia
    Sede VARCHAR(200) NOT NULL,            -- Sede della compagnia
    Recapito_Telefonico VARCHAR(15)        -- Recapito telefonico
);

-- Tabella Porto
CREATE TABLE Porto (
    Città VARCHAR(100) PRIMARY KEY,        -- Nome della città del porto
    Numero_Massimo_Navi INT               -- Numero massimo di navi che possono ormeggiare
);

-- Tabella Crociera
CREATE TABLE Crociera (
    IMO CHAR(10) PRIMARY KEY,              -- IMO (International Maritime Organization number) della crociera
    Nome_Nave VARCHAR(100) NOT NULL,       -- Nome della nave
    Min_Equipaggio INT NOT NULL,                    -- Numero minimo di equipaggio
    Max_Passeggeri INT NOT NULL,                    -- Numero massimo di passeggeri
    Num_Prenotazioni INT NOT NULL,                  -- Numero di prenotazioni attive
    Porto_Partenza VARCHAR(100),          -- Porto di partenza
    Porto_Finale VARCHAR(100),            -- Porto finale
    Data_Ora_Partenza DATETIME,           -- Data e ora di partenza
    Durata INT,                            -- Durata della crociera (in ore o giorni)
    PI_Compagnia CHAR(10),                 -- PI della compagnia che gestisce la crociera
    FOREIGN KEY (PI_Compagnia) REFERENCES Compagnia(PI),
    FOREIGN KEY (Porto_Partenza) REFERENCES Porto(Città),
    FOREIGN KEY (Porto_Finale) REFERENCES Porto(Città)
);

-- Tabella Tappa
CREATE TABLE Tappa (
    IMO CHAR(10),                          -- IMO della crociera
    Città VARCHAR(100),                    -- Città della tappa
    Data_Ora_Partenza DATETIME,            -- Data e ora di partenza della tappa
    Data_Ora_Arrivo DATETIME NOT NULL,              -- Data e ora di arrivo della tappa
    PRIMARY KEY (IMO, Città, Data_Ora_Partenza), -- Chiave primaria composta
    FOREIGN KEY (IMO) REFERENCES Crociera(IMO),
    FOREIGN KEY (Città) REFERENCES Porto(Città)
);

-- Tabella Persona
CREATE TABLE Persona (
    CF CHAR(16) PRIMARY KEY,               -- Codice fiscale della persona
    Nome VARCHAR(100) NOT NULL,            -- Nome della persona
    Cognome VARCHAR(100) NOT NULL,         -- Cognome della persona
    Sesso CHAR(1) NOT NULL                 -- Sesso della persona
);

-- Tabella Ospite
CREATE TABLE Ospite (
    CF CHAR(16),                           -- Codice fiscale dell'ospite
    Costo DECIMAL(10, 2),                   -- Costo del biglietto
    IMO_Crociera CHAR(10),                  -- IMO della crociera
    PRIMARY KEY (CF, IMO_Crociera),        -- Chiave primaria composta
    FOREIGN KEY (CF) REFERENCES Persona(CF),
    FOREIGN KEY (IMO_Crociera) REFERENCES Crociera(IMO)
);

-- Tabella Equipaggio
CREATE TABLE Equipaggio (
    CF CHAR(16),                           -- Codice fiscale dell'equipaggio
    IDequipaggio CHAR(10) PRIMARY KEY,     -- ID univoco per l'equipaggio
    Lingue_Parlate VARCHAR(100),           -- Lingue parlate
    Stipendio DECIMAL(10, 2),               -- Stipendio dell'equipaggio
    Anni_Servizio INT,                     -- Anni di servizio
    IMO_Crociera CHAR(10),                  -- IMO della crociera
    FOREIGN KEY (CF) REFERENCES Persona(CF)
);

-- Tabella Animatore
CREATE TABLE Animatore (
    CF CHAR(16),                           -- Codice fiscale dell'animatore
    Abilità VARCHAR(100),                  -- Abilità dell'animatore
    PRIMARY KEY (CF),
    FOREIGN KEY (CF) REFERENCES Equipaggio(CF)
);

-- Tabella Evento
CREATE TABLE Evento (
    Nome VARCHAR(100),                     -- Nome dell'evento
    Tipologia VARCHAR(100),                -- Tipologia dell'evento
    IMO_Crociera CHAR(10),                  -- IMO della crociera
    Num_Cons_Partecipanti INT,             -- Numero consigliato di partecipanti
    Num_Min_Animatori INT,                 -- Numero minimo di animatori
    Età_Consigliata INT,                   -- Età consigliata per partecipare
    PRIMARY KEY (Nome, Tipologia, IMO_Crociera), -- Chiave primaria composta
    FOREIGN KEY (IMO_Crociera) REFERENCES Crociera(IMO)
);

-- Tabella ORGANIZZA
CREATE TABLE ORGANIZZA (
    CF_Animatore CHAR(16),                 -- Codice fiscale dell'animatore
    Nome_Evento VARCHAR(100),              -- Nome dell'evento
    Tipologia_Evento VARCHAR(100),         -- Tipologia dell'evento
    IMO_Crociera_Evento CHAR(10),          -- IMO della crociera
    PRIMARY KEY (CF_Animatore, Nome_Evento, Tipologia_Evento, IMO_Crociera_Evento), -- Chiave primaria composta
    FOREIGN KEY (CF_Animatore) REFERENCES Animatore(CF),
    FOREIGN KEY (Nome_Evento) REFERENCES Evento(Nome),
    FOREIGN KEY (Tipologia_Evento) REFERENCES Evento(Tipologia),
    FOREIGN KEY (IMO_Crociera_Evento) REFERENCES Evento(IMO_Crociera)
);

-- Query 1 – Trovare le crociere che toccano più di 3 porti diversi e indicarne la città di partenza, di arrivo e il numero di tappe
SELECT C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale, COUNT(DISTINCT T.Città) AS Numero_Tappe
FROM Crociera C
JOIN Tappa T ON C.IMO = T.IMO
GROUP BY C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale
HAVING COUNT(DISTINCT T.Città) > 3
ORDER BY Numero_Tappe DESC;

-- QUERY 2 – Visualizzare tutte le crociere in partenza da una città inserita dall’utente
SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni
FROM Crociera c
WHERE c.Porto_Partenza = '<CITTÀ>';

-- Query 3 – Trovare le crociere che hanno una media del costo dei biglietti superiore a 500 euro
SELECT O.IMO_Crociera, C.Nome_Nave, AVG(O.Costo) AS Media_Costo
FROM Ospite O
JOIN Crociera C ON O.IMO_Crociera = C.IMO
GROUP BY O.IMO_Crociera, C.Nome_Nave
HAVING AVG(O.Costo) > 500
ORDER BY Media_Costo DESC;

-- Query 4 – Visualizzare gli animatori con più eventi organizzati, mostrando solo quelli con più di 2 eventi
SELECT A.CF, P.Nome, P.Cognome, COUNT(*) AS Num_Eventi
FROM Animatore A
JOIN Persona P ON A.CF = P.CF
JOIN ORGANIZZA O ON A.CF = O.CF_Animatore
GROUP BY A.CF, P.Nome, P.Cognome
HAVING COUNT(*) > 2
ORDER BY Num_Eventi DESC;

-- Query 5 – Trovare, per ogni crociera, la percentuale di occupazione rispetto alla capacità massima (Num_Prenotazioni / Max_Passeggeri)
SELECT IMO, Nome_Nave,
       ROUND((Num_Prenotazioni * 100.0) / Max_Passeggeri, 2) AS Percentuale_Occupazione
FROM Crociera
WHERE Max_Passeggeri > 0
ORDER BY Percentuale_Occupazione DESC;

-- INDICE PER QUERY 2
CREATE INDEX idx_crociera_porto_partenza ON Crociera(Porto_Partenza);
-- La query accede frequentemente alla tabella Crociera filtrando per Porto_Partenza (colonna che rappresenta una foreign key verso Porto(Città)), per poi eseguire un join con Porto e una successiva aggregazione.
-- L’indice creato su Crociera(Porto_Partenza) consente una ricerca più veloce di tutte le crociere con partenza da un certo porto, 
-- un’efficiente costruzione del join tra Crociera e Porto e una rapida scansione per il GROUP BY che raggruppa proprio per città.
