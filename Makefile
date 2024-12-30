CFLAGS = $(shell pkg-config --cflags glib-2.0) -Wall
LDFLAGS = $(shell pkg-config --libs glib-2.0) 

hello: 
	@echo "hello"

obj/symbol-table.o: src/symbol-table.c
	@gcc $(CFLAGS) -c src/symbol-table.c -o obj/symbol-table.o  $(LDFLAGS)

obj/utils.o: src/utils.c 
	@gcc -c src/utils.c -o obj/utils.o

parser.y: src/parser.y src/scanner.l
	@bison -d src/parser.y -o src/parser.tab.c
	@flex -o src/lex.yy.c src/scanner.l

assembler.o: parser.y obj/utils.o 
	@gcc $(CFLAGS)  obj/utils.o src/parser.tab.c src/lex.yy.c -o obj/assembler.o -lfl $(LDFLAGS)

main: assembler.o

clean : 
	@rm obj/* src/lex.yy.c src/*.tab.*