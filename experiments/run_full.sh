for FILE in Acacia/*.ltl
do

    #FILE=Acacia/load_0.ltl

    FILENAME=$(basename $FILE .ltl)

    cp $FILE ltlf_formulas/${FILENAME}_0.ltl
    cp Acacia/${FILENAME}.part dfa_games/${FILENAME}.part
    
    ../build/bin/ltlf2fol ltlf_formulas/${FILENAME}_0.ltl NNF > fol_formulas/${FILENAME}_0.mona
    mona -w -xw fol_formulas/${FILENAME}_0.mona > dfa_games/${FILENAME}_0.dfa

    mona -gw fol_formulas/${FILENAME}_0.mona > dfa_games/${FILENAME}_0.dot
    dot -Tpdf dfa_games/${FILENAME}_0.dot -o dfa_games/${FILENAME}_0.pdf

    ../build/bin/bdd dfa_games/${FILENAME}_ 1 dfa_games/${FILENAME}.part
    
done
