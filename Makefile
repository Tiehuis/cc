all:
	gcc main.c lexer.c parser.c eval.c -Wall -Wextra -Wno-unused -g
