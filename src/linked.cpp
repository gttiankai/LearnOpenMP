// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef N
#  define N 5
#endif
#ifndef FS
#  define FS 38
#endif

struct node {
  int data;
  int fib_data;
  struct node* next;
};

int fib(int n) {
  int x, y;
  if (n < 2) {
    return (n);
  } else {
    x = fib(n - 1);
    y = fib(n - 2);
    return (x + y);
  }
}

void processwork(struct node* p) {
  int n;
  n = p->data;
  p->fib_data = fib(n);
}

struct node* init_list(struct node* p) {
  int i;
  struct node* head = NULL;
  struct node* temp = NULL;

  head = (struct node*)malloc(sizeof(struct node));
  p = head;
  p->data = FS;
  p->fib_data = 0;
  for (i = 0; i < N; i++) {
    temp = (struct node*)malloc(sizeof(struct node));
    p->next = temp;
    p = temp;
    p->data = FS + i + 1;
    p->fib_data = i + 1;
  }
  p->next = NULL;
  return head;
}

/**
Process linked list
Each linked list node will be processed by function 'processwork()'
Each linked list node will compute 5 fibonacci numbers beginning with 38
38 : 39088169
39 : 63245986
40 : 102334155
41 : 165580141
42 : 267914296
43 : 433494437
Compute Time: 7.087344 seconds
 * */

int main(int argc, char* argv[]) {
  double start, end;
  struct node* p = NULL;
  struct node* temp = NULL;
  struct node* head = NULL;

  printf("Process linked list\n");
  printf("  Each linked list node will be processed by function 'processwork()'\n");
  printf("  Each linked list node will compute %d fibonacci numbers beginning with %d\n", N, FS);

  p = init_list(p);
  head = p;

  start = omp_get_wtime();
  {
    while (p != NULL) {
      processwork(p);
      p = p->next;
    }
  }

  end = omp_get_wtime();
  p = head;
  while (p != NULL) {
    printf("%d : %d\n", p->data, p->fib_data);
    temp = p->next;
    free(p);
    p = temp;
  }
  free(p);

  printf("Compute Time: %f seconds\n", end - start);

  return 0;
}
