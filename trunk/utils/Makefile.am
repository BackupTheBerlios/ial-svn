INCLUDES =					\
	@PACKAGE_CFLAGS@			\
	@IAL_DEFINES@

bin_PROGRAMS = ialmon iallatency

ialmon_SOURCES = 				\
	ialmon.h				\
	ialmon.c

iallatency_SOURCES = 				\
	latency.c				\
	ialmon.h

ialmon_LDADD =					\
	@PACKAGE_LIBS@				\
	../libial/libial.la

iallatency_LDADD = 				\
	@PACKAGE_LIBS@				\
	../libial/libial.la
