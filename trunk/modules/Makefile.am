INCLUDES =			\
    -I$(top_srcdir)             \
    @PACKAGE_CFLAGS@		\
    @IAL_DEFINES@

libdir = $(moduledir)
	
lib_LTLIBRARIES =		\
    libial_acpi.la		\
    libial_evdev.la		\
    libial_toshiba.la


libial_toshiba_la_SOURCES = 	\
    libial_toshiba.c 		\
    libial_toshiba_main.c	\
    libial_toshiba_filter.c	\
    libial_toshiba_utils.c	\
    libial_toshiba.h

libial_evdev_la_SOURCES =	\
    libial_evdev.c		\
    libial_evdev_main.c      	\
    libial_evdev.h		\
    libial_evdev_keys.h		\
    libial_evdev_key_to_string.h

libial_acpi_la_SOURCES =	\
    libial_acpi.c		\
    libial_acpi_main.c		\
    libial_acpi.h

libial_toshiba_la_LIBADD = 	\
    ../libial/libial.la

libial_acpi_la_LIBADD = 	\
    ../libial/libial.la

libial_evdev_la_LIBADD = 	\
    ../libial/libial.la
