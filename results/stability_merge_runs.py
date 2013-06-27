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
repentions_stab = {}	# Aggregates H (hops) and N (net size) ==> 1.0 - (H/N)
repentions_time = {}
repentions_NS = {}
repentions_perclonglinks = {}

for fn in glob.glob('Stability-*.sca'):	# For each repetition file
	with open(fn) as f:
		
		for line in f:
			
			if 'attr repetition' in line:
				repetition = int(line.split()[2])
			
			elif 'attr iterationvars ' in line:
				freq = float(line.split()[2].split(',')[0].split('=')[1])
			
			elif 'lookupHopsSig:mean' in line:
				hops = float(line.split()[3])
				
				if repetition not in repentions_hops:
					repentions_hops[repetition] = {}
				repentions_hops[repetition][freq] = hops
			
			elif 'lookupStabilitySig:mean' in line:
				stab = float(line.split()[3])
				
				if repetition not in repentions_stab:
					repentions_stab[repetition] = {}
				repentions_stab[repetition][freq] = stab
			
			elif 'lookupTimeSig:mean' in line:
				time = float(line.split()[3])
				
				if repetition not in repentions_time:
					repentions_time[repetition] = {}
				repentions_time[repetition][freq] = time
			
			elif 'lookupNSSig:mean' in line:
				NS = float(line.split()[3])
				
				if repetition not in repentions_NS:
					repentions_NS[repetition] = {}
				repentions_NS[repetition][freq] = NS
			
			elif 'lookupPercLongLinksSig:mean' in line:
				percLongLinks = float(line.split()[3])
				
				if repetition not in repentions_perclonglinks:
					repentions_perclonglinks[repetition] = {}
				repentions_perclonglinks[repetition][freq] = percLongLinks

print 'hops'
pprint (repentions_hops)
print 'stab'
pprint (repentions_stab)
print 'time'
pprint (repentions_time)
#~ print 'ingongs'
#~ pprint (repentions_ingoing)


## SECOND: Aggregate data & Compute Confidence Intervals
# Dictionaries indexed by x-points
aggr_hops = {}
aggr_stab = {}
aggr_time = {}
aggr_NS = {}
aggr_perclonglinks = {}
#~ aggr_ingoing = {}
confidence_hops = {}
confidence_stab = {}
confidence_time = {}
confidence_NS = {}
confidence_perclonglinks = {}
#~ confidence_ingoing = {}

repetitions = repentions_hops.keys()	# repetition indexes
xs = repentions_hops[0].keys()			# x-axis points
print repetitions
print xs

for x in xs:
	# Only consider x points that are present in every alldata_* input
	try:
		hops_array = np.array([ repentions_hops[rep][x] for rep in repetitions ])
		#print 'hops ok'
		stab_array = np.array([ repentions_stab[rep][x] for rep in repetitions ])
		#print 'stab ok'
		time_array = np.array([ repentions_time[rep][x] for rep in repetitions ])
		NS_array = np.array([ repentions_NS[rep][x] for rep in repetitions ])
		#print 'time ok'
		perclonglinks_array = np.array([ repentions_perclonglinks[rep][x] for rep in repetitions ])
		#~ ingoing_array = np.array([ repentions_ingoing[rep][x] for rep in repetitions ])
	
	except:
		print 'error'
	
	else:
		n, min_max, mean, var, skew, kurt = stats.describe(hops_array)
		std=math.sqrt(var)
		aggr_hops[x] = mean
		confidence_hops[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(stab_array)
		std=math.sqrt(var)
		aggr_stab[x] = mean
		confidence_stab[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(time_array)
		std=math.sqrt(var)
		aggr_time[x] = mean
		confidence_time[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(NS_array)
		std=math.sqrt(var)
		aggr_NS[x] = mean
		confidence_NS[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(perclonglinks_array)
		std=math.sqrt(var)
		aggr_perclonglinks[x] = mean
		confidence_perclonglinks[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)


print 'hops'
pprint (aggr_hops)
pprint (confidence_hops)
print 'stab'
pprint (aggr_stab)
pprint (confidence_stab)
print 'time'
pprint (aggr_time)
pprint (confidence_time)


## THIRD: Output aggregated data in .dat files for Gnuplot (or other)
with open('stability.dat', 'w') as f:
	for x in sorted(aggr_hops):
		print >> f, ('{:.20f}\t'*16).format(
			x,
			aggr_hops[x], confidence_hops[x][0], confidence_hops[x][1],
			aggr_stab[x], confidence_stab[x][0], confidence_stab[x][1],
			aggr_time[x], confidence_time[x][0], confidence_time[x][1],
			aggr_perclonglinks[x], confidence_perclonglinks[x][0], confidence_perclonglinks[x][1],
			aggr_NS[x], confidence_NS[x][0], confidence_NS[x][1],
		)

## LAST: Call Gnuplot!
call(['gnuplot', 'stability.gnu'])
