#!/bin/bash

cat << _end_ | gnuplot
set terminal epslatex
set output "svm-k2-G05.tex"

#Label and data format
set format y "%.2f" 
set decimalsign '.'

#Border frame format
set border 3
set xtics nomirror
set ytics nomirror
set logscale x

#grid style
set style line 12 lc rgb '#808080' lt 0 lw 0.5
set grid back ls 12

#Label text
set xlabel "Parámetro C"
set ylabel "Precisión"

#Legend style
set key right center width -3 height 1 box lt -1 lw 1

#Line style
set style line 101 lw 1.5 lt rgb "#994455"
set style line 102 lw 1.5 lt rgb "#997700"
#set style line 103 lw 1.5 lt rgb "#004488"

plot 'svm_k2_hist' using 1:3 t "Train" with line ls 101, \
    'svm_k2_hist' using 1:4 t "Validación" with line ls 102 
#'knn_grey' using 1:4 t "Test" with line ls 103
_end_