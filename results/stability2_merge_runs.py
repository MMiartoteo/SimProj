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
for fn in glob.glob('Stability2-*.sca'):	# For each repetition file
	with open(fn) as f:
		for line in f:
			if 'inGoingSizeSig:mean' in line:
				ingoings = int(float(line.split()[3]))
				ingoings -= ingoings%2
		
		f.seek(0)
		for line in f:
			
			if 'attr repetition' in line:
				repetition = int(line.split()[2])
			
			#elif 'attr iterationvars ' in line:
			#	
			
			elif 'lookupHopsSig:mean' in line:
				hops = float(line.split()[3])
				
				if math.isnan(hops): continue
				
				if repetition not in repentions_hops:
					repentions_hops[repetition] = {}
				repentions_hops[repetition][ingoings] = hops
				
				#N = 32. + float(line.split()[1].split('[')[1].split(']')[0]) + 1.
				
				#if repetition not in repentions_stab:
				#	repentions_stab[repetition] = {}
				#repentions_stab[repetition][freq] = 1.0 - (hops/N)
			
			elif 'lookupStabilitySig:mean' in line:
				stab = float(line.split()[3])
				
				if math.isnan(stab): continue
				
				if repetition not in repentions_stab:
					repentions_stab[repetition] = {}
				repentions_stab[repetition][ingoings] = stab
			
			elif 'lookupTimeSig:mean' in line:
				time = float(line.split()[3])
				
				if math.isnan(time): continue
				
				if repetition not in repentions_time:
					repentions_time[repetition] = {}
				repentions_time[repetition][ingoings] = time

print 'hops'
pprint (repentions_hops)
print 'stab'
pprint (repentions_stab)
print 'time'
pprint (repentions_time)

## SECOND: Aggregate data & Compute Confidence Intervals
# Dictionaries indexed by x-points
aggr_hops = {}
aggr_stab = {}
aggr_time = {}
confidence_hops = {}
confidence_stab = {}
confidence_time = {}

repetitions = repentions_hops.keys()	# repetition indexes
xs = repentions_hops[0].keys()			# x-axis points
print repetitions
print xs

for x in xs:
	# Only consider x points that are present in every alldata_* input
	try:
		hops_array = np.array([ repentions_hops[rep][x] for rep in repetitions ])
		print 'hops ok'
		stab_array = np.array([ repentions_stab[rep][x] for rep in repetitions ])
		print 'stab ok'
		time_array = np.array([ repentions_time[rep][x] for rep in repetitions ])
		print 'time ok'
	
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


## CONFIDENCE INTERVAL (based on Normal distribution... wft!!)
#avg_s = np.array(hops_array)

print 'hops'
pprint (aggr_hops)
print 'stab'
pprint (aggr_stab)
print 'time'
pprint (aggr_time)


## THIRD: Output aggregated data in .dat files for Gnuplot (or other)
with open('stability.dat', 'w') as f:
	#for x,h in sorted(aggr_hops.iteritems(), key=lambda (x,h):x):
	for x in sorted(aggr_hops):
		stability = 1. - (aggr_hops[x]/float(x))
		print >> f, '{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}\t{:.20f}'.format(
			x,
			aggr_hops[x], confidence_hops[x][0], confidence_hops[x][1],
			aggr_stab[x], confidence_stab[x][0], confidence_stab[x][1],
			aggr_time[x], confidence_time[x][0], confidence_time[x][1],
		)
#~ with open('join_cost_max.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_max.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])
#~ with open('join_cost_min.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_min.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])


## LAST: Call Gnuplot!
call(['gnuplot', 'stability2.gnu'])
