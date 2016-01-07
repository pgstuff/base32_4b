/*
 * Author: The maintainer's name
 * Created at: Wed Oct 14 23:12:59 -0400 2015
 *
 */

SET client_min_messages = warning;

-- SQL definitions
CREATE TYPE base32_4b;

-- basic i/o functions
CREATE OR REPLACE FUNCTION base32_4b_in(cstring) RETURNS base32_4b AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_out(base32_4b) RETURNS cstring AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_send(base32_4b) RETURNS bytea AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_recv(internal) RETURNS base32_4b AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE base32_4b (
	input = base32_4b_in,
	output = base32_4b_out,
	send = base32_4b_send,
	receive = base32_4b_recv,
	internallength = 4,
	passedbyvalue
);

-- functions to support btree opclass
CREATE OR REPLACE FUNCTION base32_4b_lt(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_le(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_eq(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_ne(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_ge(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_gt(base32_4b, base32_4b) RETURNS bool AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base32_4b_cmp(base32_4b, base32_4b) RETURNS int4 AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
CREATE OR REPLACE FUNCTION base32_4b_to_text(base32_4b) RETURNS text AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION text_to_base32_4b(text) RETURNS base32_4b AS '$libdir/base32_4b'
LANGUAGE C IMMUTABLE STRICT;

-- operators
CREATE OPERATOR < (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_lt,
	commutator = >, negator = >=,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_le,
	commutator = >=, negator = >,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_eq,
	commutator = =, negator = <>,
	restrict = eqsel, join = eqjoinsel,
	merges
);
CREATE OPERATOR <> (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_ne,
	commutator = <>, negator = =,
	restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR > (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_gt,
	commutator = <, negator = <=,
	restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
	leftarg = base32_4b, rightarg = base32_4b, procedure = base32_4b_ge,
	commutator = <=, negator = <,
	restrict = scalargtsel, join = scalargtjoinsel
);

-- aggregates
CREATE OR REPLACE FUNCTION base32_4b_smaller(base32_4b, base32_4b)
RETURNS base32_4b
AS '$libdir/base32_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION base32_4b_larger(base32_4b, base32_4b)
RETURNS base32_4b
AS '$libdir/base32_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE min(base32_4b)  (
    SFUNC = base32_4b_smaller,
    STYPE = base32_4b,
    SORTOP = <
);

CREATE AGGREGATE max(base32_4b)  (
    SFUNC = base32_4b_larger,
    STYPE = base32_4b,
    SORTOP = >
);

-- btree operator class
CREATE OPERATOR CLASS base32_4b_ops DEFAULT FOR TYPE base32_4b USING btree AS
	OPERATOR 1 <,
	OPERATOR 2 <=,
	OPERATOR 3 =,
	OPERATOR 4 >=,
	OPERATOR 5 >,
	FUNCTION 1 base32_4b_cmp(base32_4b, base32_4b);
-- cast from/to text
CREATE CAST (base32_4b AS text) WITH FUNCTION base32_4b_to_text(base32_4b) AS ASSIGNMENT;
CREATE CAST (text AS base32_4b) WITH FUNCTION text_to_base32_4b(text) AS ASSIGNMENT;
