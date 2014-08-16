#===============================================================================
#		Makefile for bin2dec.c
#		Convert binary numbers to decimal
#===============================================================================

CC=gcc
PREFIX=/usr
FILES=bin2dec.c
OPTFLAGS=-g -Wall
OUTPUT=bin2dec
SRC=src
DOC=doc
MANPAGE=bin2dec.1.gz
OUTPUTDIR=$(PREFIX)/bin
MANPATH=$(PREFIX)/share/man/man1

all: $(SRC)/bin2dec.c
	$(CC) $(OPTFLAGS) -o $(OUTPUT) $(SRC)/$(FILES)

install:
	install $(OUTPUT) -D $(OUTPUTDIR)/$(OUTPUT)
	install $(DOC)/$(MANPAGE) -D $(MANPATH)/$(MANPAGE)

uninstall:
	rm -f $(OUTPUTDIR)/$(OUTPUT)
	rm -f $(MANPATH)/$(MANPAGE)

clean:
	rm -f $(OUTPUT)
