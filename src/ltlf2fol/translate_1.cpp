#include "ltlf2fol.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "Attr.hpp"
#include "AttrRanking.hpp"
#include "BucketElimination.hpp"
#include "JoinExpTree.hpp"

#include "../synthesis/VarPartition.h"
#include "LTLInterp.h"

#define MAXN 1000000
char in[MAXN];

using boost::to_upper_copy;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::set;
using std::string;
using std::vector;

int main (int argc, char ** argv)
{
  string StrLine;

  if (argc != 5)
  {
    printf("Usage: %s <input-file> <output-prefix> <format> <partition-file>",
	   argv[0]);
    
    return 0;
  }
  
  string input = argv[1];
  string outprefix = argv[2];
  string format = argv[3];
  string part_file = argv[4];
  
  ifstream myfile(input);
  if (!myfile.is_open()) //判断文件是否存在及可读
  {
    printf("unreadable file!");
    return -1;
  }

  vector<ltl_formula*> formulas;
  vector<jet::AttrSet> support_sets;

  VarPartition part = VarPartition::load(part_file);
  
  while (getline(myfile, StrLine))
  {
    //myfile.close(); //关闭文件
    strcpy (in, StrLine.c_str());
    //out << "#" << in << endl;

    ltl_formula *root = getAST (in);
    set<string> support = get_alphabet(root);
    vector<string> names(support.size());
    transform(support.begin(), support.end(), names.begin(),
	      [] (const string& name)
	      {
		return to_upper_copy(name);
	      });

    formulas.push_back(root);
    support_sets.push_back(part.from_names(names));
  }

  jet::AttrSet all_vars = part.env_vars().unionWith(part.sys_vars());
  jet::AttrRanking var_ranking = jet::AttrRanking::MCS(all_vars,
						       support_sets);
  jet::RankBasedClustering clustering(var_ranking);

  vector<jet::JoinExpTree> leaves;
  leaves.reserve(formulas.size());
  
  for (size_t i = 0; i < formulas.size(); i++)
  {
    leaves.push_back(jet::JoinExpTree::leaf(i, support_sets[i]));
  }

  vector<jet::JoinExpTree> clusters = clustering.apply(leaves);
  LTLInterp interp(move(formulas));

  size_t i = 0;
  
  for (const jet::JoinExpTree& cluster : clusters)
  {
    ltl_formula *clustered_root = interp.eval(cluster);
    
    ofstream out(outprefix + "_" + std::to_string(i) + ".mona");

    //myfile.close(); //关闭文件
    strcpy (in, StrLine.c_str());
    //out << "#" << in << endl;

    ltl_formula *bnfroot = bnf (clustered_root);
    ltl_formula *newroot;

    //out << "#" << to_string(bnfroot).c_str() << endl;

    if(format == "NNF"){
      out << "#NNF format" << endl;
      newroot = nnf (bnfroot);   
    }
    else{
      out << "#BNF format" << endl;
      newroot = bnfroot;
    }

    //out << "#" << to_string(newroot).c_str() << endl;

    ltlf2fol (newroot, out);

    // printf ("%s\n", res.c_str ());
    destroy_formula (clustered_root);
    destroy_formula (newroot);
    //destroy_formula (nnfroot);

    out.close();

    ++i;
  }

  myfile.close();
}
