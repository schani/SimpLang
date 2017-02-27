#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include "compiler.h"

static void
scan_main (context_t *ctx)
{
	for (;;) {
		token_t token = scan(ctx);
		if (token.type == TOKEN_EOF)
			break;

		if (token_type_is_keyword(token.type)) {
			printf("keyword %s\n", token_type_keyword_name(token.type));
		} else if (token_type_is_operator(token.type)) {
			printf("operator %s\n", token_type_operator_name(token.type));
		} else if (token.type == TOKEN_INTEGER) {
			printf("integer %" PRId64 "\n", token.v.i);
		} else if (token.type == TOKEN_IDENT) {
			printf("identifier %s\n", token.v.name);
		} else {
			assert(false);
		}
	}
}

static void
print_expr (expr_t *expr, int indent)
{
	for (int i = 0; i < indent; i++)
		printf("  ");
	switch (expr->type) {
		case EXPR_INTEGER:
			printf("%" PRId64 "\n", expr->v.i);
			break;
		case EXPR_IF:
			printf("if\n");
			print_expr(expr->v.if_expr.condition, indent + 1);
			print_expr(expr->v.if_expr.consequent, indent + 1);
			print_expr(expr->v.if_expr.alternative, indent + 1);
			break;
		default:
			assert(false);
	}
}

static void
parse_main (context_t *ctx)
{
	expr_t *expr = parse_expr(ctx);
	print_expr(expr, 0);
}

int
main (int argc, char *argv[])
{
	context_t ctx;

	if (argc != 2) {
		fprintf(stderr, "Usage: simplang FILE\n");
		return 1;
	}

	scan_init(&ctx, argv[1]);

	parse_main(&ctx);

	return 0;
}
