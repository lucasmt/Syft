#include "ltlf2fol.h"
#include <assert.h>
#include <fstream>
#include <iostream>

#define MAXN 1000000
char in[MAXN];

using namespace std;

int main (int argc, char ** argv)
{
  string StrLine;
  std::string input;
  std::string outprefix;
  std::string format;
  assert(argc == 4);
  input = argv[1];
  outprefix = argv[2];
  format = argv[3];
  ifstream myfile(input);
  if (!myfile.is_open()) //判断文件是否存在及可读
  {
    printf("unreadable file!");
    return -1;
  }

  size_t i = 0;
  
  while (getline(myfile, StrLine))
  {
    ofstream out(outprefix + "_" + to_string(i) + ".mona");

    //myfile.close(); //关闭文件
    strcpy (in, StrLine.c_str());
    //out << "#" << in << endl;

    ltl_formula *root = getAST (in);
    ltl_formula *bnfroot = bnf (root);
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
    destroy_formula (root);
    destroy_formula (newroot);
    //destroy_formula (nnfroot);

    out.close();

    ++i;
  }

  myfile.close();
}
