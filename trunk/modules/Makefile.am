INCLUDES =			\
    @PACKAGE_CFLAGS@		\
    @IAL_DEFINES@

libdir = $(moduledir)
	
lib_LTLIBRARIES =		\
    libial_asus.la		\
    libial_acpi.la		\
    libial_event.la		\
    libial_toshiba.la


libial_toshiba_la_SOURCES = 	\
    libial_toshiba.c 		\
    libial_toshiba_main.c	\
    libial_toshiba_filter.c	\
    libial_toshiba_utils.c	\
    libial_toshiba.h

libial_asus_la_SOURCES =	\
    libial_asus.c		\
    libial_asus.h

libial_event_la_SOURCES =	\
    libial_event.c		\
    libial_event_main.c      	\
    libial_event.h

libial_acpi_la_SOURCES =	\
    libial_acpi.c		\
    libial_acpi_main.c		\
    libial_acpi.h

libial_toshiba_la_LIBADD = 	\
    ../libial/libial.la

libial_acpi_la_LIBADD = 	\
    ../libial/libial.la

libial_event_la_LIBADD = 	\
    ../libial/libial.la
