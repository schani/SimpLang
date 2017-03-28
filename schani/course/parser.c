#include <assert.h>
#include <string.h>

#include "compiler.h"
#include "dynarr.h"

static expr_t*
alloc_expr (context_t *ctx, expr_type_t type)
{
	expr_t *expr = pool_alloc(&ctx->pool, sizeof(expr_t));
	expr->type = type;
	return expr;
}

static token_t
expect_token (context_t *ctx, token_type_t type)
{
	token_t t = scan(ctx);
	assert(t.type == type);
	return t;
}

expr_t*
parse_expr (context_t *ctx)
{
	token_t t = scan(ctx);
	expr_t *expr;

	switch (t.type) {
			//integer
		case TOKEN_INTEGER:
			expr = alloc_expr(ctx, EXPR_INTEGER);
			expr->v.i = t.v.i;
			break;

			//| ident
		case TOKEN_IDENT:
			expr = alloc_expr(ctx, EXPR_IDENT);
			expr->v.ident = t.v.name;
			break;

			//| "if" expr "then" expr "else" expr "end"
		case TOKEN_IF:
			expr = alloc_expr(ctx, EXPR_IF);
			expr->v.if_expr.condition = parse_expr(ctx);
			expect_token(ctx, TOKEN_THEN);
			expr->v.if_expr.consequent = parse_expr(ctx);
			expect_token(ctx, TOKEN_ELSE);
			expr->v.if_expr.alternative = parse_expr(ctx);
			expect_token(ctx, TOKEN_END);
			break;

			//| "let" bindings "in" expr "end"
		case TOKEN_LET: {
			dynarr_t arr;
			dynarr_init(&arr, &ctx->pool);
			do {
				binding_t *binding = pool_alloc(&ctx->pool, sizeof(binding_t));

				t = expect_token(ctx, TOKEN_IDENT);
				binding->name = t.v.name;
				expect_token(ctx, TOKEN_ASSIGN);
				binding->expr = parse_expr(ctx);

				dynarr_append(&arr, binding);

				t = scan(ctx);

				assert(t.type == TOKEN_AND || t.type == TOKEN_IN);
			} while (t.type == TOKEN_AND);

			expr = alloc_expr(ctx, EXPR_LET);

			expr->v.let.body = parse_expr(ctx);
			expect_token(ctx, TOKEN_END);

			binding_t *bindings = pool_alloc(&ctx->pool, sizeof(binding_t) * dynarr_length(&arr));
			for (int i = 0; i < dynarr_length(&arr); i++)
				memcpy(&bindings[i], dynarr_nth(&arr, i), sizeof(binding_t));

			expr->v.let.n = dynarr_length(&arr);
			expr->v.let.bindings = bindings;

			break;
		}

			//| unop expr
		case TOKEN_NOT:
		case TOKEN_NEGATE:
			expr = alloc_expr(ctx, EXPR_UNARY);
			expr->v.unary.op = t.type;
			expr->v.unary.operand = parse_expr(ctx);
			break;

			//| "(" expr binop expr ")"
		case TOKEN_OPEN_PAREN:
			expr = alloc_expr(ctx, EXPR_BINARY);
			expr->v.binary.left = parse_expr(ctx);
			t = scan(ctx);
			assert(t.type >= TOKEN_FIRST_BINARY_OPERATOR && t.type <= TOKEN_LAST_BINARY_OPERATOR);
			expr->v.binary.op = t.type;
			expr->v.binary.right = parse_expr(ctx);
			expect_token(ctx, TOKEN_CLOSE_PAREN);
			break;

		default:
			// FIXME: parse error!
			assert(false);
	}
	return expr;
}
