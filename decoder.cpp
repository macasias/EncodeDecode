//  Author: Sean Davis

#include "decoder.h"
#include <iostream>
using namespace std;

Decoder::Decoder()
{
  for(int i = 0; i < 256; i++)
    counts[i] = 0;
} // Decoder()


Decoder::~Decoder()
{
} // ~Decoder()


void reAssignBits(HuffNode *node, unsigned int bits, int position, unsigned int *character,
                unsigned int *codeLength)
{
  if(node->left == NULL) // || node->right == NULL)
  {
    character[node->character] = bits;
    codeLength[node->character] = position;

    return;
  }

  reAssignBits(node->left, bits, position+1, character, codeLength);
  //left shift bits(add a zero) formerly passed in (bits << 1)

  bits |= 1 << position; //change to 1
  reAssignBits(node->right, bits, position+1, character, codeLength);

}


/*
void decodeMessage(HuffNode *node,  int encodePos, unsigned char *encodedMessage,
                  unsigned char *decodedMessage, int decodePos, int bit,
                  int bitCounter)
{ //needs to return the length of the code so it can subtract it from bitCounter
  if(bitCounter == 8)
  {
    encodePos++;
    bit = 1;
    bitCounter -= 8;
  }

  if(node->left == NULL) //only need to check one child
  {
    decodedMessage[decodePos++] = (node->letter);
    return;
  }

  else if(encodedMessage[encodePos++] & bit) //if rightmost bit is 1, go right
    decodeMessage(node->right, encodePos, encodedMessage,
                  decodedMessage, decodePos, (bit <<= 1), (bitCounter+1));


  else //going left
    decodeMessage(node->left, encodePos, encodedMessage,
                  decodedMessage, decodePos, (bit <<= 1), (bitCounter+1));


}
*/


void Decoder::decode(const unsigned char* encodedMessage, const int encodedSize, 
  unsigned char* decodedMessage, int *decodedSize)
{

//  cout << "Decoder" << endl;
  int numCharsUsed = (int)encodedMessage[0]; 
  //so the code knos how far to read in


  int pos = 1;

  //////////////////////////////////
  //Now I need to read in 1 byte into int byte
  //Then I read in the next 4 to fill in int counts[byte]
  //That gives count and nodes to fill in huffHeap
  //and subsequently rebuilt huffTree and reassign bits
  //////////////////////////////////
//  cout << numCharsUsed << endl;
  if( numCharsUsed == 0 )
    numCharsUsed = 256;

  for( int i = 0; i < numCharsUsed; i++ )
  {
    counts[encodedMessage[pos]] = (unsigned int)(encodedMessage[pos+1]) +
      (unsigned int)(encodedMessage[pos+2] << 8) + (unsigned int)(encodedMessage[pos+3] << 16)
      + (unsigned int)(encodedMessage[pos+4] << 24);
    //adds up the counts and increases pos by 5
    pos = pos + 5; 
  }

//  cout << "Tree ended at: " << pos << endl;

//  for(int i = 0; i < 256; i++)
//    cout << i << " " << counts[i] << endl;


  BinaryHeap <HuffNode*> huffHeap(256);
  huffHeap.currentSize = 0;

  for(int i = 0; i < 256; i++) //Rebuild your heap for HuffTree
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
    //gets children of try tree again


    int newCount = newLeft->count + newRight->count; //add counts together
    HuffNode *tree = new HuffNode(newLeft, newRight, newCount, 1, 1); //creates try

    huffHeap.insert(tree); //puts try back into heap
  } //rebuilds huffman tree


  unsigned int bits = 0;

  unsigned int characters[256];
  unsigned int codeLength[256];

  for( int i = 0; i < 256; i++ )
  {
    characters[i] = 0;
    codeLength[i] = 0;
  }

  reAssignBits(huffHeap.findMin(), bits, 0, characters, codeLength);

  unsigned int bitCounter = 0; //so I know what bit I'm looking at
  pos = 1283; //encoding is hard-coded to start there
  int bit = 1; //to track the specific bit

  int decodePos = 0;
  unsigned int extraBits = encodedMessage[1282];

if(extraBits != 0)
{
  while(pos < (encodedSize - 1) || bitCounter != extraBits)
  {
    HuffNode *ptr = huffHeap.findMin();
    
    while( ptr->left ) 
    {
      if(encodedMessage[pos] & bit)
      {
        ptr = ptr->right;
        bit <<= 1;
      }
      else
      {
        ptr = ptr->left;
        bit <<= 1;
      }

      bitCounter++;

      if(bitCounter == 8)
      {
        bitCounter = 0;
        pos++;
        bit = 1;
      }
    }

    decodedMessage[decodePos++] = (ptr->letter);

  }
}

else {
  while(pos < (encodedSize) || bitCounter != extraBits)
  {
    HuffNode *ptr = huffHeap.findMin();

    while( ptr->left )
    {
      if(encodedMessage[pos] & bit)
      {
        ptr = ptr->right;
        bit <<= 1;
      }
      else
      {
        ptr = ptr->left;
        bit <<= 1;
      }

      bitCounter++;

      if(bitCounter == 8)
      {
        bitCounter = 0;
        pos++;
        bit = 1;
      }
    }

    decodedMessage[decodePos++] = (ptr->letter);

  }
}
  *decodedSize = decodePos;

} // decode()
