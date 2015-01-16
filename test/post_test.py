#!/bin/python3
import csv
import sys
from statistics import mean
from operator import itemgetter
from itertools import groupby
from pprint import pprint as pp

# Edges; Needles; Granularity; Program; 1; 2; 4; 8; 16; 32; 64
# 0 	 1 			2  			3  		4  5  6  7  8   9   10

if(len(sys.argv) < 2):
	print("Usage: " + sys.argv[0] + " <test_results>.csv")
	exit(0)

datareader = csv.reader(open(sys.argv[1]), delimiter=";")

in_data = []
for row in datareader:
	in_data.append(row)

PAR_BINS = ['farm', 'map', 'farm-no-io'] 
BINS = [ 'seq' ] + PAR_BINS

GRAINS = ['300', '3000', '30000']
PAR_DEG = ['1', '2', '4', '8', '16', '32', '64']

def pos(pd):
	if pd == '1': return 4
	elif pd == '2' : return 5
	elif pd == '4' : return 6
	elif pd == '8' : return 7
	elif pd == '16' : return 8
	elif pd == '32' : return 9
	elif pd == '64' : return 10


E_NUM = ['100', '10000', '1M', '7M']
N_NUM = ['2' ,'100', '250']
INPUTS = []

for e in E_NUM:
	for n in N_NUM:
		INPUTS.append(e + 'x' + n)

data = list(map(lambda l : [l[0]+'x'+l[1]] + [l[2]] + [l[3].strip()] + list(map(float, l[4:])), in_data[1:]))

tc_seq = {}
data.sort(key=itemgetter(0))
t_types = groupby(data, itemgetter(0))
for tname, tdata in t_types:
	x = list(tdata)
	tc_seq[tname] = list(filter(lambda k : k[2] == 'seq', x))[0][4]


data.sort(key=itemgetter(2))
par_data = list(filter(lambda i : i[3] != 'seq', data))
par_progs = groupby(par_data, itemgetter(2))

avg_speedup_over_grains=[]
avg_scalab_over_grains=[]
for pname, pdata in par_progs:
	x = list(pdata)
	x.sort(key=itemgetter(0))
	test_types = groupby(x, itemgetter(0))
	for tname, tdata in test_types:
		y = list(tdata)
		times = list(zip(y[0][3:],y[1][3:],y[2][3:]))
		avgs = list(map(lambda x : mean(list(x)), times))
		tc_1 = avgs[0]
		scb = list(map(lambda x : "{0:.2f}".format(round(tc_1 / x,2)), avgs))
		spdp = list(map(lambda x : "{0:.2f}".format(round(tc_seq[tname] / x, 2)), avgs))
		avg_speedup_over_grains.append([pname,tname] + spdp)
		avg_scalab_over_grains.append([pname, tname] + scb)

csvout = csv.writer(sys.stdout, delimiter=";")
for r in avg_scalab_over_grains:
	csvout.writerow(['sclb'] + r)
for r in avg_speedup_over_grains:
	csvout.writerow(['spdp'] + r)