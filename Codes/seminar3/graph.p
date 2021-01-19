# Gnuplot script for plotting data in file "test.dat"
set terminal png
set output "multi.png"

set title "Execution time to perform 20.000 productions with 2 threads "

set key right center

set xlabel "Number of productions"
set ylabel "Time (ms)"

set xrange [0:20000]
set yrange [0:1000]

set key right bottom
set key invert

plot "test.dat" using 1:3 w linespoints title "pthread", "test.dat" using 1:2 w linespoints title "green"
