#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "encode.h"
#include "heap.h"

int find_int(int data[], int val, int size){
  int i;

  for(i = 0; i < size; i++){
    if(data[i] == val){
      return i;
    }
  }
  return -1;
}

int get_char_data(char *file, int data[], int freq[]){
  FILE *fp = fopen(file, "r");
  int c_int, idx, size = 0;

  while(fp && (c_int = fgetc(fp)) != EOF){
    idx = find_int(data, c_int, size);
    if(idx != -1){
      freq[idx]++;
    }else{
      data[size] = c_int;
      freq[size++] = 1;
    }
  }
  data[size] = EOF;
  data[size++] = 1;

  fclose(fp);
  return size;
}

int get_max_len(Encoding *encodings, int size){
  int max_len = 0, i;
  for(i = 0; i < size; i++){
    max_len = encodings[i].len > max_len ? encodings[i].len : max_len;
  }
  return max_len;
}

int write_encodings(char *src, char *dst, Encoding *encodings, int size){
  FILE *fp_src = fopen(src, "r");
  FILE *fp_dst = fopen(dst, "w");

  int c_int, i, j, shift = 0, offset = 8 * sizeof(char), max_len;
  char buffer[512] = {0};

  if(!fp_src || !fp_dst)
    return -1;

  max_len = get_max_len(encodings, size);
  //write max length for decode
  for(i = sizeof(int) - 1; i >= 0; i--){
    fputc(max_len >> (8 * i), fp_dst);
  }

  //format: enc.c, enc.enc, enc.len
  //enc.enc is compressed to the max bytes needed to represent max_len
  //enc.len is compressed to the max bytes needed to represent sizeof(int) * 8
  for(i = 0; i < ENCODING_SIZE; i++){
    if(encodings[i].len != 0){
      fputc(encodings[i].c, fp_dst);
      for(j = max_len >> 3; j >= 0; j--){
        fputc(encodings[i].enc >> (8 * j), fp_dst);
      }
      for(j = (sizeof(int) * 8) >> 8; j >= 0; j--){
        fputc(encodings[i].len >> (8 * j), fp_dst);
      }
    }
    //fprintf(fp_dst, "%c %d %d\n", encodings[i].c, encodings[i].enc, encodings[i].len);
  }
  fputc(0, fp_dst);
  while((c_int = fgetc(fp_src)) != EOF){
    Encoding e = encodings[c_int];
    if(offset - e.len > 0){
      offset -= e.len;
      buffer[shift] |= (e.enc << offset);
    }else if(offset - e.len == 0){
      buffer[shift++] |= e.enc;
      for(i = 0; i < shift; i++){
        fputc(buffer[i], fp_dst);
        buffer[i] = 0;
      }
      shift = 0;
      offset = 8 * sizeof(char);
    }else{
      int length = e.len;
      while(offset - length < 0){
        buffer[shift] |= e.enc >> (length - offset);
        length = length - offset;
        shift++;
        offset = 8 * sizeof(char);
      }
      if(offset - length > 0){
        offset -= length;
        buffer[shift] |= (e.enc << offset);
      }else if(offset - length == 0){
        buffer[shift++] |= e.enc;
        for(i = 0; i < shift; i++){
          fputc(buffer[i], fp_dst);
          buffer[i] = 0;
        }
        shift = 0;
        offset = 8 * sizeof(char);
      }
    }
  }
  if(offset != 8 * sizeof(char)){
    for(i = 0; i <= shift; i++){
      fputc(buffer[i], fp_dst);
      buffer[i] = 0;
    }
  }
  fclose(fp_src);
  fclose(fp_dst);
  return 0;
}

void print_encodings(Encoding *encodings, int size){
  int i;
  for(i = 0; i < size; i++){
    printf("%c: %x %d\n", encodings[i].c, encodings[i].enc, encodings[i].len);
  }
}

int encode(char *file, char *out){
  int name_idx;
  int freq[MAX_CAP] = {0};
  int data[MAX_CAP] = {0};
  int size = get_char_data(file, data, freq);
  char *new_file;

  if(size == 0){
    printf("Could not open file: %s\n", file);
    return -1;
  }

  Heap *heap = create_heap(size);

  build_heap(heap, data, freq);

  build_huffman_tree(heap);

  Encoding *encodings;
  
  encodings = get_encodings(heap);

  //print_encodings(encodings, size);
  if(out){
    new_file = out;
  }else{
    name_idx = strrchr(file, '.') - file;
    new_file = malloc((sizeof(char)) * name_idx + 4);
    strncpy(new_file, file, name_idx);
    strcat(new_file, ".bz");
  }

  printf("Result filename: %s\n", new_file);

  return write_encodings(file, new_file, encodings, size);
}