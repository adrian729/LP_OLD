RM = rm -f

ALLOBJS = *.o

ALLDRIVOBS = *.exe *.prova

all: dist-clean example0.exe example1.exe

allNew: clear all

example0.exe:
	antlr example0.g
	dlg parser.dlg scan.c
	gcc -Wno-write-strings -o example0.exe example0.c scan.c err.c -I/home/brutus729/Documents/FIB/LP/Pccts/h

example1.exe:
	antlr -gt example1.g
	dlg -ci parser.dlg scan.c
	g++ -Wno-write-strings -o example1.exe example1.c scan.c err.c -I/home/brutus729/Documents/FIB/LP/Pccts/h

clear: clean dist-clean

clean:
	$(RM) $(ALLOBJS)

dist-clean: clean
	$(RM) $(ALLDRIVOBS)
