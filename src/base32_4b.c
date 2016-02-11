#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef uint32 base32_4b_t;

//                              23456789ABCDEFGHJKLMNPQRSTUVWXYZ
static char base32_chars[32] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
static char char_to_num_b32[] = {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,1,2,3,4,5,6,7,8,9,127,127,127,127,127,127,127,10,11,12,13,14,
    15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,127,127,127,127,127,127,127,127,127,127,10,11,12,13,14,15,16,17,18,19,20,21,
    22,23,24,25,26,27,28,29,30,31,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

#define BASE324BGetDatum(x)	 UInt32GetDatum(x)
#define DatumGetBASE324B(x)	 DatumGetUInt32(x)
#define PG_GETARG_BASE324B(n) DatumGetBASE324B(PG_GETARG_DATUM(n))
#define PG_RETURN_BASE324B(x) return BASE324BGetDatum(x)

Datum base32_4b_in(PG_FUNCTION_ARGS);
Datum base32_4b_out(PG_FUNCTION_ARGS);
Datum base32_4b_to_text(PG_FUNCTION_ARGS);
Datum base32_4b_from_text(PG_FUNCTION_ARGS);
Datum base32_4b_send(PG_FUNCTION_ARGS);
Datum base32_4b_recv(PG_FUNCTION_ARGS);
Datum base32_4b_lt(PG_FUNCTION_ARGS);
Datum base32_4b_le(PG_FUNCTION_ARGS);
Datum base32_4b_eq(PG_FUNCTION_ARGS);
Datum base32_4b_ne(PG_FUNCTION_ARGS);
Datum base32_4b_ge(PG_FUNCTION_ARGS);
Datum base32_4b_gt(PG_FUNCTION_ARGS);
Datum base32_4b_cmp(PG_FUNCTION_ARGS);

static base32_4b_t cstring_to_base32_4b(char *base32_4b_string);
static char *base32_4b_to_cstring(base32_4b_t base32_4b);



/* generic input/output functions */
PG_FUNCTION_INFO_V1(base32_4b_in);
Datum
base32_4b_in(PG_FUNCTION_ARGS)
{
	base32_4b_t	result;

	char   *base32_4b_str = PG_GETARG_CSTRING(0);
	result = cstring_to_base32_4b(base32_4b_str);

	PG_RETURN_BASE324B(result);
}

PG_FUNCTION_INFO_V1(base32_4b_out);
Datum
base32_4b_out(PG_FUNCTION_ARGS)
{
	base32_4b_t	packed_base32_4b;
	char   *base32_4b_string;

	packed_base32_4b = PG_GETARG_BASE324B(0);
	base32_4b_string = base32_4b_to_cstring(packed_base32_4b);

	PG_RETURN_CSTRING(base32_4b_string);
}

/* type to/from text conversion routines */
PG_FUNCTION_INFO_V1(base32_4b_to_text);
Datum
base32_4b_to_text(PG_FUNCTION_ARGS)
{
	char	*base32_4b_string;
	text	*base32_4b_text;

	base32_4b_t	packed_base32_4b =  PG_GETARG_BASE324B(0);

	base32_4b_string = base32_4b_to_cstring(packed_base32_4b);
	base32_4b_text = DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(base32_4b_string)));

	PG_RETURN_TEXT_P(base32_4b_text);
}

PG_FUNCTION_INFO_V1(base32_4b_from_text);
Datum
base32_4b_from_text(PG_FUNCTION_ARGS)
{
	text  *base32_4b_text = PG_GETARG_TEXT_P(0);
	char  *base32_4b_str = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(base32_4b_text)));
	base32_4b_t base32_4b = cstring_to_base32_4b(base32_4b_str);

	PG_RETURN_BASE324B(base32_4b);
}

