set ylabel "throughput(Kbps)"
set xlabel "UE number"
plot "Lte_Ul_throughput_50RB" with linespoints lw 1 pt 8 ps 2  lc rgb "black" title "Lte Throughput", "10rb_80m_10dbm/system_throughput" with linespoints lw 1 ps 2  lc rgb "black" title "Lte+D2D Throughput"