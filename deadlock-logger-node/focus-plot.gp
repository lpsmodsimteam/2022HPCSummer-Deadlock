reset session

set datafile separator ','
set terminal pngcairo size 1920, 1080
set output "plot-full.png"
set grid
set xlabel "Time (ms)"
set offset graph 0,0.1,0.1,0
set key font ",12"
set xtics font ",12"
set ytics font ",12"
set ylabel font ",16"
set xlabel font ",16"
set title font ",16"

set title "Number of Consecutive Cycles Idle"  
set ylabel "Cycles Idle" 
#plot for [i=1:ARG1] "output/log_data.csv" using 1:4 every ARG1::i title "Node-".i with lines lw 3

#plot "output/log_data.csv" using 1:4 every 3::0 title "Node-0" with lines lw 3 lt rgb "#0000ff" dashtype 1, "output/log_data.csv" using 1:4 every 3::1 title "Node-1" with lines lw 3 lt rgb "#ff0067" dashtype 4, "output/log_data.csv" using 1:4 every 3::2 title "Node-2" with lines lw 3 lt rgb "#ffa600" dashtype 5

set title "Number of State Changes" 
set ylabel "Number of State Changes"
#plot for [i=1:ARG1] "output/log_data.csv" using 1:3 every ARG1::i title "Node-".i with lines lw 3

plot "output/log_data.csv" using 1:3 every 3::0 title "Node-0" with lines lw 3 lt rgb "#0000ff" dashtype 1, "output/log_data.csv" using 1:4 every 3::1 title "Node-1" with lines lw 3 lt rgb "#ff0067" dashtype 4, "output/log_data.csv" using 1:4 every 3::2 title "Node-2" with lines lw 3 lt rgb "#ffa600" dashtype 5

set title "Number of Consecutive Queue Request"
set ylabel "Queue Request"
#plot for [i=1:ARG1] "output/log_data.csv" using 1:5 every ARG1::i title "Node-".i with lines lw 3

#plot "output/log_data.csv" using 1:5 every 3::0 title "Node-0" with lines lw 3 lt rgb "#0000ff" dashtype 1, "output/log_data.csv" using 1:4 every 3::1 title "Node-1" with lines lw 3 lt rgb "#ff0067" dashtype 4, "output/log_data.csv" using 1:4 every 3::2 title "Node-2" with lines lw 3 lt rgb "#ffa600" dashtype 5