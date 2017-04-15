#include <stdbool.h>
#include <assert.h>
#include <string.h>

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

static int64_t
env_lookup (environment_t *env, char *name)
{
	for (;;) {
		assert(env != NULL);
		if (strcmp(env->name, name) == 0)
			return env->value;
		env = env->next;
	}
}

static environment_t*
env_bind (environment_t *env, char *name, int64_t value)
{
	environment_t *new = malloc(sizeof(environment_t));
	new->name = name;
	new->value = value;
	new->next = env;
	return new;
}

static void
env_free (environment_t *env, environment_t *old)
{
	while (env != old) {
		environment_t *next = env->next;
		free(env);
		env = next;
	}
}

int64_t
eval_expr (environment_t *env, expr_t *expr)
{
	switch (expr->type) {
		case EXPR_INTEGER:
			return expr->v.i;

		case EXPR_IDENT:
			return env_lookup(env, expr->v.ident);

		case EXPR_IF:
			if (eval_expr(env, expr->v.if_expr.condition))
				return eval_expr(env, expr->v.if_expr.consequent);
			else
				return eval_expr(env, expr->v.if_expr.alternative);

		case EXPR_UNARY: {
			int64_t operand = eval_expr(env, expr->v.unary.operand);
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
			int64_t left = eval_expr(env, expr->v.binary.left);
			if (expr->v.binary.op == TOKEN_LOGIC_AND) {
				if (!left)
					return 0;
				return boolify_int(eval_expr(env, expr->v.binary.right));
			}
			if (expr->v.binary.op == TOKEN_LOGIC_OR) {
				if (left)
					return 1;
				return boolify_int(eval_expr(env, expr->v.binary.right));
			}
			int64_t right = eval_expr(env, expr->v.binary.right);
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

		case EXPR_LET: {
			environment_t *old = env;
			for (int i = 0; i < expr->v.let_loop.n; i++) {
				int64_t value = eval_expr(env, expr->v.let_loop.bindings[i].expr);
				env = env_bind(env, expr->v.let_loop.bindings[i].name, value);
			}
			int64_t result = eval_expr(env, expr->v.let_loop.body);
			env_free(env, old);
			return result;
		}

		default:
			assert(false);
	}
}
