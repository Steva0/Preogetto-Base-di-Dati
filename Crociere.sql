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
    Min_Equipaggio INT,                    -- Numero minimo di equipaggio
    Max_Passeggeri INT,                    -- Numero massimo di passeggeri
    Num_Prenotazioni INT,                  -- Numero di prenotazioni attive
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
    Data_Ora_Arrivo DATETIME,              -- Data e ora di arrivo della tappa
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
    IDequipaggio INT PRIMARY KEY,           -- ID univoco per l'equipaggio
    Lingue_Parlate VARCHAR(100),           -- Lingue parlate
    Stipendio DECIMAL(10, 2),               -- Stipendio dell'equipaggio
    Anni_Servizio INT,                     -- Anni di servizio
    IMO_Crociera CHAR(10),                  -- IMO della crociera
    FOREIGN KEY (CF) REFERENCES Persona(CF),
    FOREIGN KEY (IMO_Crociera) REFERENCES Crociera(IMO)
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
