CC = gcc
CFLAGS = -Iinc -Wall -Wextra
SRC = src/hypervisor.c src/vm.c
OBJ = $(SRC:.c=.o)
BIN = bin/hypervisor

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
