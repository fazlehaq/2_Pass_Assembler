hello: 
	@echo "hello"

obj/utils.o: src/utils.c 
	@gcc -c src/utils.c -o obj/utils.o

parser.y: src/parser.y src/scanner.l
	@bison -d src/parser.y -o src/parser.tab.c
	@flex -o src/lex.yy.c src/scanner.l

assembler.o: parser.y obj/utils.o 
	@gcc obj/utils.o src/parser.tab.c src/lex.yy.c -o obj/assembler.o -lfl

main: assembler.o

clean : 
	@rm obj/* 