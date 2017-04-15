#include <assert.h>
#include <string.h>

#include "compiler.h"
#include "dynarr.h"

static token_t lookahead;

static token_t
consume (context_t *ctx)
{
	token_t result = lookahead;
	lookahead = scan(ctx);
	return result;
}

void
parser_init (context_t *ctx)
{
	lookahead = scan(ctx);
}

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
	token_t t = consume(ctx);
	assert(t.type == type);
	return t;
}

expr_t*
parse_expr (context_t *ctx)
{
	token_t t = consume(ctx);
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

			//| ("let" | "loop") bindings "in" expr "end"
		case TOKEN_LET:
		case TOKEN_LOOP: {
			expr_type_t expr_type = t.type == TOKEN_LET ? EXPR_LET : EXPR_LOOP;
			dynarr_t arr;
			dynarr_init(&arr, &ctx->pool);
			do {
				binding_t *binding = pool_alloc(&ctx->pool, sizeof(binding_t));

				t = expect_token(ctx, TOKEN_IDENT);
				binding->name = t.v.name;
				expect_token(ctx, TOKEN_ASSIGN);
				binding->expr = parse_expr(ctx);

				dynarr_append(&arr, binding);

				t = consume(ctx);

				assert(t.type == TOKEN_AND || t.type == TOKEN_IN);
			} while (t.type == TOKEN_AND);

			expr = alloc_expr(ctx, expr_type);

			expr->v.let_loop.body = parse_expr(ctx);
			expect_token(ctx, TOKEN_END);

			binding_t *bindings = pool_alloc(&ctx->pool, sizeof(binding_t) * dynarr_length(&arr));
			for (int i = 0; i < dynarr_length(&arr); i++)
				memcpy(&bindings[i], dynarr_nth(&arr, i), sizeof(binding_t));

			expr->v.let_loop.n = dynarr_length(&arr);
			expr->v.let_loop.bindings = bindings;

			break;
		}

			//| "recur" arg {arg}
			//arg = "(" expr ")"
		case TOKEN_RECUR: {
			dynarr_t arr;
			dynarr_init(&arr, &ctx->pool);
			do {
				expect_token(ctx, TOKEN_OPEN_PAREN);
				dynarr_append(&arr, parse_expr(ctx));
				expect_token(ctx, TOKEN_CLOSE_PAREN);
			} while (lookahead.type == TOKEN_OPEN_PAREN);
			expr = alloc_expr(ctx, EXPR_RECUR);
			expr->v.recur.n = dynarr_length(&arr);
			expr->v.recur.args = (expr_t**)dynarr_data(&arr);
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
			t = consume(ctx);
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
