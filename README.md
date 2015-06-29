WordCount using Map Reduce (POSIX threads) in plain C
=====================================================

Warning: it is work in progress - unfinished!

Setup
=====
On CentOS 7 install:

	yum install binutils gcc make

If you have no /var/cache/man/whatis file in system, try as root:

	/etc/cron.daily/makewhatis.cron

Compile & Run
=============

	make run

To diagnose memory leaks:

	make mtrace_run

Note: there are currently some leaks (but outside this C code???)

