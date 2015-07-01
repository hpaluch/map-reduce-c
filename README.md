WordCount using Map Reduce (POSIX threads) in plain C
=====================================================

Warning: it is work in progress - unfinished!

Setup
=====
On CentOS 7 install:

	yum install binutils gcc make

Install this project requirements:

	yum install glib2-devel valgrind doxygen

If you have no /var/cache/man/whatis file in system, try as root:

	/etc/cron.daily/makewhatis.cron

Compile & Run
=============

	make run

To diagnose memory leaks try:

	make mtrace_run

or

	make valgrind

Bugs
====

Note: there are currently some leaks when running
in threads mode (but outside this C code???)

