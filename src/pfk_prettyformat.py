#!/usr/bin/python

import sys
names=sys.argv[2].split(',')

f=open(sys.argv[1])
r=[]
for l in f.readlines():
	bf=int(l.split(',')[0])
	count=int(l.split(',')[1])
	s=[names[i] for i in xrange(len(names)) if bf&(2**i)]
	r.append([len(s),count,s,bf])
r.sort(reverse=True)

for x in r:
	bf=x[3]
	tc=0
	for y in r:
		if y[3]&bf==bf: tc+=y[1]
	print "%d,\'%s\',%d,%d" % (x[0]," ".join(x[2]),x[1],tc)

