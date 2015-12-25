SHELL	= /bin/sh
COMP	= g++
ARCH	= ar

FLAGS	= --std=c++11

SRCDIR	= source
OUTDIR	= out
OUTDMMY	= $(OUTDIR)/.ignoreThis.dummy.out
PREFIX	= $(DESTDIR)/usr/local
BINDIR	= $(PREFIX)/bin
INCDIR	= $(PREFIX)/include/huffmancoder
LIBDIR	= $(PREFIX)/lib

TARGET	= libhuffmancoder.a
SOURCES	= $(shell echo $(SRCDIR)/*.cpp)
HEADERS	= $(shell echo $(SRCDIR)/*.h)
OBJECTS	= $(patsubst $(SRCDIR)/%.cpp,$(OUTDIR)/%.o,$(SOURCES))
OUT		= $(OUTDIR)/$(TARGET)

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp $(OUTDMMY)
	$(COMP) -c $(FLAGS) -o $@ $<

$(OUT): $(OBJECTS)
	$(ARCH) rvc $(OUT) $^

setup:

$(OUTDMMY):
	@echo Creating output directory
	mkdir -p $(OUTDIR)
	touch $@
	@echo This is a dummy file for Makefile to know that the out directory was created. It will be removed by distclean along with the directory. > $@

install: $(OUT)
	cp $(OUT) $(LIBDIR)/$(TARGET)
	mkdir -p $(INCDIR)
	cp $(HEADERS) -d $(INCDIR)

uninstall:
	-rm $(LIBDIR)/$(TARGET)
	-rm -rf $(INCDIR)

clean:
	-rm -f $(OBJECTS)
	-rm -f gmon.out

distclean: clean
	-rm -f $(OUT)
	-rm -f $(OUTDMMY)
	-rmdir $(OUTDIR)

.PHONY : install uninstall clean distclean
