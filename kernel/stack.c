#include "linux/syscalls.h"

struct Node {
  struct Node* prev;
  int val;
};

struct Stack {
  struct Node* top;
};

SYSCALL_DEFINE2(stack_create, int, val, void __user *, ptr){
  struct Stack* stack = (struct Stack*)ptr;
  struct Node* node = kmalloc(sizeof(struct Node), GFP_KERNEL);
  node->prev = NULL;
  node->val = val;
  stack->top = node;
  return 0;
}

SYSCALL_DEFINE1(stack_destroy, void __user *, ptr){
  struct Stack* stack = (struct Stack*)ptr;
  if (stack == NULL) {
	  return -1;
  }
  struct Node *node = stack->top;
  while (node != NULL) {
    struct Node *next = node->prev;
    pr_info("### Liberando um nó de valor %d", node->val);
    kfree(node);
    node = next;
  }
  // Não cabe ao kernel liberar o espaço alocado pelo stack. Cabe ao usuário
  return 0;
}

SYSCALL_DEFINE2(stack_append, void __user *, ptr, int, val){
  struct Stack *stack = (struct Stack*)ptr;
  struct Node *new = kmalloc(sizeof(int), GFP_KERNEL);
  new->prev = stack->top;
  new->val = val;
  stack->top = new;
  return 0;
}

SYSCALL_DEFINE2(stack_pop, void __user *, ptr, int __user *, out_val){
  struct Stack* stack = (struct Stack*)ptr;
  if (stack->top == NULL) {
    return 0;
  }
  struct Node* curr = stack->top;
  *out_val = curr->val;
  stack->top = stack->top->prev;
  kfree(curr);
  return 0;
}
