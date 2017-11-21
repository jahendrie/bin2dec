#===============================================================================
#		Makefile for bin2dec.c
#		Convert binary numbers to decimal
#===============================================================================

CC=gcc
PREFIX=/usr
FILES=bin2dec.c
#OPTFLAGS=-g -Wall
OPTFLAGS=-O3
OUTPUT=bin2dec
SRC=src
DOC=doc
MANPAGE=bin2dec.1.gz
OUTPUTDIR=$(PREFIX)/bin
MANPATH=$(PREFIX)/share/man/man1
DOCPATH=$(PREFIX)/share/doc/bin2dec
LICENSEPATH=$(PREFIX)/share/licenses/bin2dec

all: $(SRC)/bin2dec.c
	$(CC) $(OPTFLAGS) -o $(OUTPUT) $(SRC)/$(FILES)

install:
	install $(OUTPUT) -D $(OUTPUTDIR)/$(OUTPUT)
	install $(DOC)/$(MANPAGE) -D $(MANPATH)/$(MANPAGE)
	install README -D $(DOCPATH)/README
	install $(DOC)/CHANGES -D $(DOCPATH)/CHANGES
	install $(DOC)/LICENSE -D $(LICENSEPATH)/LICENSE

uninstall:
	rm -f $(OUTPUTDIR)/$(OUTPUT)
	rm -f $(MANPATH)/$(MANPAGE)
	rm -r $(DOCPATH)
	rm -r $(LICENSEPATH)

clean:
	rm -f $(OUTPUT)
