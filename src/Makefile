# Compiler options
CC = gcc
CFLAGS = -lfl

# Paths
EXEC_DIR = execs
BISON_OUT = $(EXEC_DIR)/parser.tab.c
FLEX_OUT = $(EXEC_DIR)/lex.yy.c

# Targets
all: $(EXEC_DIR) $(BISON_OUT) $(FLEX_OUT)
	$(CC) -o $(EXEC_DIR)/assembler.o $(BISON_OUT) $(FLEX_OUT) $(CFLAGS)

$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

$(BISON_OUT): parser.y
	bison -d parser.y -o $(BISON_OUT)

$(FLEX_OUT): scanner.l
	flex -o $(FLEX_OUT) scanner.l

clean:
	rm -rf $(EXEC_DIR)/*.c $(EXEC_DIR)/*.h $(EXEC_DIR)/assembler.o
