set encoding iso_8859_1

unset border

set key right bottom

set style data lines
set grid
set xlabel 'n' font ",18"

set logscale x 2

set xrange [*:*]
#set yrange [0:*]

#set grid lt 0 lw 1

stab1(x) = 1.0 - ((1./1.)*(log(x)**2))/x
stab2(x) = 1.0 - ((1./2.)*(log(x)**2))/x
stab4(x) = 1.0 - ((1./4.)*(log(x)**2))/x
stab8(x) = 1.0 - ((1./8.)*(log(x)**2))/x
stab16(x) = 1.0 - ((1./16.)*(log(x)**2))/x

set term postscript enhanced color size 10,6

set output "stab-teorica-1.eps"
set xrange [1:50]
#set yrange [0.7:1]
set ylabel 'Stabilità teorica' font ",18"
plot stab16(x) title "k=16" lw 6 lc 5,\
	stab8(x) title "k=8" lw 6 lc 4,\
	stab4(x) title "k=4" lw 6 lc 3,\
	stab2(x) title "k=2" lw 6 lc 2,\
	stab1(x) title "k=1" lw 6 lc 1
     
set output "stab-teorica-2.eps"
set xrange [1:2**10]
#set yrange [0.8:1]
set ylabel 'Stabilità teorica' font ",18"
set samples 100000
plot stab16(x) title "k=16" lw 6 lc 5,\
	stab8(x) title "k=8" lw 6 lc 4,\
	stab4(x) title "k=4" lw 6 lc 3,\
	stab2(x) title "k=2" lw 6 lc 2,\
	stab1(x) title "k=1" lw 6 lc 1

