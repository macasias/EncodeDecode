// Author: Sean Davis
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "CPUTimer.h"
#include "encoder.h"
#include "decoder.h"

using namespace std;


int main(int argc, char** argv)
{
  unsigned char *message, *encodedMessage, *decodedMessage;
  int encodedSize, decodedSize, size, i;
  Encoder *encoder;
  Decoder *decoder;
  struct stat filestatus;
  stat(argv[1], &filestatus);
  size = filestatus.st_size;
  message = new unsigned char[size];
  encodedMessage = new unsigned char[size];
  decodedMessage = new unsigned char[2 * size];
  ifstream inf(argv[1], ios::binary);
  inf.read((char*) message, size);
  inf.close();
  CPUTimer ct;
  encoder = new Encoder();
  encoder->encode((const unsigned char*) message, (const int) size, encodedMessage, 
    &encodedSize);
  delete encoder;
  decoder = new Decoder();
  decoder->decode((const unsigned char*) encodedMessage, (const int) encodedSize, 
    decodedMessage, &decodedSize);
  cout << dec << "CPU time: " << ct.cur_CPUTime() << " Encoded size: " 
      << encodedSize;

  for(i = 0; i < filestatus.st_size && message[i] == decodedMessage[i]; i++);
  
  if(i == filestatus.st_size && decodedSize == filestatus.st_size)
    cout << " OK\n";
  else
    if( message[i] != decodedMessage[i])
      cout << "\nMismatch at location " << i << hex << showbase <<" original: " 
        << (int) message[i] << " decoded: " << (int) decodedMessage[i] << endl;
    else
      cout << "\nMismatch of sizes, original: " << filestatus.st_size 
        << " decoded: " << decodedSize << endl;
 
  return 0;
} // main()


