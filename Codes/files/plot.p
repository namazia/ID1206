set terminal png
set output "plot.png"

plot 'freq.dat' using 1:2 with boxes
