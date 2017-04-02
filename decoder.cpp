//  Author: Sean Davis

#include "decoder.h"

//using namespace std;

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
  if(node->left == NULL)
  {
    character[node->character] = bits;
    codeLength[node->character] = position;
/*
    cout << (unsigned char)node->character << " ";

    for( int j = 0x200; j > 0; j >>= 1)
      if(character[node->character] & j)
        cout << '1';
      else cout << '0';

    cout << endl;
*/

    return;
  }

  reAssignBits(node->left, bits, position+1, character, codeLength);
  //left shift bits(add a zero) formerly passed in (bits << 1)

  bits |= 1 << position; //change to 1
  reAssignBits(node->right, bits, position+1, character, codeLength);

}



int decodeMessage(HuffNode *node,  unsigned int workbench, int codeLength,
                  unsigned char *decodedMessage, int decodePos)
{ //needs to return the length of the code so it can subtract it from bitCounter
  int maybe = 0;  

  if(node->left == NULL) //only need to check one child
  {
    decodedMessage[decodePos] = (unsigned char)(node->character);

    return codeLength;
  }

  else if(workbench & 1) //if rightmost bit is 1, go right
    maybe =  decodeMessage(node->right, (workbench >> 1), (codeLength + 1),
                  decodedMessage, decodePos);


  else //going left
    maybe = decodeMessage(node->left, (workbench >> 1), (codeLength + 1),
                  decodedMessage, decodePos);

  return maybe;
}



void Decoder::decode(const unsigned char* encodedMessage, const int encodedSize, 
  unsigned char* decodedMessage, int *decodedSize)
{
  ///////////////////////////////////
  //Here is where I read in first 4 bytes so I know how far in to read
  //////////////////////////////////

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

  for( int i = 0; i < numCharsUsed; i++ )
  {
    counts[encodedMessage[pos]] = (int)(encodedMessage[pos+1]) +
      (int)(encodedMessage[pos+2] << 8) + (int)(encodedMessage[pos+3] << 16)
      + (int)(encodedMessage[pos+4] << 24);
    //adds up the counts and increases pos by 5
    pos = pos + 5; 
  }

//  cout << "Count of a: " << counts['!'] << endl;

  BinaryHeap <HuffNode*> huffHeap(256);
  huffHeap.currentSize = 0;

  for(int i = 0; i < 256; i++) //Rebuild your heap for HuffTree
    if(counts[i] != 0)
    {
      HuffNode *huff = new HuffNode(NULL, NULL, counts[i], i);
      huffHeap.insert(huff);
//      cout << "Reinserting " << (char)i << " Count: " << counts[i] << endl;
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

//    cout << (char)(newLeft->character) << " " << (char)(newRight->character) << endl;

    int newCount = newLeft->count + newRight->count; //add counts together
    HuffNode *tree = new HuffNode(newLeft, newRight, newCount, 1); //creates try

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

/*
  for(int j = 0x80; j > 0; j >>= 1)
    if(characters['!'] & j)
      cout << '1';
    else cout << '0';

  cout << endl;

  unsigned char test = (unsigned char)characters['a'];

  for(int j = 0x80; j > 0; j >>= 1)
    if( test & j )
      cout << '1';
    else cout << '0';

  cout << endl;
  //This was me testing to see if I needed to shift the bits over 
  //before setting an int equal to a char. I did not, encoder should
  //work the way it is.
*/

  int bitCounter = 0; //so I know what bit I'm looking at
  pos = 1281; //encoding is hard-coded to start there
  unsigned int workbench = 0;
  int bit = 1; //to track the specific bit
  int bitsUsed;

  /////////////////////////////////////
  //I guess what I'll have to do is just grab one int of data, 
  //then take it to a function that returns an int
  //That int will tell me how many bits to shift over when I get back
  //The function will start at the head of the tree. 
  //If it reads a 1, go left, if it reads a 0, go right.
  //Every time it goes, increment 1 each time.
  //When the node has no children, done. 
  /////////////////////////////////////
/*
  workbench = encodedMessage[pos++];
  (workbench >> 8) = encodedMessage[pos++];
  (workbench >> 16) = encodedMessage[pos++];
  (workbench >> 24) = encodedMessage[pos++];  
  //supposed to load up the workbench the first time
*/

  for(int i = 0; i < 4; i++) //because 4 chars to an int
  {
    bit = 1;
    int j = 0;
    while( j < 8 ) //makes sure bit stays below 8
    { 
      if( encodedMessage[pos] & bit )
        workbench |= 1 << (bitCounter); //set a 1;

      j++;
      bit <<= 1;
      bitCounter++;
    }
    pos++;
  } //this loads up workbench for the first time(all 32 bits)
  
//  cout << bitCounter << endl; //should be 32
//  cout << pos << endl; //should be 1285

  int decodePos = 0;

  while( pos < encodedSize )
  {
    bitsUsed = decodeMessage(huffHeap.findMin(),workbench, 0,
                             decodedMessage, decodePos++);

    workbench >>= bitsUsed; //remove the bits we already looked at
    
    bitCounter = bitCounter - bitsUsed;

    while(bitCounter < 24) //while theres enought room for another byte
    {
      int j = 0; //counts bits in new byte
      bit = 1;

      while( j < 8 ) //add another byte
      {
        if( encodedMessage[pos] & bit )
          workbench |= 1 << (bitCounter);

        j++;
        bit <<= 1;
        bitCounter = bitCounter+1;
      
      }
  
      pos++;
    }
  }

//  cout << (int)decodedMessage[1] << endl;
  *decodedSize = decodePos;
//  cout << decodePos << endl;
} // decode()
