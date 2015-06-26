CFLAGS = -Wall -g -pthread
LDFLAGS = -g -pthread

all : mapr_wc mtrace_mapr_wc

mapr_wc: mapr_wc.o
mtrace_mapr_wc: mtrace_mapr_wc.o

mapr_wc.o : mapr_wc.c

mtrace_mapr_wc.o : mapr_wc.c
	$(COMPILE.c) -DMAPR_WC_MTRACE $(OUTPUT_OPTION) $<

.PHONY: run
run: mapr_wc
	./mapr_wc 2

.PHONY: mtrace_run
mtrace_run: mtrace_mapr_wc
	MALLOC_TRACE=trace_mem.lst ./mtrace_mapr_wc 2
	mtrace mtrace_mapr_wc trace_mem.lst

.PHONY: clean
clean:
	rm -f mapr_wc mtrace_mapr_wc *.o *.lst

