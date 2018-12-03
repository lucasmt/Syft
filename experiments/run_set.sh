BASENAME=$1
N=$2
MONA=mona

for (( I=0; I < N; I++ ))
do
    ../build/bin/ltlf2fol ltlf_formulas/${BASENAME}_$I.ltl NNF > fol_formulas/${BASENAME}_$I.mona
    $MONA -w -xw fol_formulas/${BASENAME}_$I.mona > dfa_games/${BASENAME}_$I.dfa

    $MONA -gw fol_formulas/${BASENAME}_$I.mona > dfa_games/${BASENAME}_$I.dot
    dot -Tpdf dfa_games/${BASENAME}_$I.dot -o dfa_games/${BASENAME}_$I.pdf

    echo $I
done

../build/bin/bdd dfa_games/${BASENAME}_ $N dfa_games/${BASENAME}.part
