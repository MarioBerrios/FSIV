#!/bin/bash

cat << _end_ | gnuplot
set terminal epslatex
set output "knn-lbp.tex"

#Label and data format
set format y "%.2f" 
set decimalsign '.'

#Border frame format
set border 3
set xtics nomirror
set ytics nomirror
set xtics 1, 2, 30

#grid style
set style line 12 lc rgb '#808080' lt 0 lw 0.5
set grid back ls 12

#Label text
set xlabel "Parámetro K"
set ylabel "Precisión"

#Legend style
set key right top width -3 height 1 box lt -1 lw 1

#Line style
set style line 101 lw 1.5 lt rgb "#994455"
set style line 102 lw 1.5 lt rgb "#997700"
#set style line 103 lw 1.5 lt rgb "#004488"

plot 'knn_lbp' using 1:2 t "Train" with line ls 101, 'knn_lbp' using 1:3 t "Validación" with line ls 102 
#'knn_grey' using 1:4 t "Test" with line ls 103
_end_