import sys
import os

CqiToDataRate = [
	0, 
	25.59, 
	39.38, 
	63.34, 
	101.07, 
	147.346, 
	197.53, 
	248.07, 
	321.57, 
	404.26,
	458.72,
	558.15,
	655.59,
	759.93,
	859.35,
	933.19
]


# gnufile = open(str(radius) + 'm_' + str(ueNum) +'ue_position', 'w')


print 'set terminal pdf'
# gnufile.write('set term jpeg size 800,600\n')
# print 'set output "' + str(radius) + 'm_' + str(ueNum) +'ue_position.pdf"\n')
print 'set title "D2D throughput"'
# print 'set size square\n')
# print 'set key outside\n')
print 'set Xlabel Ue number'
print 'set Ylabel D2D throughput'
print 'plot "-" with lines title D2D throughput'


for rt, dirs, filenames in os.walk(sys.argv[1]):
	for files in filenames:
		fp = open(rt + '/' + files)

		throughput = 0

		for line in fp:
			line = line.strip('\n')
			term = line.split('\t')
			if term[0] == '0.628':
				if term[3] == '0' and term[4] == '0':
					throughput += CqiToDataRate[int(term[-2])] * int(term[-1])

		print files[-3:] + " " + str(throughput)