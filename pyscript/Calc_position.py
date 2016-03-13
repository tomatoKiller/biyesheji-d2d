
import sys

# ueNum = 160
# radius = 500

# d2dlinks = 28

# gnufile = open(str(radius) + 'm_' + str(ueNum) +'ue_position', 'w')


# gnufile.write('set terminal pdf\n')
# # gnufile.write('set term jpeg size 800,600\n')
# gnufile.write('set output "' + str(radius) + 'm_' + str(ueNum) +'ue_position.pdf"\n')
# gnufile.write('set title "UE distribution"\n')
# gnufile.write('set size square\n')
# gnufile.write('set key outside\n')
# gnufile.write('set parametric\n')
# gnufile.write('set trange [-pi:pi]\n')

# plotStr = 'plot "Dat/position" with points pt 7 ps 0.4 title "ue"'
# plotStr += ',' + str(radius) + '*sin(t),' + str(radius) + '*cos(t) lw 2 lc 4 title "radius=' + str(radius) + 'm"'
# # plotStr += ',50*sin(t),50*cos(t) lw 2 lc 10 title "radius=50m"'

# # plotStr += ',for [i=0:0] "Dat/d2dlinkPosition" index i with lines lc 1 lw 4 title "d2d link"'

# # plotStr += ',for [i=1:' + str(d2dlinks - 1) +'] "Dat/d2dlinkPosition" index i with lines lc 1 lw 2 notitle'

# plotStr += '\n'

# gnufile.write(plotStr)


for line in open(sys.argv[1]):
	term = line.strip('\n').split()
	if term[0] == '+0.0ns':
		# gnufile.write(term[4][2:-2] + ' ' + term[5][2:-2] + '\n')
		print term[4][2:-2] + ' ' + term[5][2:-2]

		
