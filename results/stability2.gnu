set encoding iso_8859_1
set datafile separator "\t"

unset border

set key right bottom font ",18"

set style data lines
set grid
set xlabel 'Numero di peer (compresi quelli in fase di joining)' font ",18"

set logscale x 2
#set logscale y

set xrange [*:*]
#set yrange [0:*]

#set grid lt 0 lw 1

set term postscript enhanced color size 10,6

set output "norelink-conc-stability2-hops.eps"
set ylabel 'Latenza media' font ",18"
plot 'stability2.dat' using 1:2:3:4 notitle with yerrorlines lw 6 lc 1

set output "norelink-conc-stability2-time.eps"
set ylabel 'Tempo medio (sec.)' font ",18"
plot 'stability2.dat' using 1:8:9:10 notitle with yerrorlines lw 6 lc 2

set output "norelink-conc-stability2-stab.eps"
#set xrange [0:1000]
set yrange [0.7:1]
set ylabel 'Stabilità' font ",18"
stab(x) = 1.0 - ((1./2.)*(log(x)**2))/x
stab2(x) = 1.0 - (log(x)/x)
plot stab(x) title "Stabilità teorica" lw 6 lc 4,\
	'stability2.dat' using ($1):14 title 'Stabilità massima' lw 6 lc 5,\
	'stability2.dat' using ($1):5:6:7    title 'Stabilità media' with yerrorlines lw 6 lc 3,\
	'stability2.dat' using ($1):11 title 'Stabilità minima' lw 6 lc 1
     

