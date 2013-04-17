cat Join-*.sca | grep -e NSig:max -e lookupHopsSig:mean | awk '{print $4;}' > join_cost.dat
#gnuplot join_cost.gnu
