import sys


for line in open(sys.argv[1]):
	term = line.strip('\n').split()
	if term[0] == 'inner' and term[1] == 'communication':
		print term[-1].split(':')[0] + ' ' + term[-1].split(':')[1]
		print term[-2].split(':')[0] + ' ' + term[-2].split(':')[1]
		print 
		print