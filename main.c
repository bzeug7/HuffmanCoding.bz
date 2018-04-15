#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "heap.h"
#include <string.h>

int main(int argc, char **argv){
  int ret_val = 0;
  if(argc < 3){
    printf("usage error: ./huffman (-e | -d) <input_file> [output_file]\n");
    return 0;
  }
  char *out;
  if(argc == 4){
    out = argv[3];
  }else{
    out = NULL;
  }
  if(strcmp(argv[1], "-e") == 0){
    ret_val = encode(argv[2], out);
  }else if(strcmp(argv[1], "-d") == 0){
    ret_val = decode(argv[2], out);
  }else{
    printf("usage error: ./huffman (-e | -d) <filename>\n");
    return 0;
  }
  return ret_val;
}