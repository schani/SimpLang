#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pools.h"

typedef struct {
	pool_t pool;
	FILE *file;
	int lookahead;
} context_t;

typedef enum {
	TOKEN_EOF,

	TOKEN_INTEGER,
	TOKEN_IDENT,

	TOKEN_LET,
	TOKEN_AND,
	TOKEN_IN,
	TOKEN_IF,
	TOKEN_THEN,
	TOKEN_ELSE,
	TOKEN_RECUR,
	TOKEN_LOOP,
	TOKEN_END,

	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_NOT,
	TOKEN_NEGATE,
	TOKEN_LESS,
	TOKEN_PLUS,
	TOKEN_TIMES,

	TOKEN_LOGIC_AND,
	TOKEN_LOGIC_OR,

	TOKEN_EQUALS,
	TOKEN_ASSIGN,

	TOKEN_FIRST_KEYWORD = TOKEN_LET,
	TOKEN_LAST_KEYWORD = TOKEN_END,

	TOKEN_FIRST_OPERATOR = TOKEN_OPEN_PAREN,
	TOKEN_LAST_OPERATOR = TOKEN_ASSIGN,

	TOKEN_FIRST_SINGLE_LETTER_OPERATOR = TOKEN_OPEN_PAREN,
	TOKEN_LAST_SINGLE_LETTER_OPERATOR = TOKEN_TIMES,

	TOKEN_FIRST_BINARY_OPERATOR = TOKEN_LESS,
	TOKEN_LAST_BINARY_OPERATOR = TOKEN_EQUALS
} token_type_t;

typedef struct {
	token_type_t type;
	union {
		int64_t i;
		char *name;
	} v;
} token_t;

bool scan_init (context_t *ctx, const char *filename);

token_t scan (context_t *ctx);

static inline bool
token_type_is_keyword (token_type_t t)
{
	return t >= TOKEN_FIRST_KEYWORD && t <= TOKEN_LAST_KEYWORD;
}

static inline bool
token_type_is_operator (token_type_t t)
{
	return t >= TOKEN_FIRST_OPERATOR && t <= TOKEN_LAST_OPERATOR;
}

const char* token_type_keyword_name (token_type_t t);
const char* token_type_operator_name (token_type_t t);

static inline void
error_assert (bool assertion, const char *error)
{
	if (assertion)
		return;
	fprintf(stderr, "Error: %s\n", error);
	exit(1);
}

typedef enum {
	EXPR_INTEGER,
	EXPR_IDENT,
	EXPR_IF,
	EXPR_LET,
	EXPR_LOOP,
	EXPR_RECUR,
	EXPR_UNARY,
	EXPR_BINARY
} expr_type_t;

typedef struct _expr_t expr_t;

typedef struct {
	char *name;
	expr_t *expr;
} binding_t;

struct _expr_t {
	expr_type_t type;
	union {
		int64_t i;
		char *ident;
		struct {
			expr_t *condition;
			expr_t *consequent;
			expr_t *alternative;
		} if_expr;
		struct {
			int n;
			binding_t *bindings;
			expr_t *body;
		} let_loop;
		struct {
			int n;
			expr_t **args;
		} recur;
		struct {
			token_type_t op;
			expr_t *operand;
		} unary;
		struct {
			token_type_t op;
			expr_t *left;
			expr_t *right;
		} binary;
	} v;
};

typedef struct _environment_t
{
	char *name;
	int64_t value;
	struct _environment_t *next;
} environment_t;

void parser_init (context_t *ctx);

expr_t* parse_expr (context_t *ctx);

int64_t eval_expr (environment_t *env, expr_t *expr);

#endif
