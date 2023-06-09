CC = gcc
VPATH = src
OBJ_DIR = $(VPATH)/obj

DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
ifeq ($(OS),Windows_NT)
  BIN = VAD.exe
  LDFLAGS = -lPDCurses
else
  BIN = VAD
  LDFLAGS = -lncurses
endif
OBJ = main.o display.o string.o fifo.o init.o Word.o VarTable.o interp.o

all: $(BIN)

interp: src/obj/VarTable.o src/obj/Word.o src/obj/interp.o
	$(CC) $^ $(LDFLAGS) -o $@

$(BIN): $(patsubst %,$(OBJ_DIR)/%,$(OBJ))
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJ_DIR)/*.o && rm $(BIN)
