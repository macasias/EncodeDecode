//  Author: Sean Davis

#include "encoder.h"
#include <iostream>
using namespace std;

Encoder::Encoder()
{
  for(int i = 0; i < 256; i++)
    counts[i] = 0; //initialize counts to 0 
} // Encoder()


Encoder::~Encoder()
{
} // ~Encoder()



void assignBits(HuffNode *node, int *bits, int position, int character[][30])
{
  if(node->character != -1)
  {
    for(int j = 0; j < 30; j++)
      character[node->character][j] = bits[j];
    int i = 0;
    while(bits[i] != -1)
    {
      cout << bits[i];
      i++;
    }
    cout << " " << (char)node->character << endl;
    return;
  }

  bits[position] = 1;
  assignBits(node->left, bits, position+1, character);

  bits[position] = 0;
  assignBits(node->right, bits, position+1, character);
  
}


void Encoder::encode(const unsigned char *message, const int size, 
  unsigned char *encodedMessage, 
    int *encodedSize)
{
  for(int i = 0; i < size; i++)
    counts[message[i]]++; //counts occurances
  cout << "Counted" << endl;
  BinaryHeap <HuffNode*> huffHeap(256);

  for(int i = 0; i < 256; i++) //Build your heap for HuffTree
    if(counts[i] != 0)
    {
      HuffNode *huff = new HuffNode(NULL, NULL, counts[i], i);
      huffHeap.insert(huff);
    }
  cout << "Tree built" << endl;
  HuffNode *newLeft;
  HuffNode *newRight;

  while(huffHeap.currentSize >= 2)
  {
    newLeft = huffHeap.findMin();
    huffHeap.deleteMin();
    newRight = huffHeap.findMin();
    huffHeap.deleteMin();    
    //gets children of try tree

    int newCount = newLeft->count + newRight->count; //add counts together
    HuffNode *tree = new HuffNode(newLeft, newRight, newCount, -1); //creates try   
    huffHeap.insert(tree); //puts try back into heap
  } //builds huffman tree
  cout << "HuffTree built" << endl;

  int bits[30];
  for(int i = 0; i < 30; i++)
    bits[i] = -1; //sets all bits to -1 as a marker
 
  int characters[256][30];
  assignBits(huffHeap.findMin(), bits, 0, characters);
 
  cout << "Bits assigned" << endl;

  unsigned int workbench = 0;
  int pos = 0;
  int bitCounter = 0; //tracks bit position

  for(int i = 0; i < size; i++) //go through list
  {
    int j = 0; //i is the message spot, j is the bit spot
    while(characters[message[i]][j] != -1) //-1 in the array means stop
    {  
      if(characters[message[i]][j] == 1)
      {
        workbench |= 1 << bitCounter; //set a 1
        cout << ((workbench >> bitCounter) & 1);
      }
      else
      {
        workbench &= ~(1 << bitCounter); //set a 0
        cout << ((workbench >> bitCounter) & 1);
      }
 
      bitCounter++;
      j++;  
    }

    while(bitCounter >= 8)
    {
//      cout << endl;
      cout << "Workbench: " << (workbench) << endl;
      encodedMessage[pos++] = (workbench >> 24); //shift bits to only take in 8bits(1 char)
//      cout << encodedMessage[pos-1] << endl;
      workbench <<= 8; //removes first 8 bits(since it's been stored now)
      bitCounter = bitCounter - 8; 
    }
  }
  cout << endl;
  cout <<"Check Message" << endl;
/*  for(int j = 0x80; j > 0; j>>= 1)
    if(encodedMessage[4] & j)
      cout << '1';
    else
      cout << '0';
  cout << endl; */
  cout << (int)encodedMessage[0] << endl;
}  // encode()
