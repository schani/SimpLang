#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "dynarr.h"
#include "compiler.h"

static inline int64_t*
get_slot (vm_t *vm, size_t abs_slot)
{
	assert (abs_slot < vm->array_size);
	return &vm->value_array[abs_slot];
}

static inline int64_t
vs_load (vm_t *vm, ssize_t rel_slot)
{
	return *get_slot(vm, (size_t)(vm->stack_pointer + rel_slot));
}

static inline void
vs_store (vm_t *vm, ssize_t rel_slot, int64_t value)
{
	*get_slot(vm, (size_t)(vm->stack_pointer + rel_slot)) = value;
}

static inline void
vs_push (vm_t *vm, int64_t n_slots)
{
	vm->stack_pointer += n_slots;
}

static inline void
vs_pop (vm_t *vm, int64_t n_slots)
{
	vm->stack_pointer -= n_slots;
}

static inline void
cs_push (vm_t *vm, int32_t pc)
{
	assert(vm->call_stack_pointer < vm->call_stack_size);
	vm->call_stack[vm->call_stack_pointer++] = pc;
}

static inline bool
cs_is_empty (vm_t *vm)
{
	return vm->call_stack_pointer == 0;
}

static inline int32_t
cs_pop (vm_t *vm)
{
	assert(!cs_is_empty(vm));
	return vm->call_stack[--vm->call_stack_pointer];
}

void
vm_init (vm_t *vm, size_t stack_size, size_t call_stack_size)
{
	vm->value_array = calloc(stack_size, sizeof(int64_t));
	assert(vm->value_array);
	vm->array_size = stack_size;
	vm->stack_pointer = 0;

	vm->call_stack = calloc(call_stack_size, sizeof(int32_t));
	vm->call_stack_size = call_stack_size;
	vm->call_stack_pointer = 0;
}

static char*
skip (char *p, const char *set)
{
	while (strchr(set, *p) != NULL) {
		assert(*p != 0);
		p++;
	}
	return p;
}

static char*
find (char *p, const char *set)
{
	while (strchr(set, *p) == NULL) {
		assert(*p != 0);
		p++;
	}
	return p;
}

static int64_t
parse_arg (char **p)
{
	char *start = skip(*p, ", \t$");
	char *end = skip(start, "-0123456789");
	char *convert_end;
	int64_t arg = (int64_t)strtoll(start, &convert_end, 10);
	assert(end == convert_end);
	*p = end;
	return arg;
}

static vm_opcode_t
which_opcode (int len, char *name, int *nargs)
{
	static struct { const char *name; int nargs; } instructions[] = {
		{ "Move", 2 },
		{ "Set", -1 },
		{ "Add", 3 },
		{ "Multiply", 3 },
		{ "Negate", 2 },
		{ "Not", 2 },
		{ "Jump", 1 },
		{ "JumpIfZero", 2 },
		{ "Call", 3 },
		{ "Return", 1 },
		{ "LessThan", 3 },
		{ "Equals", 3 },
		{ NULL, 0 }
	};
	for (int i = 0; instructions[i].name != NULL; i++) {
		if (strlen(instructions[i].name) != len)
			continue;
		if (strncmp(name, instructions[i].name, len) == 0) {
			*nargs = instructions[i].nargs;
			return (vm_opcode_t)i;
		}
	}
	assert(false);
}

#define LINE_LENGTH 128

void
vm_load (vm_t *vm, const char *filename)
{
	pool_t pool;
	pool_init(&pool);
	dynarr_t ins_ptrs;
	dynarr_init(&ins_ptrs, &pool);
	char line[LINE_LENGTH];
	FILE *f = fopen(filename, "r");
	assert(f != NULL);
	while (fgets(line, LINE_LENGTH, f) != NULL) {
		vm_ins_t *ins = pool_alloc(&pool, sizeof(vm_ins_t));
		char *start = skip(line, "0123456789 \t");
		char *end = find(start, " \t");
		int nargs;
		ins->opcode = which_opcode(end - start, start, &nargs);

		int32_t arg1 = (int32_t)parse_arg(&end);
		if (nargs == -1) {
			int64_t imm = parse_arg(&end);
			ins->args.imm.arg = arg1;
			ins->args.imm.imm = imm;
		} else {
			int32_t arg2 = 0, arg3 = 0;
			if (nargs >= 2)
				arg2 = (int32_t)parse_arg(&end);
			if (nargs >= 3)
				arg3 = (int32_t)parse_arg(&end);
			ins->args.slot.arg1 = arg1;
			ins->args.slot.arg2 = arg2;
			ins->args.slot.arg3 = arg3;
		}

		dynarr_append(&ins_ptrs, ins);
	}

	vm->num_instructions = dynarr_length(&ins_ptrs);
	vm->instructions = malloc(sizeof(vm_ins_t) * vm->num_instructions);
	for (int i = 0; i < vm->num_instructions; i++)
		memcpy(&vm->instructions[i], dynarr_nth(&ins_ptrs, i), sizeof(vm_ins_t));

	pool_free(&pool);
}

