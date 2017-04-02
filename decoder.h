// Author: Sean Davis
#ifndef DECODER_H
#define	DECODER_H

#include "HuffNode.h"
#include "BinaryHeap.h"

class Decoder
{
public:
  Decoder();
  ~Decoder();
  void decode(const unsigned char* encodedMessage, const int encodedSize, 
    unsigned char* decodedMessage, int *decodedSize);
private:
  int counts[256];
};

#endif	/* DECODER_H */

