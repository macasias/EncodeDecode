# EncodeDecode

I built encoder.cpp, encoder.h, decoder.cpp, and decoder.h.
The other files were provided by the instructor Sean Davis. The BMP, PNG, anf TXT files were provided to help test the encoder/decoder.

To run the code, 

  >> make;
  >> encoderRunner.out [fileName]
  
  Encoder works by going through the entire file and counting the instances of each of the 256 possible characters. 
  It then uses a binary heap to assign encoded values to each of those chars, longer encodings for less frequent characters. 
  The encoding for the binary heap is stored in the first four bytes of the encoded message, followed by the actual encoded message.
  
  Decoder reads in the first four bytes of the encoded file so that it has the data to rebuild the binary heap. 
  It then reads in the rest of the encoded file bit by bit, while traversing the binary heap, and knows it has read in a 
  letter when it reaches the end of a branch. Repeat until the end of the message. 
