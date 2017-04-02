// Author: Sean Davis
#ifndef ENCODER_H
#define	ENCODER_H

#include "BinaryHeap.h"
#include "HuffNode.h"

class Encoder
{
public:
  Encoder();
  void encode(const unsigned char *message, const int size, 
    unsigned char *encodedMessage, int *encodedSize);
  ~Encoder();
private:
  int counts[256];


};

#endif	/* ENCODER_H */

