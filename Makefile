# get rm and rmdir and sep
ifeq ($(OS), Windows_NT)
	RM := del
	RMDIR := rmdir /s /q
	SEP := \\
	COPY-WS2-32 := copy C:$(SEP)Windows$(SEP)System32$(SEP)ws2_32.dll build$(SEP)dll$(SEP)
	LIBS := -l ws2_32
else
	RM := rm
	RMDIR := rm -rf
	SEP := /
	COPY-WS2-32 := 
	LIBS := 
endif

# windows's mkdir not has -p option :/
MKDIR := mkdir
CC := gcc
INCLUDE := .
RELEASE := 0
OPT := 
VALGRIND := 
PROG :=
TEST_PROG :=

ifeq ($(OS), Windows_NT)
	PROG := aji.exe
	TEST_PROG := aji_tests.exe
	ifeq ($(RELEASE), 1)
		CFLAGS := -Wall \
			-O2  \
			-std=c11 \
			-Wno-unused-function \
			-Wno-unused-result \
			-I$(INCLUDE) \
			-L build$(SEP)dll
	else
		CFLAGS := -Wall \
			-g \
			-O0  \
			-std=c11 \
			-Wno-unused-function \
			-Wno-unused-result \
			-D_DEBUG \
			-I$(INCLUDE) \
			-L build$(SEP)dll
	endif
	OUTLIB := libaji.dll
else
	PROG := aji
	TEST_PROG := aji_tests
	VALGRIND := valgrind
	ifeq ($(RELEASE), 1)
		CFLAGS := -Wall \
			-O2 \
			-std=c11 \
			-Wno-unused-function \
			-Wno-unused-result \
			-I$(INCLUDE) \
			-fPIC \
			$(OPT)
	else
		CFLAGS := -Wall \
			-g \
			-O0 \
			-std=c11 \
			-Wno-unused-function \
			-Wno-unused-result \
			-D_DEBUG \
			-I$(INCLUDE) \
			-fPIC \
			$(OPT)
	endif
	OUTLIB := libaji.so
endif

# this is benri tool
# $(warning $(wildcard aji/*.c))

all: tests aji lib

.PHONY: clean
clean:
	$(RMDIR) build

.PHONY: init
init:
	$(MKDIR) \
	build \
	build$(SEP)dll \
	build$(SEP)tests \
	build$(SEP)lib \
	build$(SEP)core \
	build$(SEP)lang$(SEP) \
	build$(SEP)lang$(SEP)builtin \
	build$(SEP)lang$(SEP)builtin$(SEP)modules

.PHONY: cc
cc:
	$(CC) -v

SRCS := build/lib/error.c \
	build/lib/memory.c \
	build/lib/memman.c \
	build/lib/memory_chain.c \
	build/lib/file.c \
	build/lib/cstring.c \
	build/lib/string.c \
	build/lib/unicode.c \
	build/lib/bytes.c \
	build/lib/cstring_vector.c \
	build/lib/cl.c \
	build/lib/format.c \
	build/lib/dict.c \
	build/lib/void_dict.c \
	build/lib/void_vector.c \
	build/lib/unicode_vector.c \
	build/lib/cmdline.c \
	build/lib/pipe.c \
	build/lib/term.c \
	build/lib/path.c \
	build/lib/unicode_path.c \
	build/lib/socket.c \
	build/core/config.c \
	build/core/util.c \
	build/core/args.c \
	build/core/error_stack.c \
	build/lang/tokens.c \
	build/lang/tokenizer.c \
	build/lang/nodes.c \
	build/lang/ast.c \
	build/lang/compiler.c \
	build/lang/traverser.c \
	build/lang/object.c \
	build/lang/object_vector.c \
	build/lang/object_dict.c \
	build/lang/node_vector.c \
	build/lang/node_dict.c \
	build/lang/opts.c \
	build/lang/utils.c \
	build/lang/gc.c \
	build/lang/kit.c \
	build/lang/global.c \
	build/lang/importer.c \
	build/lang/arguments.c \
	build/lang/chain_node.c \
	build/lang/chain_nodes.c \
	build/lang/chain_object.c \
	build/lang/chain_objects.c \
	build/lang/excepts.c \
	build/lang/lex_env.c \
	build/lang/lex_env_list.c \
	build/lang/builtin/structs.c \
	build/lang/builtin/functions.c \
	build/lang/builtin/func_info_vector.c \
	build/lang/builtin/module.c \
	build/lang/builtin/modules/unicode.c \
	build/lang/builtin/modules/bytes.c \
	build/lang/builtin/modules/vector.c \
	build/lang/builtin/modules/dict.c \
	build/lang/builtin/modules/opts.c \
	build/lang/builtin/modules/file.c \
	build/lang/builtin/modules/os.c \
	build/lang/builtin/modules/io.c \
	build/lang/builtin/modules/env.c \
	build/lang/builtin/modules/socket.c \

