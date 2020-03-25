CC=gcc
CFLAGS=-c -g -Wall -std=c99
#LDFLAGS=-pthread

SOURCES=mi_mkfs.c ficheros_basicos.c leer_sf.c bloques.c ficheros.c escribir.c leer.c permitir.c
LIBRARIES=ficheros_basicos.o bloques.o ficheros.o
INCLUDES=ficheros_basicos.h bloques.h ficheros.h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

#mi_mkfs: mi_mkfs.o $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@
#leer_sf: leer_sf.o $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@


%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
