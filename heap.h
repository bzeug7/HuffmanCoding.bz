#define MAX_CAP 256
#define ENCODING_SIZE (1 << 8 * sizeof(char))
#include <stdio.h>
typedef struct node_t{
	int isLeaf;
  int frequency;
	int c;
  struct node_t *left;
  struct node_t *right;
} Node;

typedef struct heap_t{
  int size;
  int cap;
  struct node_t ** heap;
} Heap;

//should be in encode.h
typedef struct {
  int c;
  int enc;
  int len;
} Encoding;

Heap *create_heap(int capacity);

void build_heap(Heap *heap, int data[], int freq[]);

void build_huffman_tree(Heap *heap);

Encoding* get_encodings(Heap *heap);

Node *rebuild_heap(Encoding encodings[]);

int read_hmc(FILE *fp, Node *n, char *buffer, int *offset, FILE *out);