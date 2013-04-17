import sys
from pprint import pprint


with open(sys.argv[1]) as f:
	
	vecinfo = {}
	data = {}
	
	for l in f:
		
		try:
			if 'vector' in l:
				
				#pid = int(l.split('[')[1].split(']')[0])
				datum = l.split()[3].split(':')[0]
				vid = int(l.split()[1])
				
				#print pid, datum, vid
				
				if not vid in vecinfo: vecinfo[vid] = {}
				#vecinfo[pid][datum] = vid
				vecinfo[vid] = datum
			
			elif len(l.split()) == 4:
				
				vid = int(l.split()[0])
				time = float(l.split()[2])
				val = float(l.split()[3])
				
				#print vid
				
				if not time in data: data[time] = {}
				data[time][vecinfo[vid]] = val
		except:
			print >> sys.stderr, l
			raise

#pprint (vecinfo)
#pprint (data)



# Compute averages
data2 = {}
for time, vals in data.items():
	n = vals['NSig']
	hops = vals['lookupHopsSig']
	
	if not n in data2: data2[n] = []
	data2[n].append(hops)



for n,all_hops in data2.items():
	mean_hops = sum(all_hops)/len(all_hops)
	print '%s\t%s' % (n, mean_hops)


#for time, vals in sorted(data.items(), key=lambda (k,v): k):
#	print '%s\t%s\t%s' % (time, vals['NSig'], vals['lookupHopsSig'])








