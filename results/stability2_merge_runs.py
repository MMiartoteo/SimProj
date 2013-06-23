import math
import scipy as sp
import numpy as np
from scipy import stats
from subprocess import call
import glob
from pprint import pprint


#allowed_x = [50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650]

#allowed_x = range(100, 10501, 100)
#allowed_x = [1, 5, 10, 20, 30, 40, 50, 100, 150, 200, 250, 300,  350, 400, 450, 500, 1000, ]
#allowed_x = range(0, 10000, 10)
allowed_x = sorted([-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9] + range(10, 10001, 30)) #[-1, 0, 10, 50, 100, 200, 400, 600, 1000]
allowed_x = [ 2**i for i in range(0,14) ] #[-1, 0, 5, 10, 50, 100, 200, 400, 600, 800, 1000, 1200, 1400]

## FIRST: Gather data from each run (I don't know why they are called "repetitions" by Omnet...)
# Dictionaries indexed by repetition number
repentions_hops = {}
repentions_time = {}
repentions_stab_mean = {}	# Aggregates H (hops) and N (net size) ==> 1.0 - (H/N)
repentions_stab_min = {}	# Aggregates H (hops) and N (net size) ==> 1.0 - (H/N)
repentions_stab_max = {}	# Aggregates H (hops) and N (net size) ==> 1.0 - (H/N)
for fn in glob.glob('Stability2-*.sca'):	# For each repetition file
	with open(fn) as f:
		for line in f:
			if 'NSSig:mean' in line:
				ingoings = int(float(line.split()[3]))
				#ingoings = min(ingoings - ingoings%10, ingoings + ingoings%10)
				
				for i in range(1, len(allowed_x)-1):
					if allowed_x[i-1]+((allowed_x[i]-allowed_x[i-1])/2) < ingoings <= allowed_x[i]+((allowed_x[i+1]-allowed_x[i])/2):
						ingoings = allowed_x[i]
						break
				else:
					print ingoings
					raise Exception()
		
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
			
			elif 'lookupTimeSig:mean' in line:
				time = float(line.split()[3])
				
				if math.isnan(time): continue
				
				if repetition not in repentions_time:
					repentions_time[repetition] = {}
				repentions_time[repetition][ingoings] = time
			
			elif 'lookupStabilitySig:mean' in line:
				stab = float(line.split()[3])
				
				if math.isnan(stab): continue
				
				if repetition not in repentions_stab_mean:
					repentions_stab_mean[repetition] = {}
				repentions_stab_mean[repetition][ingoings] = stab
			
			elif 'lookupStabilitySig:min' in line:
				stab = float(line.split()[3])
				
				if math.isnan(stab): continue
				
				if repetition not in repentions_stab_min:
					repentions_stab_min[repetition] = {}
				repentions_stab_min[repetition][ingoings] = stab
			
			elif 'lookupStabilitySig:max' in line:
				stab = float(line.split()[3])
				
				if math.isnan(stab): continue
				
				if repetition not in repentions_stab_max:
					repentions_stab_max[repetition] = {}
				repentions_stab_max[repetition][ingoings] = stab

print 'hops'
pprint (repentions_hops)
print 'stab'
pprint (repentions_stab_mean)
print 'time'
pprint (repentions_time)

## SECOND: Aggregate data & Compute Confidence Intervals
# Dictionaries indexed by x-points
aggr_hops = {}
aggr_time = {}
aggr_stab_mean = {}
aggr_stab_min = {}
aggr_stab_max = {}

confidence_hops = {}
confidence_time = {}
confidence_stab_mean = {}
confidence_stab_min = {}
confidence_stab_max = {}

repetitions = repentions_hops.keys()	# repetition indexes
xs = repentions_hops[0].keys()			# x-axis points
print repetitions
print xs

for x in xs:
	# Only consider x points that are present in every alldata_* input
	try:
		hops_array = np.array([ repentions_hops[rep][x] for rep in repetitions if x in repentions_hops[rep] ])
		print 'hops ok'
		time_array = np.array([ repentions_time[rep][x] for rep in repetitions if x in repentions_time[rep] ])
		print 'time ok'
		stab_array_mean = np.array([ repentions_stab_mean[rep][x] for rep in repetitions if x in repentions_stab_mean[rep] ])
		stab_array_min  = np.array([ repentions_stab_min[rep][x] for rep in repetitions if x in repentions_stab_min[rep] ])
		stab_array_max  = np.array([ repentions_stab_max[rep][x] for rep in repetitions if x in repentions_stab_max[rep] ])
		print 'stab ok'
	
	except:
		print 'error'
	
	else:
		n, min_max, mean, var, skew, kurt = stats.describe(hops_array)
		std=math.sqrt(var)
		aggr_hops[x] = mean
		confidence_hops[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(time_array)
		std=math.sqrt(var)
		aggr_time[x] = mean
		confidence_time[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(stab_array_mean)
		std=math.sqrt(var)
		aggr_stab_mean[x] = mean
		confidence_stab_mean[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(stab_array_min)
		std=math.sqrt(var)
		aggr_stab_min[x] = mean
		confidence_stab_min[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)
		
		n, min_max, mean, var, skew, kurt = stats.describe(stab_array_max)
		std=math.sqrt(var)
		aggr_stab_max[x] = mean
		confidence_stab_max[x] = stats.norm.interval(1-0.05,loc=mean,scale=std)


## CONFIDENCE INTERVAL (based on Normal distribution... wft!!)
#avg_s = np.array(hops_array)

print 'hops'
pprint (aggr_hops)
print 'stab'
pprint (aggr_stab_mean)
print 'time'
pprint (aggr_time)


## THIRD: Output aggregated data in .dat files for Gnuplot (or other)
with open('stability2.dat', 'w') as f:
	#for x,h in sorted(aggr_hops.iteritems(), key=lambda (x,h):x):
	for x in sorted(aggr_hops):
		#stability = 1. - (aggr_hops[x]/float(x))
		print >> f, ('{:.20f}\t'*16).format(
			x,
			aggr_hops[x], confidence_hops[x][0], confidence_hops[x][1],
			aggr_stab_mean[x], confidence_stab_mean[x][0], confidence_stab_mean[x][1],
			aggr_time[x], confidence_time[x][0], confidence_time[x][1],
			
			aggr_stab_min[x], confidence_stab_min[x][0], confidence_stab_min[x][1],
			aggr_stab_max[x], confidence_stab_max[x][0], confidence_stab_max[x][1],
		)
#~ with open('join_cost_max.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_max.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])
#~ with open('join_cost_min.dat', 'w') as f:
	#~ for x,h in sorted(aggrdata_min.iteritems(), key=lambda (x,h):x):
		#~ print >> f, '%s\t%s\t%s\t%s' % (x, h, confidence_avg[x][0], confidence_avg[x][1])


## LAST: Call Gnuplot!
call(['gnuplot', 'stability2.gnu'])
#call(['epstopdf', '--autorotate=All', 'stability2.eps'])
