
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "heap.h"

int process_hmc(FILE *fp, Node *n, FILE *out){
  //printf("Beginning to process file\n");
  int res, offset = 0;
  char buffer;
  do{
    res = read_hmc(fp, n, &buffer, &offset, out);
  }while(res == 0);
  //printf("\nEnd of file\n");
  return res == 1? 0 : -1;
}

int decode(char *file, char *out){
  Encoding encodings[ENCODING_SIZE] = {0};
  int max_len = 0, i, j, c, size = 0;

  FILE *fp = fopen(file, "r");
  FILE *out_fp;
  if(out){
     out_fp = fopen(out, "w"); 
  }else{
    int name_idx = strrchr(file, '.') - file;
    char *new_file = malloc((sizeof(char)) * name_idx + 5);
    strncpy(new_file, file, name_idx);
    strcat(new_file, ".txt");
    out_fp = fopen(new_file, "w");
  }
  if(fp == 0){
    printf("Could not open file: %s\n", file);
    return -1;
  }else if(out_fp == 0){
    printf("Could not open file: %s\n", out);
  }

  for(i = sizeof(int) - 1; i >= 0; i--){
    max_len |= fgetc(fp) << (i * 8);
  }
  for(i = 0; i < ENCODING_SIZE; i++){
    Encoding e;
    c = fgetc(fp);
    if(c != 0){
      e.c = c;
      e.enc = 0;
      e.len = 0;
      for(j = max_len >> 3; j >= 0; j--){
        e.enc |= (fgetc(fp) << (j * 8));
      }
      for(j = (sizeof(int) * 8) >> 8; j >= 0; j--){
        e.len |= (fgetc(fp) << (j * 8));
      }
      printf("%c %x %d\n", e.c, e.enc, e.len);
      encodings[c] = e;
      size++;
    }else{
      break;
    }
  }

  Node * n = rebuild_heap(encodings);
  process_hmc(fp, n, out_fp);
  fclose(out_fp);
  fclose(fp);
  return 0;
}