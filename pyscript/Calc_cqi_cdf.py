


f = open('Dat/160ue_d2dlink_cqi')

cqiList = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
lineNum = 0

for line in f:
	cqi = int(line.strip('\n').split(' ')[-1])
	cqiList[cqi] += 1
	lineNum += 1


cqi_sum = 0
for i in range(0, 16):
	cqi_sum += cqiList[i]
	print cqi_sum * 1.0 / lineNum





