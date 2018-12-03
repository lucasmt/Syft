L=$1

for I in {1..50}
do
	cp ../benchmarks/lilymulti/case_${L}_50/lily${I}.ltl ltlf_formulas/lily${I}.ltl
	cp ../benchmarks/lilymulti/case_${L}_50/lily${I}.part dfa_games/lily${I}.part
done
