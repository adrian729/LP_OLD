RM = rm -f

ALLOBJS = *.o

ALLDRIVOBS = *.exe *.prova

all: dist-clean lego.exe

allNew: clear all

lego.exe:
	antlr -gt lego.g
	dlg -ci parser.dlg scan.c
	g++ -std=c++11 -Wno-write-strings -o lego.exe lego.c scan.c err.c -I/home/brutus729/Documents/FIB/LP/Pccts/h

clear: clean dist-clean

clean:
	$(RM) $(ALLOBJS)

dist-clean: clean
	$(RM) $(ALLDRIVOBS)
