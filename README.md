# IMS
Autoři: Jakub Svoboda (xsvobo0z), Michael Schmid (xschmi08
Návod pro spuštění:
1. Stáhněte soubor se vstupními daty pomocí příkazu "make download" nebo
	ručně z jedné z těchto adres:
	http://www.stud.fit.vutbr.cz/~xschmi08/data.csv
	http://www.stud.fit.vutbr.cz/~xsvobo0z/data.csv
	a umístěte jej do kořenového adresáře projektu
2. Pro spuštění se vzorovými vstupními daty použijte příkaz "make run"
3. Pro spuštění programu s jinými daty jej přelože pomocí příkazu "make"
	a spusťte příkazem ./ims -f file -x X -y Y -a angle -V Velocity -s samplingRate –d dimensions
	kde file je cesta k souboru z bodu 1,
	X a Y jsou souřadnice epicentra požáru,
	angle je úhel větru (0° = jižní vítr, úhly číslovány po směru hodinových ručiček)
	Velocity je rychlost větru
	samplingRate je počet kroků simulace po kterém se generuje obrázek aktuálního stavu
	a dimensions je počet buněk automatu (výška a šířka, pro výše zmíněný vstupní soubor je
	tato hodnota rovna 1500).
	Všechny argumenty jsou povinné a kromě argumentu file očekávají jako vstup přirozená čísla.