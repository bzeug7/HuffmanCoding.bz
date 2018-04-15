#include <stdlib.h>
#include "heap.h"

Heap *create_heap(int capacity){
  Heap *h = (Heap *) malloc(sizeof(Heap));
  h->heap = (Node **) malloc(capacity * sizeof(Node *));
  h->cap = capacity;
  h->size = 0;
  return h;
}

void print_heap(Heap *h, int idx){
  int left = 2 * idx + 1, right = 2 * idx + 2;
  printf("Idx %d, char %c, freq %d\n", idx, h->heap[idx]->c, h->heap[idx]->frequency);
  if(left < h->size)
    print_heap(h, left);
  if(right < h->size)
    print_heap(h, right);
}

void swapNode(Node **a, Node **b){
  Node *temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

void heapify(Heap *h, int idx){
  int smallest = idx, left = idx * 2 + 1, right = idx * 2 + 2;
  if(left < h->size && h->heap[left]->frequency < h->heap[smallest]->frequency)
    smallest = left;

  if(right < h->size && h->heap[right]->frequency < h->heap[smallest]->frequency)
    smallest = right;

  if(smallest != idx){
    swapNode(&h->heap[smallest], &h->heap[idx]);
    heapify(h, smallest);
  }
}

void build_heap(Heap *h, int data[], int freq[]){
  int i;
  Node *n;
  for(i = 0; i < h->cap; i++){
    n = (Node *) malloc(sizeof(Node));
    n->isLeaf = 1;
    n->c = data[i];
    n->frequency = freq[i];
    n->left = NULL;
    n->right = NULL;
    h->heap[i] = n;
    h->size++;
  }

  for(i = (h->size - 2) / 2; i >= 0; i--){
    heapify(h, i);
  }
  //print_heap(h, 0);
}

Node *extract_min(Heap *h){
  Node * ret = h->heap[0];
  h->heap[0] = h->heap[h->size - 1];
  h->heap[h->size--] = 0;
  heapify(h, 0);

  return ret;
}

void insert(Heap *h, Node *node){
  ++h->size;
  int i = h->size - 1;
 
  while (i && node->frequency < h->heap[(i - 1) / 2]->frequency) {

      h->heap[i] = h->heap[(i - 1) / 2];
      i = (i - 1) / 2;
  }

  h->heap[i] = node;
}

void build_huffman_tree(Heap *heap){
  Node *top, *left, *right;
  
  while(heap->size > 1){
    left = extract_min(heap);
    right = extract_min(heap);

    top = (Node *) malloc(sizeof(Node));
    top->frequency = left->frequency + right->frequency;
    top->isLeaf = 0;
    top->left = left;
    top->right = right;
    top->c = 0;

    insert(heap, top);
  }
}

Encoding get_encoding(Node **n, int enc, int length){
  if((*n)->isLeaf){
    printf(": <%c>\n", (char) (*n)->c);
     Encoding e = {(*n)->c, enc, length};
    *n = NULL;
    return e;
  }else if((*n)->left != NULL){
    printf("0");
    Encoding e = get_encoding(&(*n)->left, enc << 1, length + 1);
    if((*n)->left == NULL && (*n)->right == NULL){
      *n = NULL;
    }
    return e;
  }else{
    printf("1");
    //enc |= (1 << length);
    Encoding e = get_encoding(&(*n)->right, (enc << 1) + 1, length + 1);
    if((*n)->left == NULL && (*n)->right == NULL){
      *n = NULL;
    }
    return e;
  } 

}

Encoding* get_encodings(Heap *h){
  int idx_e = 0;
  Encoding *encodings = (Encoding *) calloc(ENCODING_SIZE, sizeof(Encoding));
  Encoding e;
  while(idx_e++ < h->cap){
    e = get_encoding(&h->heap[0], 0, 0);
    encodings[e.c] = e;
  }
  return encodings;
}

void fill_in_heap(Node *n, int enc, int len, int c){
  if(len == 0){
    n->isLeaf = 1;
    n->c = c;
  }else{
    n->isLeaf = 0;
    if(enc & (1 << (len - 1))){
      if(!n->right){
        n->right = (Node *) malloc(sizeof(Node));
        n->right->left = NULL;
        n->right->right = NULL;
      }
      fill_in_heap(n->right, enc, len - 1, c);
    }else{
      if(!n->left){
        n->left = (Node *) malloc(sizeof(Node));
        n->left->left = NULL;
        n->left->right = NULL;
      }
      fill_in_heap(n->left, enc, len - 1, c);
    }
  }
}

Node *rebuild_heap(Encoding encodings[]){
  Node *n = (Node *) malloc(sizeof(Node));
  n->left = NULL;
  n->right = NULL;
  int i;
  for(i = 0; i < ENCODING_SIZE; i++){
    if(encodings[i].len != 0){
      fill_in_heap(n, encodings[i].enc, encodings[i].len, encodings[i].c);
    }
  }

  return n;
}

int read_hmc(FILE *fp, Node *n, char *buff, int *offset, FILE *out){
  int c;
  if(n->isLeaf){
    fprintf(out, "%c", n->c);
    return 0;
  }
  if(*offset == 0){
    *offset = 8 * sizeof(char);
    c = fgetc(fp);
    //printf("%x\n", c);
    if(c == EOF){
      return 1;
    }
    *buff = c; 
  }
  *offset = *offset - 1;
  if(*buff & (1 << *offset)){
    return read_hmc(fp, n->right, buff, offset, out);
  }else{
    return read_hmc(fp, n->left, buff, offset, out);
  }
}