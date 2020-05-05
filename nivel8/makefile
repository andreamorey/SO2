CC=gcc
CFLAGS=-c -g -Wall -std=c99
#LDFLAGS=-pthread

SOURCES=mi_mkfs.c ficheros_basico.c leer_sf.c bloques.c directorios.c ficheros.c escribir.c leer.c permitir.c truncar.c mi_ls.c mi_mkdir.c mi_stat.c mi_chmod.c mi_touch.c
LIBRARIES=ficheros_basico.o bloques.o ficheros.o directorios.o
INCLUDES=ficheros_basico.h bloques.h ficheros.h directorios.h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar mi_ls mi_mkdir mi_stat mi_chmod mi_touch
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
