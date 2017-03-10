1. Estructura:
	-"Pccts" (amb els includes necessaris).
	-Practica: Makefile i .g -> lego.g (codi practica), legoGrid.g (executa la practica i, a demes, pinta els diferents nivells de la graella i la graella en ultim lloc).
	-inputs_pract: input_pract.in -> codi de la practica (copiat del pdf). input_pract_correct.in -> codi de la practica canviant el "HEIGHT(B4) > 1" de TOWER10 per "HEIGHT(B4) > 0", ja que no hi ha blocs de tamany 0 i mai entraria al bucle.
	-inputs_prova: diferents inputs de prova.

2. Us Makefile:
	make: per compilar els 3 .g, no hauria de donar problemes (amb pccts i c++11).
	make dist-clean: neteja els fitxers creats al fer make.

3. Comentaris/aclariments practica (propis):
	
	Explicacio implementacio (segons el que he entes que s'havia de fer, pero crec que podia portar a alguna confusio):

	HEIGHT(BX): altura mes alta, dins dels recuadres de BX, A PARTIR DE BX (inclos ell mateix).
		-> EXEMPLE: B1 amb mida 4x4, a sobre te B2 de mida 4x4. Sobre B2 hi ha B3 amb mida 2x3 i sobre B3 un bloc B4 de 2x2.
		HEIGHT(B2) = 3 (la seva + la de B3 + la de B4).

	PLACE: no cal que estigui a nivell de terra, mentre estigui anivellat permeto el PLACE.

	MOVE: el mateix que el PLACE, a part mou el BLOC i TOTS els que tingui per sobre, i nomes si es possible (ja que si comparteix blocs superiors a ell amb altres blocs que estiguin al seu voltant no podra). No cal que es desplaci a la mateixa altura a la que estaba.
		->EXEMPLE: no podriem fer move de B0 si tinguessim (cada nombre representa un bloc diferent, amb el 0 sent B0 i "-" cap bloc):
			A nivell de B0:
			0 0 1 1
			0 0 1 1
			Sobre B0 i B1:
			- 2 2 -
			- 2 2 -
		El bloc B2 esta sobre B0 i sobre B1, per tant no podrem moure B0

	ASSIG (=): ID nomes apunta a un BLOC simple (si te coses per sobre, al fer un PUSH d'ell sobre un altre o un MOVE es mouran amb ell).
	Per tant BZ = BX PUSH BY, per exemple, possara BX (i tot el que tingui a sobre) sobre BY si es pot, despres BZ apuntara al bloc BY.

	FITS(ID, X, Y, L): mira que hi capiga algo al nivell L del bloc ID (no mira des del terre, mira des d'on comença el bloc). El bloc mateix ja compta com a nivell 1 (per a que funcioni com diu al pdf i quedi B2 amb HEIGHT = 2).
	