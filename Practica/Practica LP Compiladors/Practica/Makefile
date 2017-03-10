RM = rm -f

ALLOBJS = *.o

ALLDRIVOBS = *.exe *.prova *.c *.h *.dlg

all: dist-clean lego.exe

allNew: clear all

lego.exe:
	antlr -gt lego.g
	dlg -ci parser.dlg scan.c
	g++ -std=c++11 -Wno-write-strings -o lego.exe lego.c scan.c err.c -I../Pccts/h
	antlr -gt legoGrid.g
	dlg -ci parser.dlg scan.c
	g++ -std=c++11 -Wno-write-strings -o legoGrid.exe legoGrid.c scan.c err.c -I../Pccts/h
	antlr -gt legoChiv.g
	dlg -ci parser.dlg scan.c
	g++ -std=c++11 -Wno-write-strings -o legoChiv.exe legoChiv.c scan.c err.c -I../Pccts/h

clear: clean dist-clean

clean:
	$(RM) $(ALLOBJS)

dist-clean: clean
	$(RM) $(ALLDRIVOBS)
