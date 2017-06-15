#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "compiler.h"

typedef struct
{
	int64_t i;
	environment_t *loop_env;
} intp_result_t;

static inline intp_result_t
make_int_result (int64_t i)
{
	intp_result_t res = { i, NULL };
	return res;
}

static inline intp_result_t
make_recur_result (environment_t *loop_env)
{
	intp_result_t res = { 0, loop_env };
	return res;
}

static inline int64_t
int_result (intp_result_t res)
{
	assert(res.loop_env == NULL);
	return res.i;
}

static intp_result_t
boolify_int (int64_t i)
{
	if (i)
		return make_int_result(1);
	else
		return make_int_result(0);
}

static intp_result_t
bool_to_int (bool b)
{
	if (b)
		return make_int_result(1);
	else
		return make_int_result(0);
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

function_t*
lookup_function (program_t *prog, char *name)
{
	for (function_t *func = prog->functions; func != NULL; func = func->next) {
		if (strcmp(func->name, name) == 0)
			return func;
	}
	return NULL;
}

static int64_t
call_function (program_t *prog, char *name, int64_t *args)
{
	function_t *func = lookup_function(prog, name);
	assert(func != NULL);
	return eval_function(prog, func, args);
}

static intp_result_t eval (program_t *prog, environment_t *env, expr_t *expr, expr_t *innermost_loop, environment_t *loop_env);

int64_t
eval_expr (program_t *prog, environment_t *env, expr_t *expr)
{
	return int_result(eval(prog, env, expr, NULL, NULL));
}

static intp_result_t
eval (program_t *prog, environment_t *env, expr_t *expr, expr_t *innermost_loop, environment_t *loop_env)
{
	switch (expr->type) {
		case EXPR_INTEGER:
			return make_int_result(expr->v.i);

		case EXPR_IDENT:
			return make_int_result(env_lookup(env, expr->v.ident));

		case EXPR_IF:
			if (eval_expr(prog, env, expr->v.if_expr.condition))
				return eval(prog, env, expr->v.if_expr.consequent, innermost_loop, loop_env);
			else
				return eval(prog, env, expr->v.if_expr.alternative, innermost_loop, loop_env);

		case EXPR_UNARY: {
			int64_t operand = eval_expr(prog, env, expr->v.unary.operand);
			switch (expr->v.unary.op) {
				case TOKEN_NOT:
					if (operand)
						return make_int_result(0);
					else
						return make_int_result(1);

				case TOKEN_NEGATE:
					return make_int_result(-operand);

				default:
					assert(false);
			}
			break;
		}

		case EXPR_BINARY: {
			int64_t left = eval_expr(prog, env, expr->v.binary.left);
			if (expr->v.binary.op == TOKEN_LOGIC_AND) {
				if (!left)
					return make_int_result(0);
				return boolify_int(eval_expr(prog, env, expr->v.binary.right));
			}
			if (expr->v.binary.op == TOKEN_LOGIC_OR) {
				if (left)
					return make_int_result(1);
				return boolify_int(eval_expr(prog, env, expr->v.binary.right));
			}
			int64_t right = eval_expr(prog, env, expr->v.binary.right);
			switch (expr->v.binary.op) {
				case TOKEN_LESS:
					return bool_to_int(left < right);

				case TOKEN_EQUALS:
					return bool_to_int(left == right);

				case TOKEN_PLUS:
					return make_int_result(left + right);

				case TOKEN_TIMES:
					return make_int_result(left * right);

				default:
					assert(false);
			}
			break;
		}

		case EXPR_LET: {
			environment_t *old = env;
			for (int i = 0; i < expr->v.let_loop.n; i++) {
				int64_t value = eval_expr(prog, env, expr->v.let_loop.bindings[i].expr);
				env = env_bind(env, expr->v.let_loop.bindings[i].name, value);
			}
			intp_result_t result = eval(prog, env, expr->v.let_loop.body, innermost_loop, loop_env);
			env_free(env, old);
			return result;
		}

		case EXPR_LOOP: {
			environment_t *old = env;
			for (int i = 0; i < expr->v.let_loop.n; i++) {
				int64_t value = eval_expr(prog, env, expr->v.let_loop.bindings[i].expr);
				env = env_bind(env, expr->v.let_loop.bindings[i].name, value);
			}
			for (;;) {
				intp_result_t result = eval(prog, env, expr->v.let_loop.body, expr, old);
				env_free(env, old);
				if (result.loop_env == NULL)
					return result;
				env = result.loop_env;
			}
		}

		case EXPR_RECUR: {
			assert(innermost_loop != NULL && innermost_loop->type == EXPR_LOOP);
			assert(expr->v.recur.n == innermost_loop->v.let_loop.n);
			for (int i = 0; i < expr->v.recur.n; i++) {
				int64_t value = eval_expr(prog, env, expr->v.recur.args[i]);
				loop_env = env_bind(loop_env, innermost_loop->v.let_loop.bindings[i].name, value);
			}
			return make_recur_result(loop_env);
		}

		case EXPR_CALL: {
			int64_t args[expr->v.call.n];
			for (int i = 0; i < expr->v.call.n; i++)
				args[i] = eval_expr(prog, env, expr->v.call.args[i]);
			return make_int_result(call_function(prog, expr->v.call.name, args));
		}

		default:
			assert(false);
	}
}

int64_t
eval_function (program_t *prog, function_t *function, int64_t *args)
{
	environment_t *env = NULL;
	for (int i = 0; i < function->n_args; i++)
		env = env_bind(env, function->args[i], args[i]);
	int64_t result = eval_expr(prog, env, function->body);
	env_free(env, NULL);
	return result;
}
