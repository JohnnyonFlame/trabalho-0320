TARGET=trabalho_0318.exe
ifeq ($(BINARIO),1)
	SRC=main.c registro_binario.c misc.c
else
	SRC=main.c token.c registro.c misc.c
endif
CFLAGS=-I.
CC=gcc

OBJ = $(patsubst %.c, %.o, ${SRC})

all: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)
