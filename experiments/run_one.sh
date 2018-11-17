BASENAME=$1

for FILE in ltlf_formulas/$BASENAME*.ltl
do

    FILENAME=$(basename $FILE .ltl)

    ../build/bin/ltlf2fol ltlf_formulas/${FILENAME}.ltl NNF > fol_formulas/${FILENAME}.mona
    mona -w -xw fol_formulas/${FILENAME}.mona > dfa_games/${FILENAME}.dfa

    mona -gw fol_formulas/${FILENAME}.mona > dfa_games/${FILENAME}.dot
    dot -Tpdf dfa_games/${FILENAME}.dot -o dfa_games/${FILENAME}.pdf

    ../build/bin/bdd dfa_games/${BASENAME}_ 1 dfa_games/${BASENAME}.part
    
done
