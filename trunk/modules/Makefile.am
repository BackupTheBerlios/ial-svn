INCLUDES =			\
    @PACKAGE_CFLAGS@		\
    @IAL_DEFINES@

libdir = $(moduledir)
	
lib_LTLIBRARIES =		\
    libial_asus.la		\
    libial_acpi.la		\
    libial_keyboard.la		\
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

libial_keyboard_la_SOURCES =	\
    libial_keyboard.c		\
    libial_keyboard_main.c      \
    libial_keyboard.h

libial_acpi_la_SOURCES =	\
    libial_acpi.c		\
    libial_acpi_main.c		\
    libial_acpi.h

libial_toshiba_la_LIBADD = 	\
    ../libial/libial.la

libial_acpi_la_LIBADD = 	\
    ../libial/libial.la

libial_keyboard_la_LIBADD = 	\
    ../libial/libial.la