/* Functions to convert to/from bytea */
PG_FUNCTION_INFO_V1(base32_4b_send);
Datum
base32_4b_send(PG_FUNCTION_ARGS)
{
	StringInfoData buffer;
	base32_4b_t base32_4b = PG_GETARG_BASE324B(0);

	pq_begintypsend(&buffer);
	pq_sendint(&buffer, (uint32)base32_4b, 4);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

PG_FUNCTION_INFO_V1(base32_4b_recv);
Datum base32_4b_recv(PG_FUNCTION_ARGS)
{
	base32_4b_t	base32_4b;
	StringInfo	buffer = (StringInfo) PG_GETARG_POINTER(0);

	base32_4b = pq_getmsgint(buffer, 4);
	PG_RETURN_BASE324B(base32_4b);
}

/* functions to support btree opclass */
PG_FUNCTION_INFO_V1(base32_4b_lt);
Datum
base32_4b_lt(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL(a < b);
}

PG_FUNCTION_INFO_V1(base32_4b_le);
Datum
base32_4b_le(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL (a <= b);
}

PG_FUNCTION_INFO_V1(base32_4b_eq);
Datum
base32_4b_eq(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL(a == b);
}

PG_FUNCTION_INFO_V1(base32_4b_ne);
Datum
base32_4b_ne(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL(a != b);
}

PG_FUNCTION_INFO_V1(base32_4b_ge);
Datum
base32_4b_ge(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL(a >= b);
}

PG_FUNCTION_INFO_V1(base32_4b_gt);
Datum
base32_4b_gt(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);
	PG_RETURN_BOOL(a > b);
}

static int32
base32_4b_cmp_internal(base32_4b_t a, base32_4b_t b)
{
    // This signed operation works because only 30 of 32 bits are used.
    return a - b;

    /*if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;*/
}

PG_FUNCTION_INFO_V1(base32_4b_cmp);
Datum
base32_4b_cmp(PG_FUNCTION_ARGS)
{
	base32_4b_t a = PG_GETARG_BASE324B(0);
	base32_4b_t b = PG_GETARG_BASE324B(1);

	PG_RETURN_INT32(base32_4b_cmp_internal(a, b));
}

/*****************************************************************************
 * Aggregate functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(base32_4b_smaller);

Datum
base32_4b_smaller(PG_FUNCTION_ARGS)
{
   base32_4b_t left  = PG_GETARG_BASE324B(0);
   base32_4b_t right = PG_GETARG_BASE324B(1);
   int32 result;

   result = base32_4b_cmp_internal(left, right) < 0 ? left : right;
   PG_RETURN_BASE324B(result);
}

PG_FUNCTION_INFO_V1(base32_4b_larger);

Datum
base32_4b_larger(PG_FUNCTION_ARGS)
{
   base32_4b_t left  = PG_GETARG_BASE324B(0);
   base32_4b_t right = PG_GETARG_BASE324B(1);
   int32 result;

   result = base32_4b_cmp_internal(left, right) > 0 ? left : right;
   PG_RETURN_BASE324B(result);
}


/*
 * Convert a cstring to a base32_4b.
 */
static base32_4b_t
cstring_to_base32_4b(char *base32_4b_str)
{
    char                *ptr;
    unsigned long int   total = 0;
    unsigned int        digit_value;

    ptr = base32_4b_str;

    if (*ptr == 0)
        ereport(ERROR,
            (errmsg("base32_4b number \"%s\" must be 1 digits or larger.", base32_4b_str)));

    for (; ; ptr += 1) {
        if (*ptr == 0)
            PG_RETURN_BASE324B(total);

        digit_value = char_to_num_b32[(unsigned)*ptr];

        if (digit_value == 127)
            ereport(ERROR,
                (errmsg("base32_4b number \"%s\" must only contain digits 0 to 9 and A to V.", base32_4b_str)));

        //total *= 32;
        //total += digit_value;
        total = (total << 5) | digit_value;

        if (total > 1073741823)
            ereport(ERROR,
                (errmsg("base32_4b number \"%s\" must be 6 digits or smaller.", base32_4b_str)));
    }

    PG_RETURN_BASE324B(total);
}

/* Convert the internal representation to output string */
static char *
base32_4b_to_cstring(base32_4b_t base32_4b)
{
    char buffer[9];
    unsigned int offset = sizeof(buffer);
    long unsigned int remainder = base32_4b;
    char   *base32_4b_str;

    buffer[--offset] = '\0';
    do {
        buffer[--offset] = base32_chars[remainder & 31];
        remainder = remainder >> 5;
    } while (remainder > 0);

    base32_4b_str = palloc(sizeof(buffer) - offset);
    memcpy(base32_4b_str, &buffer[offset], sizeof(buffer) - offset);
    return base32_4b_str;
}
