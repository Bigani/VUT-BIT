NAME=proj2
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS=-lrt -lpthread

$(NAME): proj2.c
	$(CC) $(CFLAGS) proj2.c -o $(NAME) $(LFLAGS)

clean:
	rm -f $(NAME)
