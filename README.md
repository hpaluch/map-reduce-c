WordCount using Map Reduce (POSIX threads) in plain C
=====================================================

Proof of Concept - how to do Map/Reduce like processing (word count)
using plain C with threads and queue.

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

"API" docs
==========
Invoke:

	make docs

To generate docs/api/html/index.html files.

Bugs
====

Note: there are currently some leaks when running
in threads mode (but outside this C code???)

