CC      = gcc
CFLAGS  = -O2
LDFLAGS = -lpthread

all: gauss_sequential gauss_parallel

gauss_sequential: gaussianseq.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

gauss_parallel: gaussianpar.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f gauss_sequential gauss_parallel

.PHONY: all clean