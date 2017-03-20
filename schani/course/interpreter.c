#include <stdbool.h>
#include <assert.h>

#include "compiler.h"

static int64_t
boolify_int (int64_t i)
{
	if (i)
		return 1;
	else
		return 0;
}

static int64_t
bool_to_int (bool b)
{
	if (b)
		return 1;
	else
		return 0;
}

int64_t
eval_expr (expr_t *expr)
{
	switch (expr->type) {
		case EXPR_INTEGER:
			return expr->v.i;

		case EXPR_IF:
			if (eval_expr(expr->v.if_expr.condition))
				return eval_expr(expr->v.if_expr.consequent);
			else
				return eval_expr(expr->v.if_expr.alternative);

		case EXPR_UNARY: {
			int64_t operand = eval_expr(expr->v.unary.operand);
			switch (expr->v.unary.op) {
				case TOKEN_NOT:
					if (operand)
						return 0;
					else
						return 1;

				case TOKEN_NEGATE:
					return -operand;

				default:
					assert(false);
			}
			break;
		}

		case EXPR_BINARY: {
			int64_t left = eval_expr(expr->v.binary.left);
			if (expr->v.binary.op == TOKEN_LOGIC_AND) {
				if (!left)
					return 0;
				return boolify_int(eval_expr(expr->v.binary.right));
			}
			if (expr->v.binary.op == TOKEN_LOGIC_OR) {
				if (left)
					return 1;
				return boolify_int(eval_expr(expr->v.binary.right));
			}
			int64_t right = eval_expr(expr->v.binary.right);
			switch (expr->v.binary.op) {
				case TOKEN_LESS:
					return bool_to_int(left < right);

				case TOKEN_EQUALS:
					return bool_to_int(left == right);

				case TOKEN_PLUS:
					return left + right;

				case TOKEN_TIMES:
					return left * right;

				default:
					assert(false);
			}
			break;
		}

		default:
			assert(false);
	}
}
