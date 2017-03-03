#include <stdbool.h>
#include <assert.h>

#include "compiler.h"

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

		default:
			assert(false);
	}
}
