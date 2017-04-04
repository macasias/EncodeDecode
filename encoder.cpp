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


void assignBits(HuffNode *node, unsigned int bits, int position, unsigned int *character,
                unsigned int *codeLength)
{
  if(node->left == NULL)
  {
    character[node->character] = bits;
    codeLength[node->character] = position;

    return;
  }

  assignBits(node->left, bits, position+1, character, codeLength);
  //left shift bits(add a zero) formerly passed in (bits << 1)

  bits |= 1 << position; //change to 1
  assignBits(node->right, bits, position+1, character, codeLength);
  
}


void Encoder::encode(const unsigned char *message, const int size, 
  unsigned char *encodedMessage, 
    int *encodedSize)
{
//  cout << "Encoder" << endl;
  for(int i = 0; i < size; i++)
    counts[message[i]]++; //counts occurances


//  for(int i = 0; i < 256; i++)
//    cout << i << " " << counts[i] << endl;

  BinaryHeap <HuffNode*> huffHeap(256);
  huffHeap.currentSize = 0;
  for(int i = 0; i < 256; i++) //Build your heap for HuffTree
    if(counts[i] != 0)
    {
      HuffNode *huff = new HuffNode(NULL, NULL, counts[i], i, (char)i);
      huffHeap.insert(huff);

    }

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
    HuffNode *tree = new HuffNode(newLeft, newRight, newCount, 1, 1); //creates try   
    huffHeap.insert(tree); //puts try back into heap
  } //builds huffman tree


  unsigned int bits = 0;
 
  unsigned int characters[256];
  unsigned int codeLength[256];

  for( int i = 0; i < 256; i++ )
  {
    characters[i] = 0;
    codeLength[i] = 0;
  }

  assignBits(huffHeap.findMin(), bits, 0, characters, codeLength);
 

  unsigned int workbench = 0;
  int pos = 1; //start of hufftree
  unsigned int bitCounter = 0; //tracks bit position


  unsigned int numChars = 0;

  for(int i = 0; i < 256; i++)
  {
    if(counts[i] != 0)
    {
      encodedMessage[pos++] = (unsigned char)i;
      //when decoding, need to treat it as an int-tracks the char used

      unsigned int charCount = counts[i];
      //stored number of that char into an int
      
      encodedMessage[pos++] = (charCount);
      encodedMessage[pos++] = (charCount >> 8);
      encodedMessage[pos++] = (charCount >> 16);
      encodedMessage[pos++] = (charCount >> 24); 
      //breaks up that int into 4 chars

      numChars++;
      //tracks number used for encodedMessage[0-3]
    } //may need to change the order of charCount
  }


  encodedMessage[0] = (numChars);


  pos = 1283; //because 256 potential different chars need 5 bytes each 
              //plus 1 for the int holding number of different chars
              //provides hardcoding for start of actual message

  for(int i = 0; i < size; i++) //go through list
  {
    int j = 0; //to know how many bits it needs to change
    int bit = 1; //to track the specific bit
    while( j < (int)codeLength[message[i]] ) 
    {  
      if( characters[message[i]] & bit )
        workbench |= 1 << bitCounter; //set a 1

      bit <<= 1; //look at next bit
      j++; 
      bitCounter++; //to track how many bits of workbench
    }
 
    while(bitCounter >= 8) //when over 1 byte, needs to add it to encoding
    {

//      encodedMessage[pos++] = (workbench >> 24); //only looks at 1 byte(1 char)
      encodedMessage[pos++] = workbench;

//      workbench <<= 8; //removes first 8 bits(since it's been stored now)
      workbench >>= 8;

      bitCounter = bitCounter - 8; 
    } 
  }

  if( bitCounter > 0 ) //catches any remaining bits
    encodedMessage[pos++] = workbench;
/*
  for(int j = 0x80; j > 0; j >>= 1)
    if(encodedMessage[pos-1] & j)
      cout << '1';
    else
      cout << '0';

  cout << endl;
*/



  cout << bitCounter << endl;

  encodedMessage[1282] = (char)bitCounter;

  cout << (int)encodedMessage[1282] << endl;

  *encodedSize = pos;

}  // encode()
