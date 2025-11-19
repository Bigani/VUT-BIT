
# Makefile
# Project IFJ19
#
# Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
# Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
# Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
# Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)

PROJ = parser
SOURCE	= expressions.c generator.c parser.c scanner.c stack.c strings.c symtable.c tokenStack.c list.c
CC	 = gcc
WARNINGS = -Wall -pedantic -Wextra
INPUT = txt.txt
OUTPUT = str.out

all: $(PROJ)

$(PROJ) : $(SOURCE)
	$(CC) -o $(PROJ) $(SOURCE) -lm -g

warnings:
	$(CC) -o $(PROJ) $(SOURCE) $(WARNINGS) -lm -g

clean:
	rm -f $(OBJS) $(OUT) $(PROJ)

run:
	make all
	./$(PROJ) < $(INPUT)

int:
	make all
	./$(PROJ) < $(INPUT) > $(OUTPUT)
	./ic19int $(OUTPUT)

test:
	./$(PROJ) < tests/test1.py > tests/test1.out
	cat tests/test1.ins
	./ic19int tests/test1.out < tests/test1.in
	./$(PROJ) < tests/test2.py > tests/test2.out
	cat tests/test2.ins
	./ic19int tests/test2.out < tests/test2.in
	./$(PROJ) < tests/test3.py > tests/test3.out
	cat tests/test3.ins
	./ic19int tests/test3.out
	./$(PROJ) < tests/test4.py > tests/test4.out
	./ic19int tests/test4.out
