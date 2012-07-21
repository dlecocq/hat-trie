set terminal png size 1200, 800

set output "insert.png"
set title "Insertion Time Relative to std::tr1::unordered_map"
set log x
set yrange [0:5]
set xrange [50:95000]
set xlabel "Items Inserted"
set ylabel "Relative Time (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:($2/$8) with lines title 'array-hash', \
     'bench.dat' using 1:($5/$8) with lines title 'std::map', \
     'bench.dat' using 1:($8/$8) with lines title 'std::tr1::unordered_map'

set output "query.png"
set title "Query Time Relative to std::tr1::unordered_map"
set log x
set yrange [0:5]
set xrange [50:95000]
set xlabel "Items Queried and Contained"
set ylabel "Relative Time (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:($3/$9) with lines title 'array-hash', \
     'bench.dat' using 1:($6/$9) with lines title 'std::map', \
     'bench.dat' using 1:($9/$9) with lines title 'std::tr1::unordered_map'

set output "random.png"
set title "Random Query Time Relative to std::tr1::unordered_map"
set log x
set yrange [0:5]
set xrange [50:95000]
set xlabel "Items Queried and Contained"
set ylabel "Relative Time (Best of 10 Consecutive Runs)"
plot 'bench.dat' using 1:($4/$10)  with lines title 'array-hash', \
     'bench.dat' using 1:($7/$10)  with lines title 'std::map', \
     'bench.dat' using 1:($10/$10) with lines title 'std::tr1::unordered_map'
