
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

for rt, dirs, filenames in os.walk(sys.argv[1]):
	for files in filenames:
		fp = open(rt + '/' + files)

		throughput = 0
		slot = 0
		line_num = 0
		begin = False

		for line in fp:
			line = line.strip('\n')
			term = line.split('\t')
	
			if term[0] == '0.41' or term[0] == '0.411' :
				begin = True

			if begin == False:
				continue

			if not (term[3] == '0' and term[4] == '0'):
				if len(term) == 8:
					throughput += CqiToDataRate[int(term[-2])] * int(term[-1])
				elif len(term) == 10:
					throughput += CqiToDataRate[int(term[-2])] * int(term[-1])
					throughput += CqiToDataRate[int(term[-4])] * int(term[-3])

					
				if slot != term[0]:
					line_num+=1
					slot = term[0]


		print files[-3:],  throughput / line_num  
