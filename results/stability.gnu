set datafile separator "\t"

#unset key
#set key noenhanced
set key right bottom

#set key font ",18" spacing 3 samplen 10 box width 3 height 3
set style data lines
set grid
set xlabel 'Frequency of join (1 every x sec.)' font ",18"

#set xtics (2,3,4,5,10,50)
#set title "Average number of hops"
#set style data linespoints

set logscale x 10
#set logscale y

#set format y "10^{%L}"
set format x "10^{%L}"

#set xrange [2**5 : 2**14
#set xtics (2**5,2**6,2**7,2**8,2**9,2**10,2**11,2**12,2**13,2**14)

set xrange [*:*] reverse
#set yrange [0:*]

#set grid lt 0 lw 1

set term postscript enhanced color size 10,6
set output "stability.eps"

set ylabel 'Number of hops' font ",18"
plot 'stability.dat' using 1:2:3:4 title 'Hops' with yerrorlines lw 3 lc 1
set ylabel 'Lookup time (sec.)' font ",18"
plot 'stability.dat' using 1:8:9:10 title 'Time (sec.)' with yerrorlines lw 3 lc 2
set ylabel 'Stability' font ",18"
plot 'stability.dat' using ($1):5:6:7 title 'Stability' with yerrorlines lw 3 lc 3
#plot 'stability.dat' using 1:11 title 'Mean Concurrent joins' with lines lw 3 lc 4
