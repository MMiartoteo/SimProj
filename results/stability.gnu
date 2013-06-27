set encoding iso_8859_1
set datafile separator "\t"

unset border

set bmargin 4.7
set lmargin 10

set style data lines
set grid

set xlabel 'Frequenza dei join' font ",24" offset 0,-1.1

set format y "%.2f"

set xtics font ',24' offset 0,-0.5
set ytics font ',24'

set logscale x 10

set format x "10^{%L}"

set xrange [*:*] reverse

set term postscript enhanced color size 10,6

set output "freq-hops.eps"
set ylabel 'Latenza media' font ",24" offset -1.4
plot 'stability.dat' using 1:2:3:4 notitle with yerrorlines lw 6 lc 1

set output "freq-time.eps"
set ylabel 'Tempo medio (sec.)' font ",24" offset -1.4
plot 'stability.dat' using 1:8:9:10 notitle with yerrorlines lw 6 lc 2

set output "freq-stability.eps"
set ylabel 'Stabilità' font ",24" offset -1.4
plot 'stability.dat' using 1:5:6:7 notitle with yerrorlines lw 6 lc 3

set output "freq-percLongLinks.eps"
set ylabel 'Percentuale dei long link' font ",24" offset -1.4
plot 'stability.dat' using 1:11:12:13 notitle with yerrorlines lw 6 lc 4

set output "freq-NS.eps"
set ylabel 'Numero di nodi nella rete' font ",24" offset -1.4
plot 'stability.dat' using 1:14:15:16 notitle with yerrorlines lw 6 lc 4
