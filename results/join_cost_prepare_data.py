#cat Join-*.sca | grep -e NSig:max -e lookupHopsSig:mean | awk '{print $4;}' > join_cost.dat
#gnuplot join_cost.gnu


from subprocess import call
import glob
from pprint import pprint


## FIRST: Gather data
alldata_avg = {}
alldata_max = {}
alldata_min = {}
for fn in glob.glob('Join-*.sca'):
	with open(fn) as f:
		for line in f:
			
			if 'attr repetition' in line:
				repetition = int(line.split()[2])
			
			elif 'NSig:max' in line:
				N = int(line.split()[3])
			
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
				
pprint (alldata_avg)

## SECOND: Aggregate data
aggrdata_avg = {}
aggrdata_max = {}
aggrdata_min = {}

repetitions = alldata_avg.keys()	# repetition indexes
xs = alldata_avg[0].keys()			# x-axis points

for x in xs:
	try:
		aggrdata_avg[x] = float(sum([ alldata_avg[rep][x] for rep in repetitions ]))/len(repetitions)
		aggrdata_max[x] = float(sum([ alldata_max[rep][x] for rep in repetitions ]))/len(repetitions)
		aggrdata_min[x] = float(sum([ alldata_min[rep][x] for rep in repetitions ]))/len(repetitions)
	except:
		pass

#~ pprint (aggrdata)

## THIRD: Output data in data file
with open('join_cost_avg.dat', 'w') as f:
	for n,h in sorted(aggrdata_avg.iteritems(), key=lambda (n,u):n):
		print >> f, '%s\t%s' % (n,h)
with open('join_cost_max.dat', 'w') as f:
	for n,h in sorted(aggrdata_max.iteritems(), key=lambda (n,u):n):
		print >> f, '%s\t%s' % (n,h)
with open('join_cost_min.dat', 'w') as f:
	for n,h in sorted(aggrdata_min.iteritems(), key=lambda (n,u):n):
		print >> f, '%s\t%s' % (n,h)

## LAST: Call Gnuplot!
call(['gnuplot', 'join_cost.gnu'])
