BIN := server
CFLAGS := -Wall -g -D_DEBUG
OBJ := server.o epoll.o
SRC := server.c epoll.c

all: $(BIN); mv $(BIN) debug/

$(BIN): $(OBJ)
	cc $(SRC) -o $@ -O2 $(CFLAGS) -o $(BIN)

$(OBJ): $(SRC)
	cc $(CFLAGS) -c $(SRC)
clean:
	$(RM) $(BIN) *.o

.PHONY: all
