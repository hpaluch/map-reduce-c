CFLAGS = -Wall -g -pthread `pkg-config glib-2.0 --cflags`
LDFLAGS = -g -pthread `pkg-config glib-2.0 --libs-only-L`
LDLIBS = `pkg-config glib-2.0 --libs-only-l`

# use 'make run RUN_THREADS=X' to override
# temporarily set to 0 (unless thread version is implemented)
RUN_THREADS = 1
# input file to count words - use 'make run WORDS_FILE=my_file.txt' to override
WORDS_FILE  = /var/cache/man/whatis

all : mapr_wc mtrace_mapr_wc

mapr_wc: mapr_wc.o
mtrace_mapr_wc: mtrace_mapr_wc.o

mapr_wc.o : mapr_wc.c

mtrace_mapr_wc.o : mapr_wc.c
	$(COMPILE.c) -DMAPR_WC_MTRACE $(OUTPUT_OPTION) $<

.PHONY: run
run: mapr_wc
	./mapr_wc $(WORDS_FILE) $(RUN_THREADS)

.PHONY: valgrind
valgrind: mapr_wc
	valgrind --leak-check=full ./mapr_wc $(WORDS_FILE) $(RUN_THREADS)

.PHONY: mtrace_run
mtrace_run: mtrace_mapr_wc
	MALLOC_TRACE=trace_mem.lst ./mtrace_mapr_wc $(WORDS_FILE) $(RUN_THREADS)
	mtrace mtrace_mapr_wc trace_mem.lst

.PHONY: clean
clean:
	rm -f mapr_wc mtrace_mapr_wc *.o *.lst

.PHONY: docs
docs:
	-rm -rf docs/api
	mkdir -p docs/api
	doxygen

