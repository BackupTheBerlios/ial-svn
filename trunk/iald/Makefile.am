configdir=$(sysconfdir)/ial

INCLUDES =					\
	-I$(top_srcdir)				\
	@PACKAGE_CFLAGS@			\
	@IAL_DEFINES@

config_DATA=                                    \
	iald.conf

sbin_PROGRAMS = iald

iald_SOURCES = 					\
	iald.h					\
	iald.c					\
	iald_conf.c				\
	iald_conf.h				\
	iald_dbus.c				\
	iald_dbus.h				\
	iald_mod.c				\
	iald_mod.h

iald_LDADD =					\
	@PACKAGE_LIBS@				\
	../libial/libial.la			\
	-ldl 
