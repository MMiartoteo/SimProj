SimProj
=======


## Test Join

#### Descrizione
Ci sono 10 run (con seed diversi).
Ogni run consta di 11 simulazioni. Per un totale di 11*10 simulazioni.
Ogni simulazione è fatta su una rete di grandezza N in {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768}.
Alla fine, l'obiettivo è di fare la media tra i run corrispondenti alle 10 simulazioni con stesso N.

In ogni simulazione, dato un certo N, questo è ciò che accade dentro la simulazione:

1. Viene creata una rete di N nodi statici
2. Un peer dinamico entra ed esce 1000 volte, ed ogni volta che entra o esce emette il numero di hop che la lookup ci ha messo per farlo entrare o uscire
3. Queste emit() vengono raccolte dal motore Omnet automaticamente, e scritte nel file .sca in forma di min, max e mean
4. Dopo queste 1000 entrate/uscite il peer si ferma, e la simulazione termina per fine di messaggi ed eventi

#### Lancia il test Join in batch:

	./Symphony -c Join -u Cmdenv

#### Raccogli i dati e plotta il grafico
I dati dei run sono scritti su .sca separati. Vanno quindi aggregati, come detto sopra, per valori di N uguali. Fare le seguenti operazioni per aggregare i run e generare il grafico finale:

	cd results/
	python join_cost_merge_runs.py

Questo crea un file join_cost.eps dentro results/
