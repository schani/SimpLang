#include <stdio.h>
#include <assert.h>

#include "compiler.h"

int
main (int argc, char *argv[])
{
	context_t ctx;

	if (argc != 2) {
		fprintf(stderr, "Usage: simplang FILE\n");
		return 1;
	}

	scan_init(&ctx, argv[1]);

	for (;;) {
		token_t token = scan(&ctx);
		if (token.type == TOKEN_EOF)
			break;

		if (token_type_is_keyword(token.type)) {
			printf("keyword %s\n", token_type_keyword_name(token.type));
		} else if (token_type_is_operator(token.type)) {
			printf("operator %s\n", token_type_operator_name(token.type));
		} else if (token.type == TOKEN_INTEGER) {
			printf("integer %lld\n", token.v.i);
		} else if (token.type == TOKEN_IDENT) {
			printf("identifier %s\n", token.v.name);
		} else {
			assert(false);
		}
	}

	return 0;
}
