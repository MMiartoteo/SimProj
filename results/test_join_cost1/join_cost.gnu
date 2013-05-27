set datafile separator "\t"

#unset key
set key noenhanced
#set key left top

#set key font ",18" spacing 3 samplen 10 box width 3 height 3
set style data lines
set grid
set xlabel 'Number of peers' font ",18"
set ylabel 'Average number of hops' font ",18"

#set xtics (2,3,4,5,10,50)
#set title "Average number of hops"
#set style data linespoints

set logscale x 2
#set logscale y

#set format y "10^{%L}"
set format x "2^{%L}"

set xrange [2**5 : 2**14]
set xtics (2**5,2**6,2**7,2**8,2**9,2**10,2**11,2**12,2**13,2**14)

#set yrange [0.4      : 10**10]
#set xrange [1      : 10**4]
#set yrange [1      : *]
#set grid lt 0 lw 1

set term postscript enhanced color size 10,6
set output "join_cost.eps"

plot 'join_cost_avg.dat' using 1:2:3:4 title 'Mean' with yerrorlines, \
     'join_cost_max.dat' using 1:2:3:4 title 'Max' with yerrorlines, \
     'join_cost_min.dat' using 1:2:3:4 title 'Min' with yerrorlines
