# To use:
#
# Run simulator once to generate a log_data.csv file.
# 
# Make sure that plot.gp is in the same folder as the file mentioned above.
#
# Run gnuplot plot.gp

reset session

set datafile separator ','
set terminal png size 1920,1080
set output "plot.png"
set grid
set offset graph 0,0.1,0.1,0
set xlabel "Time (ms)"
set multiplot layout 2,2 title "Deadlock"

    set title "Number of Consecutive Cycles Idle"  
    set ylabel "Cycles Idle" 
    plot for [i=1:ARG1] "output/log_data.csv" using 1:4 every ARG1::i title "Node-".i with lines lw 3

    set title "Number of State Changes" 
    set ylabel "Number of State Changes"
    plot for [i=1:ARG1] "output/log_data.csv" using 1:3 every ARG1::i title "Node-".i with lines lw 3

    set title "Number of Consecutive Queue Request"
    set ylabel "Queue Request"
    plot for [i=1:ARG1] "output/log_data.csv" using 1:5 every ARG1::i title "Node-".i with lines lw 3

unset multiplot
