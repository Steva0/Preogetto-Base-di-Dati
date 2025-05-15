CREATE TABLE IF NOT EXISTS Compagnia (
    PI CHAR(10) PRIMARY KEY,
    Nome VARCHAR(100) NOT NULL,
    Sede VARCHAR(200) NOT NULL,
    Recapito_Telefonico VARCHAR(15)
);

CREATE TABLE IF NOT EXISTS Porto (
    Città VARCHAR(100) PRIMARY KEY,
    Numero_Massimo_Navi INT
);

CREATE TABLE IF NOT EXISTS Crociera (
    IMO CHAR(10) PRIMARY KEY,
    Nome_Nave VARCHAR(100) NOT NULL,
    Min_Equipaggio INT NOT NULL,
    Max_Passeggeri INT NOT NULL,
    Num_Prenotazioni INT NOT NULL,
    Porto_Partenza VARCHAR(100),
    Porto_Finale VARCHAR(100),
    Data_Ora_Partenza TIMESTAMP,
    Durata INT,
    PI_Compagnia CHAR(10),
    FOREIGN KEY (PI_Compagnia) REFERENCES Compagnia(PI),
    FOREIGN KEY (Porto_Partenza) REFERENCES Porto(Città),
    FOREIGN KEY (Porto_Finale) REFERENCES Porto(Città)
);

CREATE TABLE IF NOT EXISTS Tappa (
    IMO CHAR(10),
    Città VARCHAR(100),
    Data_Ora_Partenza TIMESTAMP,
    Data_Ora_Arrivo TIMESTAMP NOT NULL,
    PRIMARY KEY (IMO, Città, Data_Ora_Partenza),
    FOREIGN KEY (IMO) REFERENCES Crociera(IMO),
    FOREIGN KEY (Città) REFERENCES Porto(Città)
);

CREATE TABLE IF NOT EXISTS Persona (
    CF CHAR(16) PRIMARY KEY,
    Nome VARCHAR(100) NOT NULL,
    Cognome VARCHAR(100) NOT NULL,
    Sesso CHAR(1) NOT NULL
);

CREATE TABLE IF NOT EXISTS Ospite (
    CF CHAR(16),
    Costo DECIMAL(10, 2),
    IMO_Crociera CHAR(10),
    PRIMARY KEY (CF, IMO_Crociera),
    FOREIGN KEY (CF) REFERENCES Persona(CF),
    FOREIGN KEY (IMO_Crociera) REFERENCES Crociera(IMO)
);

CREATE TABLE IF NOT EXISTS Equipaggio (
    CF CHAR(16) UNIQUE,
    IDequipaggio CHAR(10) PRIMARY KEY,
    Lingue_Parlate VARCHAR(100),
    Stipendio DECIMAL(10, 2),
    Anni_Servizio INT,
    IMO_Crociera CHAR(10),
    FOREIGN KEY (CF) REFERENCES Persona(CF)
);

CREATE TABLE IF NOT EXISTS Animatore (
    CF CHAR(16),
    Abilità VARCHAR(100),
    PRIMARY KEY (CF),
    FOREIGN KEY (CF) REFERENCES Equipaggio(CF)
);

CREATE TABLE IF NOT EXISTS Evento (
    Nome VARCHAR(100),
    Tipologia VARCHAR(100),
    IMO_Crociera CHAR(10),
    Num_Cons_Partecipanti INT,
    Num_Min_Animatori INT,
    Età_Consigliata INT,
    PRIMARY KEY (Nome, Tipologia, IMO_Crociera),
    FOREIGN KEY (IMO_Crociera) REFERENCES Crociera(IMO)
);

CREATE TABLE IF NOT EXISTS ORGANIZZA (
    CF_Animatore CHAR(16),
    Nome_Evento VARCHAR(100),
    Tipologia_Evento VARCHAR(100),
    IMO_Crociera_Evento CHAR(10),
    PRIMARY KEY (CF_Animatore, Nome_Evento, Tipologia_Evento, IMO_Crociera_Evento),
    FOREIGN KEY (CF_Animatore) REFERENCES Animatore(CF),
    FOREIGN KEY (Nome_Evento, Tipologia_Evento, IMO_Crociera_Evento) REFERENCES Evento(Nome, Tipologia, IMO_Crociera)
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

CREATE INDEX idx_crociera_porto_partenza ON Crociera(Porto_Partenza);

-- Query 1 - Seleziona tutte le crociere con piu' di 3 tappe
SELECT C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale, COUNT(DISTINCT T.Città) AS Numero_Tappe
FROM Crociera C
JOIN Tappa T ON C.IMO = T.IMO
GROUP BY C.IMO, C.Nome_Nave, C.Porto_Partenza, C.Porto_Finale
HAVING COUNT(DISTINCT T.Città) > 3
ORDER BY Numero_Tappe DESC;

-- Query 2 – Visualizzare tutte le crociere in partenza da una città inserita dall’utente
SELECT c.IMO, c.Nome_Nave, c.Num_Prenotazioni
FROM Crociera c
WHERE c.Porto_Partenza = '<CITTÀ>';

-- Query 3 - Seleziona tutte le crociere con un costo medio superiore a 500
SELECT O.IMO_Crociera, C.Nome_Nave, AVG(O.Costo) AS Media_Costo
FROM Ospite O
JOIN Crociera C ON O.IMO_Crociera = C.IMO
GROUP BY O.IMO_Crociera, C.Nome_Nave
HAVING AVG(O.Costo) > 500
ORDER BY Media_Costo DESC;

-- Query 4 - Seleziona tutti gli animatori che hanno organizzato piu' di 2 eventi
SELECT A.CF, P.Nome, P.Cognome, COUNT(*) AS Num_Eventi
FROM Animatore A
JOIN Persona P ON A.CF = P.CF
JOIN ORGANIZZA O ON A.CF = O.CF_Animatore
GROUP BY A.CF, P.Nome, P.Cognome
HAVING COUNT(*) > 2
ORDER BY Num_Eventi DESC;

-- Query 5 - Seleziona tutte le crociere con un numero di prenotazioni superiore alla media
SELECT IMO, Nome_Nave,
       ROUND((Num_Prenotazioni * 100.0) / Max_Passeggeri, 2) AS Percentuale_Occupazione
FROM Crociera
WHERE Max_Passeggeri > 0
ORDER BY Percentuale_Occupazione DESC;


