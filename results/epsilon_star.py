import sys, math

def STD(X):
	mu = float(sum(X)) / float(len(X))
	return math.sqrt( sum([ (x-mu)**2 for x in X ]) / float(len(X)) )



dat = [ [ float(f) for f in row.split('\t') if f != '' ] for row in open(sys.argv[1]).read().strip().split('\n') ]

POS = int(sys.argv[2])

S = [ r[POS] for r in dat ]

print S
print 2*STD(S)


