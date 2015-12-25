SHELL	= /bin/sh
COMP	= g++
ARCH	= ar

FLAGS	= --std=c++11

TARGET	= libhuffmancoder.a
SOURCES	= $(shell echo *.cpp)
HEADERS	= $(shell echo *.h)
OBJECTS	= $(SOURCES:.cpp=.o)

PREFIX	= $(DESTDIR)/usr/local
BINDIR	= $(PREFIX)/bin
INCDIR	= $(PREFIX)/include/huffmancoder
LIBDIR	= $(PREFIX)/lib

$(TARGET):
	$(COMP) $(SOURCES) $(FLAGS) -c
	$(ARCH) rvc $(TARGET) $(OBJECTS)

install: $(TARGET)
	cp $(TARGET) -d $(LIBDIR)
	mkdir $(INCDIR)
	cp $(HEADERS) -d $(INCDIR)

uninstall:
	-rm $(LIBDIR)/$(TARGET)
	-rm -rf $(INCDIR)

clean:
	-rm -f $(OBJECTS)
	-rm -f gmon.out

distclean: clean
	-rm -f $(TARGET)

.PHONY : uninstall clean distclean
