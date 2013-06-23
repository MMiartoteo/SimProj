set datafile separator "\t"

unset key

set style data lines
set grid

set bmargin 4.7

set xlabel 'Numero di peer' font ",24" offset 0,-1.1
set ylabel 'Latenza media' font ",24"

set xtics font ',24' offset 0,-0.5
set ytics font ',24'

set logscale x 2

set format x "2^{%L}"

set xrange [2**5 : 2**14]
set xtics (2**5,2**6,2**7,2**8,2**9,2**10,2**11,2**12,2**13,2**14)

unset border

set term postscript enhanced color size 10,6
set output "join_cost.eps"

plot 'join_cost_avg.dat' using 1:2:3:4 title "simulazione" with yerrorlines lw 4
	 #(log(x)**2)*(1.0/3) title "limite teorico"
     #'join_cost_max.dat' using 1:2:3:4 title 'Max' with yerrorlines, \
     #'join_cost_min.dat' using 1:2:3:4 title 'Min' with yerrorlines
