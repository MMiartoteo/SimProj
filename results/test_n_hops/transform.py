import sys
from pprint import pprint

data = {}

for n in range(100):
	
	data[n] = []
	
	with open('Join-%d.vec' % n) as f:
		
		vecinfo = {}
		
		
		
		
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
					
					data[n] += [val]
					
					#if not time in data: data[time] = {}
					#data[time][vecinfo[vid]] = val
			except:
				print >> sys.stderr, l
				#raise

#pprint (vecinfo)
#pprint (data)



for n,vals in data.items():
	for val in vals:
		print '%s\t%s' % (n, val)



exit()

# Compute averages
data2 = {}
for time, vals in data.items():
	#n = vals['NSig']
	hops = vals['lookupHopsSig']
	
	if not 32 in data2: data2[32] = []
	data2[32].append(hops)



for n,all_hops in data2.items():
	mean_hops = sum(all_hops)/len(all_hops)
	print '%s\t%s' % (n, mean_hops)


#for time, vals in sorted(data.items(), key=lambda (k,v): k):
#	print '%s\t%s\t%s' % (time, vals['NSig'], vals['lookupHopsSig'])








