set size square
set key outside
set parametric
set trange [-pi:pi]
plot "Dat/500m_160ue_position" with points pt 7 ps 0.4 title "ue", 500*sin(t),500*cos(t) lw 2 lc 4 title "radius=500m"