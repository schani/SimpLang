#include <assert.h>

#include "compiler.h"

static expr_t*
alloc_expr (context_t *ctx, expr_type_t type)
{
	expr_t *expr = pool_alloc(&ctx->pool, sizeof(expr_t));
	expr->type = type;
	return expr;
}

static void
expect_token (context_t *ctx, token_type_t type)
{
	token_t t = scan(ctx);
	assert(t.type == type);
}

expr_t*
parse_expr (context_t *ctx)
{
	token_t t = scan(ctx);
	expr_t *expr;

	switch (t.type) {
		case TOKEN_INTEGER:
			expr = alloc_expr(ctx, EXPR_INTEGER);
			expr->v.i = t.v.i;
			break;

		case TOKEN_IF:
			expr = alloc_expr(ctx, EXPR_IF);
			expr->v.if_expr.condition = parse_expr(ctx);
			expect_token(ctx, TOKEN_THEN);
			expr->v.if_expr.consequent = parse_expr(ctx);
			expect_token(ctx, TOKEN_ELSE);
			expr->v.if_expr.alternative = parse_expr(ctx);
			expect_token(ctx, TOKEN_END);
			break;

		default:
			// FIXME: parse error!
			assert(false);
	}
	return expr;
}
