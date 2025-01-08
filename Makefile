CFLAGS = $(shell pkg-config --cflags glib-2.0) -Wall 
LDFLAGS = $(shell pkg-config --libs glib-2.0) 

hello: 
	@echo "hello"

obj/read_obj_file.o: src/read_obj_file.c
	@gcc src/read_obj_file.c -o obj/read_obj_file

obj/obj_file.o: src/obj_file.c
	@gcc -c $(CFLAGS) src/obj_file.c -o obj/obj_file.o 

obj/routines.o: src/routines.c 
	@gcc -c $(CFLAGS) -c src/routines.c  -o obj/routines.o $(LDFLAGS)

obj/symbol-table.o: src/symbol-table.c
	@gcc -c $(CFLAGS) -c src/symbol-table.c -o obj/symbol-table.o  $(LDFLAGS)

obj/utils.o: src/utils.c 
	@gcc -c $(CFLAGS) src/utils.c -o obj/utils.o

parser.y: src/parser.y src/scanner.l
	@bison -d -Wcounterexamples src/parser.y -o src/parser.tab.c
	@flex -o src/lex.yy.c src/scanner.l

obj-dir: 
	@mkdir -p obj

assembler.o: obj-dir parser.y obj/utils.o  obj/symbol-table.o obj/routines.o obj/obj_file.o
	@gcc $(CFLAGS) src/parser.tab.c src/lex.yy.c obj/symbol-table.o obj/utils.o obj/routines.o obj/obj_file.o -o obj/assembler.o -lfl $(LDFLAGS)

main: assembler.o 

all : main obj/read_obj_file.o

assemble : 
	@./obj/assembler.o ./sample/prg.asm

clean : 
	@rm obj/* src/lex.yy.c src/*.tab.*