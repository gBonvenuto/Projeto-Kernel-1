#include "linux/syscalls.h"

struct Node {
  struct Node* prev;
  int val;
};

struct Stack {
  struct Node* top;
};

SYSCALL_DEFINE2(stack_create, int, val, void __user **, ptr){
  pr_info("### Criando stack");
  struct Stack* stack = kmalloc(sizeof(struct Stack), GFP_KERNEL);
  struct Node* node = kmalloc(sizeof(struct Node), GFP_KERNEL);
  node->prev = NULL;
  node->val = val;
  stack->top = node;
  if(copy_to_user(ptr, &stack, sizeof(void*))){
	  return -EFAULT;
  }
  return 0;
}

SYSCALL_DEFINE1(stack_destroy, void __user *, ptr){
  pr_info("### Começando a destruir o Stack");
  struct Stack* stack = (struct Stack*)ptr;
  if (stack == NULL) {
	  return -EFAULT;
  }
  struct Node *node = stack->top;
  while (node != NULL) {
    struct Node *next = node->prev;
    pr_info("### Destroy: Liberando um nó de valor %d", node->val);
    kfree(node);
    node = next;
  }
  kfree(stack);
  return 0;
}

SYSCALL_DEFINE2(stack_append, void __user *, ptr, int, val){
  pr_info("### Adicionando valor %d ao stack", val);
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
  if(copy_to_user(out_val, &curr->val, sizeof(int))){
	  return -EFAULT;
  }
  pr_info("### Pop do valor %d", curr->val);
  stack->top = stack->top->prev;
  kfree(curr);
  return 0;
}
