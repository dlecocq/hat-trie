set terminal png

set output "insert.png"
set title "Insertion Time"
set log xy
set yrange [1:10000]
set xrange [10:95000]
set xlabel "Items Inserted"
set ylabel "Ticks To Insert (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:2 with lines title 'array-hash', \
     'bench.dat' using 1:5 with lines title 'std::map', \
     'bench.dat' using 1:8 with lines title 'std::tr1::unordered_map'

set output "query.png"
set title "Query Time"
set log xy
set yrange [1:10000]
set xrange [10:95000]
set xlabel "Items Queried and Contained"
set ylabel "Ticks To Query (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:3 with lines title 'array-hash', \
     'bench.dat' using 1:6 with lines title 'std::map', \
     'bench.dat' using 1:9 with lines title 'std::tr1::unordered_map'

set output "random.png"
set title "Random Query Time"
set log xy
set yrange [1:10000]
set xrange [10:95000]
set xlabel "Items Queried and Contained"
set ylabel "Ticks To Query (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:4  with lines title 'array-hash', \
     'bench.dat' using 1:7  with lines title 'std::map', \
     'bench.dat' using 1:10 with lines title 'std::tr1::unordered_map'
