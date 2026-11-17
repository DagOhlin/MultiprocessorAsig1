CC      = gcc
CFLAGS  = -O2
LDFLAGS = -lpthread

all: gauss_sequential gauss_parallel qsort_sequential qsort_parallel

gauss_sequential: gaussianseq.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

gauss_parallel: gaussianpar.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

qsort_sequential: qsortseq.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

qsort_parallel: qsortpar.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f gauss_sequential gauss_parallel qsort_sequential qsort_parallel

.PHONY: all clean