#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "dynstring.h"
#include "compiler.h"

static const char *keyword_names[] = { "let", "and", "in", "if", "then", "else", "recur", "loop", "end", NULL };
static const char *single_letter_operators = "()!-<+*";

static token_type_t
find_keyword (const char *name)
{
	int i;
	for (i = 0; keyword_names[i]; i++) {
		if (strcmp(name, keyword_names[i]) == 0)
			return TOKEN_FIRST_KEYWORD + i;
	}
	return TOKEN_EOF;
}

const char*
token_type_keyword_name (token_type_t t)
{
	assert(token_type_is_keyword(t));
	return keyword_names[t - TOKEN_FIRST_KEYWORD];
}

const char*
token_type_operator_name (token_type_t t)
{
	static const char *names[] = { "(", ")", "!", "-", "<", "+", "*", "&&", "||", "==", "=" };

	assert(token_type_is_operator(t));
	return names[t - TOKEN_FIRST_OPERATOR];
}

static int
lookahead (context_t *ctx)
{
	return ctx->lookahead;
}

static bool
consume (context_t *ctx)
{
	ctx->lookahead = fgetc(ctx->file);
	return ctx->lookahead != EOF;
}

bool
scan_init (context_t *ctx, const char *filename)
{
	bool ok;
	ctx->file = fopen(filename, "r");
	assert(ctx->file);
	ok = consume(ctx);
	assert(ok);
	return true;
}

token_t
scan (context_t *ctx)
{
	if (lookahead(ctx) == EOF) {
		token_t t = { TOKEN_EOF };
		return t;
	}
	while (isspace(lookahead(ctx))) {
		if (!consume(ctx)) {
			token_t t = { TOKEN_EOF };
			return t;
		}
	}

	int c = lookahead(ctx);
	if (isalpha(c) || c == '_') {
		dynstring_t ds = ds_new(&ctx->pool);
		ds_append_char(&ds, c);
		while (consume(ctx) && (isalnum(lookahead(ctx)) || lookahead(ctx) == '_'))
			ds_append_char(&ds, lookahead(ctx));
		token_type_t tt = find_keyword(ds.data);
		if (tt == TOKEN_EOF) {
			token_t t = { TOKEN_IDENT };
			t.v.name = ds.data;
			return t;
		} else {
			token_t t = { tt };
			return t;
		}
	}
	if (isdigit(c)) {
		dynstring_t ds = ds_new(&ctx->pool);
		ds_append_char(&ds, c);
		while (consume(ctx) && isdigit(lookahead(ctx)))
			ds_append_char(&ds, lookahead(ctx));
		token_t t = { TOKEN_INTEGER };
		t.v.i = atol(ds.data);
		return t;
	}
	if (strchr(single_letter_operators, c)) {
		token_t t = { TOKEN_FIRST_SINGLE_LETTER_OPERATOR + (strchr(single_letter_operators, c) - single_letter_operators) };
		consume(ctx);
		return t;
	}
	if (c == '&' || c == '|') {
		consume(ctx);
		error_assert(lookahead(ctx) == c, "invalid token");
		consume(ctx);
		token_t t = { c == '&' ? TOKEN_LOGIC_AND : TOKEN_LOGIC_OR };
		return t;
	}
	if (c == '=') {
		consume(ctx);
		if (lookahead(ctx) != '=') {
			token_t t = { TOKEN_ASSIGN };
			return t;
		} else {
			token_t t = { TOKEN_EQUALS };
			consume(ctx);
			return t;
		}
	}

	token_t t = { TOKEN_EOF };
	error_assert(false, "invalid character");
	return t;
}
