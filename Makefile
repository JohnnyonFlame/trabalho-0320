TARGET=trabalho_0318.exe
SRC=main.c token.c registro.c misc.c
CFLAGS=-I.
CC=gcc

OBJ = $(patsubst %.c, %.o, ${SRC})

all: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)
