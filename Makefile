CFLAGS = -Wall -g -pthread
LDFLAGS = -g -pthread

all : mapr_wc

mapr_wc: mapr_wc.o

mapr_wc.o : mapr_wc.c

.PHONY: run
run: mapr_wc
	./mapr_wc 2

.PHONY: clean
clean:
	rm -f mapr_wc mapr_wc.o

