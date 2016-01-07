\set ECHO none
\i sql/base32_4b.sql
\set ECHO all

CREATE TABLE base32_4bs(id serial primary key, base32_4b base32_4b unique);
INSERT INTO base32_4bs(base32_4b) VALUES('012345');
INSERT INTO base32_4bs(base32_4b) VALUES('6789AB');
INSERT INTO base32_4bs(base32_4b) VALUES('CDEFGH');
INSERT INTO base32_4bs(base32_4b) VALUES('IJKLMN');
INSERT INTO base32_4bs(base32_4b) VALUES('OPQRST');
INSERT INTO base32_4bs(base32_4b) VALUES('UV');
INSERT INTO base32_4bs(base32_4b) VALUES('200004');
INSERT INTO base32_4bs(base32_4b) VALUES('400002');
INSERT INTO base32_4bs(base32_4b) VALUES('300003');
INSERT INTO base32_4bs(base32_4b) VALUES('abctuv');
INSERT INTO base32_4bs(base32_4b) VALUES('0'); -- test limits
INSERT INTO base32_4bs(base32_4b) VALUES('VVVVVV');

SELECT * FROM base32_4bs ORDER BY base32_4b;

SELECT MIN(base32_4b) AS min FROM base32_4bs;
SELECT MAX(base32_4b) AS max FROM base32_4bs;

-- index scan
TRUNCATE base32_4bs;
INSERT INTO base32_4bs(base32_4b) SELECT '44'||id FROM generate_series(5678, 8000) id;

SET enable_seqscan = false;
SELECT id,base32_4b::text FROM base32_4bs WHERE base32_4b = '446000';
SELECT id,base32_4b FROM base32_4bs WHERE base32_4b >= '447000' LIMIT 5;
SELECT count(id) FROM base32_4bs;
SELECT count(id) FROM base32_4bs WHERE base32_4b <> ('446500'::text)::base32_4b;
RESET enable_seqscan;

-- operators and conversions
SELECT '0'::base32_4b < '0'::base32_4b;
SELECT '0'::base32_4b > '0'::base32_4b;
SELECT '0'::base32_4b < '1'::base32_4b;
SELECT '0'::base32_4b > '1'::base32_4b;
SELECT '0'::base32_4b <= '0'::base32_4b;
SELECT '0'::base32_4b >= '0'::base32_4b;
SELECT '0'::base32_4b <= '1'::base32_4b;
SELECT '0'::base32_4b >= '1'::base32_4b;
SELECT '0'::base32_4b <> '0'::base32_4b;
SELECT '0'::base32_4b <> '1'::base32_4b;
SELECT '0'::base32_4b = '0'::base32_4b;
SELECT '0'::base32_4b = '1'::base32_4b;

-- COPY FROM/TO
TRUNCATE base32_4bs;
COPY base32_4bs(base32_4b) FROM STDIN;
00000000
VVVVVV
\.
COPY base32_4bs TO STDOUT;

-- clean up --
DROP TABLE base32_4bs;

-- errors
SELECT ''::base32_4b;
SELECT '1234567'::base32_4b;
SELECT 'Z'::base32_4b;
