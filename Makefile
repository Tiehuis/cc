all:
	gcc main.c lexer.c parser.c token.c -Wall -Wextra -Wno-unused -g