OBJS := $(SRCS:.c=.o)

tests: build/tests.o $(OBJS)
	#$(COPY-WS2-32)
	$(CC) $(CFLAGS) -o build/aji_tests $^ $(LIBS)

aji: build/app.o $(OBJS)
	#$(COPY-WS2-32)
	$(CC) $(CFLAGS) -o build/aji $^ $(LIBS)

lib: $(OBJS)
	#$(COPY-WS2-32)
	$(CC) $(CFLAGS) -shared -o build/$(OUTLIB) $^ $(LIBS)

.PHONY: test
test: build$(SEP)$(PROG) build$(SEP)$(TEST_PROG)
	$(VALGRIND) build$(SEP)aji_tests cstring_vector && \
	$(VALGRIND) build$(SEP)aji_tests cstring && \
	$(VALGRIND) build$(SEP)aji_tests string && \
	$(VALGRIND) build$(SEP)aji_tests unicode && \
	$(VALGRIND) build$(SEP)aji_tests bytes && \
	$(VALGRIND) build$(SEP)aji_tests file && \
	$(VALGRIND) build$(SEP)aji_tests cl && \
	$(VALGRIND) build$(SEP)aji_tests cmdline && \
	$(VALGRIND) build$(SEP)aji_tests error && \
	$(VALGRIND) build$(SEP)aji_tests util && \
	$(VALGRIND) build$(SEP)aji_tests path && \
	$(VALGRIND) build$(SEP)aji_tests unicode_path && \
	$(VALGRIND) build$(SEP)aji_tests dict && \
	$(VALGRIND) build$(SEP)aji_tests void_dict && \
	$(VALGRIND) build$(SEP)aji_tests void_vector && \
	$(VALGRIND) build$(SEP)aji_tests tokenizer && \
	$(VALGRIND) build$(SEP)aji_tests compiler && \
	$(VALGRIND) build$(SEP)aji_tests traverser_1 && \
	$(VALGRIND) build$(SEP)aji_tests traverser_2 && \
	$(VALGRIND) build$(SEP)aji_tests traverser_3 && \
	$(VALGRIND) build$(SEP)aji_tests traverser_4 && \
	$(VALGRIND) build$(SEP)aji_tests lib.list && \
	$(VALGRIND) build$(SEP)aji_tests lib.stream && \
	$(VALGRIND) build$(SEP)aji_tests error_stack && \
	$(VALGRIND) build$(SEP)aji_tests gc && \
	$(VALGRIND) build$(SEP)aji_tests objdict && \
	$(VALGRIND) build$(SEP)aji tests/tests.aji

.PHONY: test-full
test-full: build$(SEP)$(PROG) build$(SEP)$(TEST_PROG)
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests cstring_vector && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests cstring && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests string && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests unicode && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests bytes && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests file && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests cl && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests cmdline && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests error && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests util && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests path && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests unicode_path && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests dict && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests void_dict && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests void_vector && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests tokenizer && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests compiler && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests traverser_1 && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests traverser_2 && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests traverser_3 && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests traverser_4 && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests lib.list && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests lib.stream && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests error_stack && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests gc && \
	$(VALGRIND) --leak-check=full build$(SEP)aji_tests objdict && \
	$(VALGRIND) --leak-check=full build$(SEP)aji tests/tests.aji

.PHONY: full
full:
	make clean
	make init
	make -j4
	make test

