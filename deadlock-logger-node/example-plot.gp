reset session

set datafile separator ','
set terminal pngcairo size 1920, 1080
set grid
set xlabel "Time (Simulator Ticks)"
set offset graph 0,0.1,0.1,0
set key font ",18"
set xtics font ",18"
set ytics font ",18" 
set ylabel font ",20" offset -2.5,0
set xlabel font ",20"
set title font ",20"
set lmargin 13

set output "plot-idle.png"
set title "Number of Consecutive Cycles Idle"  
set ylabel "Cycles Idle" 
set key at graph 0.95, 1

#plot for [i=1:ARG1] "demo_data/log_data.csv" using 1:4 every ARG1::i title "Node-".i with lines lw 3
plot "demo_data/log_data.csv" using 1:4 every 3::0 title "Node-0" with lines lw 5 lt rgb "#0000ff" dashtype 1, "demo_data/log_data.csv" using 1:4 every 3::1 title "Node-1" with lines lw 5 lt rgb "#ff0067" dashtype 4, "demo_data/log_data.csv" using 1:4 every 3::2 title "Node-2" with lines lw 5 lt rgb "#ffa600" dashtype 5

set output "plot-state.png"
set key at graph 1, 0.95
set title "Number of State Changes" 
set ylabel "Number of State Changes"

#plot for [i=1:ARG1] "demo_data/log_data.csv" using 1:3 every ARG1::i title "Node-".i with lines lw 3
plot "demo_data/log_data.csv" using 1:3 every 3::0 title "Node-0" with lines lw 5 lt rgb "#0000ff" dashtype 1, "demo_data/log_data.csv" using 1:3 every 3::1 title "Node-1" with lines lw 5 lt rgb "#ff0067" dashtype 4, "demo_data/log_data.csv" using 1:3 every 3::2 title "Node-2" with lines lw 5 lt rgb "#ffa600" dashtype 5

set output "plot-request.png"
set key at graph 0.95, 1
set title "Number of Consecutive Queue Request"
set ylabel "Queue Request"

#plot for [i=1:ARG1] "demo_data/log_data.csv" using 1:5 every ARG1::i title "Node-".i with lines lw 3
plot "demo_data/log_data.csv" using 1:5 every 3::0 title "Node-0" with lines lw 5 lt rgb "#0000ff" dashtype 1, "demo_data/log_data.csv" using 1:5 every 3::1 title "Node-1" with lines lw 5 lt rgb "#ff0067" dashtype 4, "demo_data/log_data.csv" using 1:5 every 3::2 title "Node-2" with lines lw 5 lt rgb "#ffa600" dashtype 5