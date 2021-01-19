# Gnuplot script for plotting data in file "random.dat"
set terminal png
set output "graph.png"

set title "Page replacement using random, optimal, LRU and clock policies"


set key right center

set xlabel "frms in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "random.dat" u 1:2 w linespoints title "random", \
     "optimal.dat" u 1:2 w linespoints title "optimal",\
     "lru.dat" u 1:2 w linespoints title "LRU",\
     "clock.dat" u 1:2 w linespoints title "clock",\
