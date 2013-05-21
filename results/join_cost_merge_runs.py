import math
import scipy as sp
import numpy as np
from scipy import stats
from subprocess import call
import glob
from pprint import pprint


## FIRST: Gather data from each run (I don't know why they are called "repetitions" by Omnet...)
# Dictionaries indexed by repetition number
alldata_avg = {}
alldata_max = {}
alldata_min = {}
for fn in glob.glob('Join-*.sca'):	# For each repetition file
	with open(fn) as f:
		for line in f:
			
			if 'attr repetition' in line:
				repetition = int(line.split()[2])
			
			elif 'attr iterationvars ' in line:
				N = int(line.split()[2].split(',')[0].split('=')[1])
			
			#elif 'NSig:max' in line:
			#	N = int(line.split()[3])
			
			elif 'lookupHopsSig:mean' in line:
				hops = float(line.split()[3])
				
				if repetition not in alldata_avg: alldata_avg[repetition] = {}
				alldata_avg[repetition][N] = hops
			
			elif 'lookupHopsSig:max' in line:
				hops = float(line.split()[3])
				
				if repetition not in alldata_max: alldata_max[repetition] = {}
				alldata_max[repetition][N] = hops
			
			elif 'lookupHopsSig:min' in line:
				hops = float(line.split()[3])
				
				if repetition not in alldata_min: alldata_min[repetition] = {}
				alldata_min[repetition][N] = hops
#pprint (alldata_avg)


## SECOND: Aggregate data & Compute Confidence Intervals
# Dictionaries indexed by x-points
aggrdata_avg = {}
aggrdata_max = {}
aggrdata_min = {}
confidence_avg = {}
confidence_max = {}
confidence_min = {}

repetitions = alldata_avg.keys()	# repetition indexes
xs = alldata_avg[0].keys()			# x-axis points
for x in xs:
	# Only consider x points that are present in every alldata_* input
	try:
		runs_avg = np.array([ alldata_avg[rep][x] for rep in repetitions ])
		runs_max = np.array([ alldata_max[rep][x] for rep in repetitions ])
		runs_min = np.array([ alldata_min[rep][x] for rep in repetitions ])
	
	except:
		pass
	
	else:
		n, min_max, mean, var, skew, kurt = stats.describe(runs_avg)
		std=math.sqrt(var)
		confidence_avg[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		aggrdata_avg[x] = mean
		#print mean, sum(confidence_avg[x])/2.
		
		n, min_max, mean, var, skew, kurt = stats.describe(runs_max)
		std=math.sqrt(var)
		confidence_max[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		aggrdata_max[x] = mean
		#assert mean == sum(confidence_max[x])/2.
		
		n, min_max, mean, var, skew, kurt = stats.describe(runs_min)
		std=math.sqrt(var)
		confidence_min[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		aggrdata_min[x] = mean
		#assert mean == sum(confidence_min[x])/2.
		
		
		#aggrdata_avg[x] = float(sum(runs_avg))/len(repetitions)
		#aggrdata_max[x] = float(sum([ alldata_max[rep][x] for rep in repetitions ]))/len(repetitions)
		#aggrdata_min[x] = float(sum([ alldata_min[rep][x] for rep in repetitions ]))/len(repetitions)
#~ pprint (aggrdata_avg)



## CONFIDENCE INTERVAL (based on Normal distribution... wft!!)
#avg_s = np.array(runs_avg)



## THIRD: Output aggregated data in .dat files for Gnuplot (or other)
with open('join_cost_avg.dat', 'w') as f:
	for x,h in sorted(aggrdata_avg.iteritems(), key=lambda (x,h):x):
		print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])
with open('join_cost_max.dat', 'w') as f:
	for x,h in sorted(aggrdata_max.iteritems(), key=lambda (x,h):x):
		print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])
with open('join_cost_min.dat', 'w') as f:
	for x,h in sorted(aggrdata_min.iteritems(), key=lambda (x,h):x):
		print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])


## LAST: Call Gnuplot!
call(['gnuplot', 'join_cost.gnu'])