build/app.o: aji/app.c aji/app.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/tests.o: tests/tests.c tests/tests.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/error.o: aji/lib/error.c aji/lib/error.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/memory.o: aji/lib/memory.c aji/lib/memory.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/memman.o: aji/lib/memman.c aji/lib/memman.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/memory_chain.o: aji/lib/memory_chain.c aji/lib/memory_chain.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/file.o: aji/lib/file.c aji/lib/file.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/cstring.o: aji/lib/cstring.c aji/lib/cstring.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/string.o: aji/lib/string.c aji/lib/string.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/unicode.o: aji/lib/unicode.c aji/lib/unicode.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/bytes.o: aji/lib/bytes.c aji/lib/bytes.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/cstring_vector.o: aji/lib/cstring_vector.c aji/lib/cstring_vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/cl.o: aji/lib/cl.c aji/lib/cl.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/format.o: aji/lib/format.c aji/lib/format.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/dict.o: aji/lib/dict.c aji/lib/dict.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/void_dict.o: aji/lib/void_dict.c aji/lib/void_dict.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/void_vector.o: aji/lib/void_vector.c aji/lib/void_vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/unicode_vector.o: aji/lib/unicode_vector.c aji/lib/unicode_vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/cmdline.o: aji/lib/cmdline.c aji/lib/cmdline.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/pipe.o: aji/lib/pipe.c aji/lib/pipe.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/term.o: aji/lib/term.c aji/lib/term.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/path.o: aji/lib/path.c aji/lib/path.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/unicode_path.o: aji/lib/unicode_path.c aji/lib/unicode_path.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lib/socket.o: aji/lib/socket.c aji/lib/socket.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/core/config.o: aji/core/config.c aji/core/config.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/core/util.o: aji/core/util.c aji/core/util.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/core/alias_info.o: aji/core/alias_info.c aji/core/alias_info.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/core/error_stack.o: aji/core/error_stack.c aji/core/error_stack.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/core/args.o: aji/core/args.c aji/core/args.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/tokenizer.o: aji/lang/tokenizer.c aji/lang/tokenizer.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/tokens.o: aji/lang/tokens.c aji/lang/tokens.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/nodes.o: aji/lang/nodes.c aji/lang/nodes.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/ast.o: aji/lang/ast.c aji/lang/ast.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/compiler.o: aji/lang/compiler.c aji/lang/compiler.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/traverser.o: aji/lang/traverser.c aji/lang/traverser.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/object.o: aji/lang/object.c aji/lang/object.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/object_vector.o: aji/lang/object_vector.c aji/lang/object_vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/object_dict.o: aji/lang/object_dict.c aji/lang/object_dict.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/node_vector.o: aji/lang/node_vector.c aji/lang/node_vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/node_dict.o: aji/lang/node_dict.c aji/lang/node_dict.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/opts.o: aji/lang/opts.c aji/lang/opts.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/utils.o: aji/lang/utils.c aji/lang/utils.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/gc.o: aji/lang/gc.c aji/lang/gc.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/kit.o: aji/lang/kit.c aji/lang/kit.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/global.o: aji/lang/global.c aji/lang/global.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/importer.o: aji/lang/importer.c aji/lang/importer.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/arguments.o: aji/lang/arguments.c aji/lang/arguments.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/chain_node.o: aji/lang/chain_node.c aji/lang/chain_node.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/chain_nodes.o: aji/lang/chain_nodes.c aji/lang/chain_nodes.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/chain_object.o: aji/lang/chain_object.c aji/lang/chain_object.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/chain_objects.o: aji/lang/chain_objects.c aji/lang/chain_objects.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/excepts.o: aji/lang/excepts.c aji/lang/excepts.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/lex_env.o: aji/lang/lex_env.c aji/lang/lex_env.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/lex_env_list.o: aji/lang/lex_env_list.c aji/lang/lex_env_list.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/structs.o: aji/lang/builtin/structs.c 
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/functions.o: aji/lang/builtin/functions.c aji/lang/builtin/functions.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/func_info_vector.o: aji/lang/builtin/func_info_vector.c aji/lang/builtin/func_info_vector.h aji/lang/builtin/func_info.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/module.o: aji/lang/builtin/module.c aji/lang/builtin/module.h 
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/unicode.o: aji/lang/builtin/modules/unicode.c aji/lang/builtin/modules/unicode.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/bytes.o: aji/lang/builtin/modules/bytes.c aji/lang/builtin/modules/bytes.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/vector.o: aji/lang/builtin/modules/vector.c aji/lang/builtin/modules/vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/dict.o: aji/lang/builtin/modules/dict.c aji/lang/builtin/modules/dict.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/opts.o: aji/lang/builtin/modules/opts.c aji/lang/builtin/modules/opts.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/file.o: aji/lang/builtin/modules/file.c aji/lang/builtin/modules/file.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/os.o: aji/lang/builtin/modules/os.c aji/lang/builtin/modules/os.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/io.o: aji/lang/builtin/modules/io.c aji/lang/builtin/modules/io.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/env.o: aji/lang/builtin/modules/env.c aji/lang/builtin/modules/env.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
build/lang/builtin/modules/socket.o: aji/lang/builtin/modules/socket.c aji/lang/builtin/modules/socket.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
