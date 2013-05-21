import math
import scipy as sp
import numpy as np
from scipy import stats
from subprocess import call
import glob
from pprint import pprint


## FIRST: Gather data from each run (I don't know why they are called "repetitions" by Omnet...)
# Dictionaries indexed by repetition number
repentions_hops = {}
repentions_time = {}
for fn in glob.glob('Join-*.sca'):	# For each repetition file
	with open(fn) as f:
		for line in f:
			
			if 'attr repetition' in line:
				repetition = int(line.split()[2])
			
			elif 'attr iterationvars ' in line:
				N = int(line.split()[2].split(',')[0].split('=')[1])
			
			elif 'lookupHopsSig:mean' in line:
				hops = float(line.split()[3])
				
				if repetition not in repentions_hops:
					repentions_hops[repetition] = {}
				repentions_hops[repetition][N] = hops
			
			elif 'lookupTimeSig:mean' in line:
				time = float(line.split()[3])
				
				if repetition not in repentions_time:
					repentions_time[repetition] = {}
				repentions_time[repetition][N] = time



## SECOND: Aggregate data & Compute Confidence Intervals
# Dictionaries indexed by x-points
aggr_hops = {}
aggr_time = {}
confidence_hops = {}
confidence_time = {}

repetitions = repentions_hops.keys()	# repetition indexes
xs = repentions_hops[0].keys()			# x-axis points
for x in xs:
	# Only consider x points that are present in every alldata_* input
	try:
		hops_array = np.array([ repentions_hops[rep][x] for rep in repetitions ])
		time_array = np.array([ repentions_time[rep][x] for rep in repetitions ])
	
	except:
		pass
	
	else:
		n, min_max, mean, var, skew, kurt = stats.describe(hops_array)
		std=math.sqrt(var)
		confidence_hops[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		aggr_hops[x] = mean
		
		n, min_max, mean, var, skew, kurt = stats.describe(time_array)
		std=math.sqrt(var)
		confidence_time[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		aggr_time[x] = mean
		


## CONFIDENCE INTERVAL (based on Normal distribution... wft!!)
#avg_s = np.array(hops_array)


pprint (aggr_hops)
pprint (aggr_time)


## THIRD: Output aggregated data in .dat files for Gnuplot (or other)
with open('stability.dat', 'w') as f:
	#for x,h in sorted(aggr_hops.iteritems(), key=lambda (x,h):x):
	for N in sorted(aggr_hops):
		stability = 1. - (aggr_hops[N]/float(N))
		print >> f, '%s\t%s' % (N, stability)
#~ with open('join_cost_max.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_max.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])
#~ with open('join_cost_min.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_min.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])


## LAST: Call Gnuplot!
call(['gnuplot', 'stability.gnu'])
