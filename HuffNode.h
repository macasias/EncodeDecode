#ifndef HUFFNODE_H
  #define HUFFNODE_H

using namespace std;

class HuffNode
{
public:
  HuffNode *left;
  HuffNode *right;
  int count;
  int character;


  HuffNode(HuffNode *l, HuffNode *r, int c, int ch) 
    :left(l), right(r), count(c), character(ch) 
  {}
  ~HuffNode();

};

#endif
