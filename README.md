WordCount using Map Reduce (POSIX threads) in plain C
=====================================================

Warning: it is work in progress - unfinished!

Setup
=====
On CentOS 7 install:

	yum install binutils gcc make

Compile & Run
=============

	make run

To diagnose memory leaks:

	make mtrace_run

Note: there are currently some leaks (but outside this C code???)

