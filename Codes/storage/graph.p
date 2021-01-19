set terminal png
set output "plot.png"
set xrange[-30:500]
set yrange[0:*]
set xtics 64
set boxwidth 10
plot 'read.dat' u 1:3:2:6:5 with candlesticks title "read", \
     'write.dat' u 1:3:2:6:5 with candlesticks title "write",\
     'write_woflag.dat' u 1:3:2:6:5 with candlesticks title "write with out O_DSYNC"
