#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

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
print_indent (int indent)
{
	for (int i = 0; i < indent; i++)
		printf("  ");
}

static void
print_expr (expr_t *expr, int indent)
{
	print_indent(indent);
	switch (expr->type) {
		case EXPR_INTEGER:
			printf("%" PRId64 "\n", expr->v.i);
			break;
		case EXPR_IDENT:
			printf("%s\n", expr->v.ident);
			break;
		case EXPR_IF:
			printf("if\n");
			print_expr(expr->v.if_expr.condition, indent + 1);
			print_expr(expr->v.if_expr.consequent, indent + 1);
			print_expr(expr->v.if_expr.alternative, indent + 1);
			break;
		case EXPR_LET:
		case EXPR_LOOP:
			printf("%s\n", expr->type == EXPR_LET ? "let" : "loop");
			for (int i = 0; i < expr->v.let_loop.n; i++) {
				binding_t *binding = &expr->v.let_loop.bindings[i];
				print_indent(indent + 2);
				printf("%s\n", binding->name);
				print_expr(binding->expr, indent + 3);
			}
			print_expr(expr->v.let_loop.body, indent + 1);
			break;
		case EXPR_RECUR:
			printf("recur\n");
			for (int i = 0; i < expr->v.recur.n; i++)
				print_expr(expr->v.recur.args[i], indent + 1);
			break;
		case EXPR_CALL:
			printf("%s\n", expr->v.call.name);
			for (int i = 0; i < expr->v.call.n; i++)
				print_expr(expr->v.call.args[i], indent + 1);
			break;
		case EXPR_UNARY:
			printf("%s\n", token_type_operator_name(expr->v.unary.op));
			print_expr(expr->v.unary.operand, indent + 1);
			break;
		case EXPR_BINARY:
			printf("%s\n", token_type_operator_name(expr->v.binary.op));
			print_expr(expr->v.binary.left, indent + 1);
			print_expr(expr->v.binary.right, indent + 1);
			break;
		default:
			assert(false);
	}
}

static void
print_function (function_t *function)
{
	printf("function\n");
	print_indent(2);
	printf("%s\n", function->name);
	for (int i = 0; i < function->n_args; i++) {
		print_indent(3);
		printf("%s\n", function->args[i]);
	}
	print_expr(function->body, 1);
}

static void
print_program (program_t *prog)
{
	for (function_t *func = prog->functions; func != NULL; func = func->next)
		print_function(func);
}

static void
parse_main (context_t *ctx)
{
	expr_t *expr = parse_expr(ctx);
	print_expr(expr, 0);
}

static void
parse_function_main (context_t *ctx)
{
	function_t *function = parse_function(ctx);
	print_function(function);
}

static void
parse_program_main (context_t *ctx)
{
	program_t *prog = parse_program(ctx);
	print_program(prog);
}

static void
eval_main (context_t *ctx)
{
	expr_t *expr = parse_expr(ctx);
	int64_t result = eval_expr(NULL, NULL, expr);
	printf("%" PRId64 "\n", result);
}

static int64_t*
parse_cmdline_args (context_t *ctx, int argc, const char **argv)
{
	int64_t *args = pool_alloc(&ctx->pool, argc * sizeof(int64_t));
	for (int i = 0; i < argc; i++)
		args[i] = (int64_t)strtoll(argv[i], NULL, 10);
	return args;
}

static int
eval_program_main (context_t *ctx, int argc, const char **argv)
{
	program_t *program = parse_program(ctx);
	function_t *function = lookup_function(program, "main");

	if (function == NULL) {
		fprintf(stderr, "Error: Function main must be defined.\n");
		return 1;
	}

	if (function->n_args != argc) {
		fprintf(stderr, "Error: main expects %d args, but got %d.\n", function->n_args, argc);
		return 2;
	}

	int64_t *args = parse_cmdline_args(ctx, function->n_args, argv);
	int64_t result = eval_function(program, function, args);
	printf("%" PRId64 "\n", result);

	return 0;
}

static void
vm_test_main (void)
{
	vm_t vm;
	vm_init(&vm, 32768, 1024);
	vm_test_value_stack(&vm);
}

static int
vm_main (context_t *ctx, const char *filename, int argc, const char **argv)
{
	//assert(argc >= 3);
	int64_t *args = parse_cmdline_args(ctx, argc, argv);
	vm_t vm;
	vm_init(&vm, 32768, 1024);
	vm_load(&vm, filename);
	vm_push_args(&vm, argc, args);
	int64_t result = vm_run(&vm);
	printf("%" PRId64 "\n", result);
	return 0;
}

int
main (int argc, const char *argv[])
{
	context_t ctx;
	pool_init(&ctx.pool);

/*
	if (argc < 2) {
		fprintf(stderr, "Usage: simplang FILE [ARGS]\n");
		return 1;
	}

	scan_init(&ctx, argv[1]);
	parser_init(&ctx);
*/

	//scan_main(&ctx);
	//parse_main(&ctx);
	//parse_function_main(&ctx);
	//eval_main(&ctx);
	//parse_program_main(&ctx);
	//return eval_program_main(&ctx, argc - 2, argv + 2);

	//vm_test_main();
	return vm_main(&ctx, argv[1], argc - 2, argv + 2);

	return 0;
}
