CC=g++
FLAGS=-Wall -Werror -pedantic -std=c++11
FILE=ims
OBJECTS=
ARGS = -p 5 -g CZ

all: $(OBJECTS)
	$(CC) $(FLAGS) ims.cpp -o $(FILE)

$(OBJECTS): functions.h

run:	${FILE}
		./${FILE} ${ARGS}

clean:
	rm ims
