import sys
import os


print 'set terminal pdf'
print 'set title "D2D pair number"'
print 'set xlabel Ue number'
print 'set ylabel D2D pair number'
print 'set output D2D pair number.pdf'
print 'plot "-" with lines title D2D pair number'

for rt, dirs, filenames in os.walk(sys.argv[1]):
	for files in filenames:
		fp = open(rt + '/' + files)

		d2dpairs = 0

		for line in fp:
			line = line.strip('\n')
			term = line.split('\t')
			if term[0] == '0.628':
				if term[3] == '0' and term[4] == '0':
					d2dpairs+=1

		print files[-3:] + " " + str(d2dpairs)
				
