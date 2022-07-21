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
set multiplot layout 2,2 title "Deadlock"

    set title "Node Idle" 
    #plot "log_data.csv" using 1:4 every 3 with lines  
    plot for [i=1:ARG1] "log_data.csv" using 1:4 every ARG1::i with lines

    set title "Node Status"
    #plot "log_data.csv" using 1:3 every 3 with lines 
    plot for [i=1:ARG1] "log_data.csv" using 1:3 every ARG1::i with lines

    set title "Number of Request"
    #plot "log_data.csv" using 1:5 every 3 with lines 

    plot for [i=1:ARG1] "log_data.csv" using 1:5 every ARG1::i with lines
unset multiplot
