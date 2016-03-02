
import sys

gnufile = open('1000m_100ue_position', 'w')


gnufile.write('set terminal pdf\n')
gnufile.write('set output "1000m_100ue_position.pdf"\n')
gnufile.write('set title "UE distribution"\n')
gnufile.write('set size square\n')
gnufile.write('set key outside\n')
gnufile.write('set parametric\n')
gnufile.write('set trange [-pi:pi]\n')
gnufile.write('plot "-" with points pt 3 title "ue",1000*sin(t),1000*cos(t) lw 2 lc 4 title "radius=1000m", 50*sin(t),50*cos(t) lw 2 lc 10 title "radius=50m"\n')


for line in open(sys.argv[1]):
	term = line.strip('\n').split()
	if term[0] == '+0.0ns':
		gnufile.write(term[4][2:-2] + ' ' + term[5][2:-2] + '\n')
		# gnufile.write('\n')
		