void
vm_push_args (vm_t *vm, int argc, int64_t *args)
{
	for (int i = 0; i < argc; i++)
		vs_store(vm, i, args[i]);
	vs_push(vm, argc);
}

int64_t
vm_run (vm_t *vm)
{
	int64_t tmp, tmp2;
	int32_t pc = 0;

	for (;;) {
		vm_ins_t *ins = &vm->instructions[pc];
		switch (ins->opcode) {
			case VM_OP_ADD:
				tmp = vs_load(vm, ins->args.slot.arg2) + vs_load(vm, ins->args.slot.arg3);
				vs_store(vm, ins->args.slot.arg1, tmp);
				break;
			case VM_OP_SET:
				vs_store(vm, ins->args.imm.arg, ins->args.imm.imm);
				break;
			case VM_OP_RETURN:
				tmp = vs_load(vm, ins->args.slot.arg1);
				if (cs_is_empty(vm))
					return tmp;
				pc = cs_pop(vm);
				ins = &vm->instructions[pc];
				assert(ins->opcode == VM_OP_CALL);
				vs_pop(vm, ins->args.slot.arg2);
				vs_store(vm, ins->args.slot.arg3, tmp);
				break;
			case VM_OP_MOVE:
				tmp = vs_load(vm, ins->args.slot.arg2);
				vs_store(vm, ins->args.slot.arg1, tmp);
				break;
			case VM_OP_MULTIPLY:
				tmp = vs_load(vm, ins->args.slot.arg2) * vs_load(vm, ins->args.slot.arg3);
				vs_store(vm, ins->args.slot.arg1, tmp);
				break;
			case VM_OP_NEGATE:
				tmp = vs_load(vm, ins->args.slot.arg2);
				vs_store(vm, ins->args.slot.arg1, -tmp);
				break;
			case VM_OP_NOT:
				tmp = vs_load(vm, ins->args.slot.arg2);
				vs_store(vm, ins->args.slot.arg1, tmp == 0 ? 1 : 0);
				break;
			case VM_OP_LESS_THAN:
				tmp = vs_load(vm, ins->args.slot.arg2);
				tmp2 = vs_load(vm, ins->args.slot.arg3);
				vs_store(vm, ins->args.slot.arg1, tmp < tmp2 ? 1 : 0);
				break;
			case VM_OP_EQUALS:
				tmp = vs_load(vm, ins->args.slot.arg2);
				tmp2 = vs_load(vm, ins->args.slot.arg3);
				vs_store(vm, ins->args.slot.arg1, tmp == tmp2 ? 1 : 0);
				break;
			case VM_OP_JUMP:
				pc = ins->args.slot.arg1;
				continue;
			case VM_OP_JUMP_IF_ZERO:
				tmp = vs_load(vm, ins->args.slot.arg1);
				if (tmp == 0) {
					pc = ins->args.slot.arg2;
					continue;
				}
				break;
			case VM_OP_CALL:
				cs_push(vm, pc);
				vs_push(vm, ins->args.slot.arg2);
				pc = ins->args.slot.arg1;
				continue;

			default:
				assert(false);
				return 0;
		}

		pc++;
	}
}

#define N 1024
#define M 32

void
vm_test_value_stack (vm_t *vm)
{
	for (int64_t i = 0; i < N; i++)
		vs_store(vm, i, i);

	int64_t off = 0;
	for (int64_t j = 0; j < M; j++) {
		for (int64_t i = 0; i < N; i++)
			assert(vs_load(vm, i - off) == i);
		vs_push(vm, j);
		off += j;
	}

	for (int64_t j = 0; j < M; j++) {
		for (int64_t i = 0; i < N; i++)
			assert(vs_load(vm, i - off) == i);
		vs_pop(vm, j);
		off -= j;
	}

	printf("test passed\n");
}
