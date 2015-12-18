all:
	gcc -g main.c lexer.c parser.c eval.c \
		-Wall -Wextra -Wno-unused -o compiler

	gcc -g interpreter.c lexer.c parser.c eval.c \
		-Wall -Wextra -Wno-unused -o interpreter
