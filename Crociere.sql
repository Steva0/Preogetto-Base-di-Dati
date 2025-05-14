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

INSERT INTO Compagnia VALUES
('0123456789', 'MareBlu Cruises', 'Genova, Italia', '+390101234567'),
('9876543210', 'Oceania Travels', 'Napoli, Italia', '+390812345678'),
('1234509876', 'Mediterranea SeaLine', 'Palermo, Italia', '+390916789012');

INSERT INTO Porto VALUES
('Genova', 10),
('Napoli', 8),
('Palermo', 6),
('Barcellona', 12),
('Marsiglia', 7),
('Atene', 9);

INSERT INTO Crociera VALUES
('IMO0000001', 'Nave Azzurra', 50, 500, 400, 'Genova', 'Atene', '2025-07-01 10:00:00', 168, '0123456789'),
('IMO0000002', 'Sole Mediterraneo', 60, 600, 520, 'Napoli', 'Barcellona', '2025-08-05 16:00:00', 120, '9876543210'),
('IMO0000003', 'Stella del Mare', 55, 450, 300, 'Palermo', 'Marsiglia', '2025-09-10 08:00:00', 144, '1234509876');

INSERT INTO Tappa VALUES
('IMO0000001', 'Napoli', '2025-07-02 10:00:00', '2025-07-01 22:00:00'),
('IMO0000001', 'Palermo', '2025-07-03 10:00:00', '2025-07-02 22:00:00'),
('IMO0000001', 'Barcellona', '2025-07-04 10:00:00', '2025-07-03 22:00:00'),
('IMO0000001', 'Marsiglia', '2025-07-05 10:00:00', '2025-07-04 22:00:00'),
('IMO0000002', 'Genova', '2025-08-06 12:00:00', '2025-08-05 20:00:00'),
('IMO0000002', 'Marsiglia', '2025-08-07 14:00:00', '2025-08-06 20:00:00');

INSERT INTO Persona VALUES
('RSSMRA80A01F205X', 'Mario', 'Rossi', 'M'),
('BNCLRA85C60H501Y', 'Laura', 'Bianchi', 'F'),
('VRDLGU90T10L219K', 'Luca', 'Verdi', 'M'),
('PLLMNL95D01A562Z', 'Manuela', 'Paolini', 'F'),
('GRGFNC78M15C351U', 'Francesco', 'Gargiulo', 'M');

INSERT INTO Ospite VALUES
('RSSMRA80A01F205X', 550.00, 'IMO0000001'),
('BNCLRA85C60H501Y', 620.00, 'IMO0000001'),
('VRDLGU90T10L219K', 480.00, 'IMO0000002');

INSERT INTO Equipaggio VALUES
('PLLMNL95D01A562Z', 'EQ001', 'Italiano, Inglese', 2200.00, 5, 'IMO0000001'),
('GRGFNC78M15C351U', 'EQ002', 'Inglese, Francese', 2500.00, 7, 'IMO0000002');

INSERT INTO Animatore VALUES
('PLLMNL95D01A562Z', 'Balli di gruppo, Karaoke'),
('GRGFNC78M15C351U', 'Teatro, Clownerie');

INSERT INTO Evento VALUES
('Festa di Benvenuto', 'Intrattenimento', 'IMO0000001', 150, 2, 18),
('Karaoke Night', 'Musica', 'IMO0000001', 100, 1, 16),
('Spettacolo Teatrale', 'Teatro', 'IMO0000002', 120, 2, 14);

INSERT INTO ORGANIZZA VALUES
('PLLMNL95D01A562Z', 'Festa di Benvenuto', 'Intrattenimento', 'IMO0000001'),
('PLLMNL95D01A562Z', 'Karaoke Night', 'Musica', 'IMO0000001'),
('GRGFNC78M15C351U', 'Spettacolo Teatrale', 'Teatro', 'IMO0000002');



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
