CSC = mcs -unsafe -debug -nowarn:0169

INITIAL_CSFILES =					\
	$(srcdir)/Initial.cs			\
	$(srcdir)/Initial_dbus.cs		\
	$(srcdir)/Initial_def.cs		\
	$(srcdir)/Initial_tray.cs		\
	$(srcdir)/Initial_ui.cs			\
	$(srcdir)/CompatFileChooser.cs


ASSEMBLIES =				\
	-pkg:dbus-sharp			\
	-pkg:gtk-sharp			\
	-pkg:glade-sharp		\
	-pkg:gnome-sharp		\
	-r:Mono.Posix

RESOURCES = 				\
	-resource:$(srcdir)/ui/initial.glade,initial.glade \
	-resource:$(srcdir)/ui/ial.png,ial.png

initiallibdir = $(libdir)/initial
initiallib_DATA = initial.exe

bin_SCRIPTS = initial

initial.exe: $(INITIAL_CSFILES) ui/initial.glade
	$(CSC) -out:$@ $(CSFLAGS) $(INITIAL_CSFILES) $(ASSEMBLIES) $(RESOURCES)

all: initial.exe

EXTRA_DIST =					\
	$(INITIAL_CSFILES)			\
	ui/initial.glade

CLEANFILES =					\
	initial						\
	initial.exe

DISTCLEANFILES = 				\
	makefile
