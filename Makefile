BIN := server
CFLAGS := -Wall -g -D_DEBUG
OBJ := server.o epoll.o
SRC := server.c epoll.c
OUT_DIR := debug

all: $(OUT_DIR) $(BIN); mv $(BIN) debug/

$(BIN): $(OBJ)
	cc $(SRC) -o $@ -O2 $(CFLAGS) -o $(BIN)

$(OBJ): $(SRC)
	cc $(CFLAGS) -c $(SRC)
clean:
	$(RM) $(BIN) *.o

$(OUT_DIR):
	mkdir debug
