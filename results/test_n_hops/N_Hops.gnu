set datafile separator "\t"
#unset key
#set key noenhanced
#set key left top
set key font ",18" spacing 3 samplen 10 box width 3 height 3
set style data lines
set grid
set xlabel 'N. of peers' font ",18"
set ylabel 'N. of hops' font ",18"
#set xtics (2,3,4,5,10,50)
#set title "Number of messages for different TTL values"
#set style data linespoints


#set logscale x
#set logscale y


#set format y "10^{%T}"
#set format x "10^{%T}"

#set xtics rotate by -90

set term postscript enhanced color size 10,6

#set xrange [0.4      : 10**10]
set yrange [0      : 80]
#set xrange [1      : 10**4]
#set yrange [1      : *]
#set grid lt 0 lw 1

set output "N_Hops.eps"

#plot 'N_Hops.dat' using 1:2 title 'N', \
#     'N_Hops.dat' using 1:3 title 'Hops'

plot 'N_Hops.dat' using 1:2 title 'Hops' smooth unique
