#include "linux/syscalls.h"

struct Node {
  struct Node* prev;
  int val;
};

struct Stack {
  struct Node* top;
};

SYSCALL_DEFINE1(stack_create, size_t *, ptr){
  struct Stack* stack = kmalloc(sizeof(struct Stack), GFP_KERNEL);
  pr_info("### Criando stack %p", stack);
  stack->top = NULL;
  if(copy_to_user(ptr, &stack, sizeof(void*))){
	  return -EFAULT;
  }
  return 0;
}

SYSCALL_DEFINE1(stack_destroy, size_t, ptr){
  struct Stack* stack = (struct Stack*)ptr;
  pr_info("### Começando a destruir o Stack %p", stack);
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

SYSCALL_DEFINE2(stack_append, size_t, ptr, int, val){
  struct Stack *stack = (struct Stack*)ptr;
  pr_info("### Adicionando valor %d ao stack %p", val, stack);
  struct Node *new = kmalloc(sizeof(int), GFP_KERNEL);
  new->prev = stack->top;
  new->val = val;
  stack->top = new;
  return 0;
}

SYSCALL_DEFINE2(stack_pop, size_t, ptr, int __user *, out_val){
  struct Stack* stack = (struct Stack*)ptr;
  if (stack->top == NULL) {
    return -EFAULT;
  }
  struct Node* curr = stack->top;
  *out_val = curr->val;
  pr_info("### Pop do valor %d do stack %p", curr->val, stack);
  if(copy_to_user(out_val, &curr->val, sizeof(int))){
	  return -EFAULT;
  }
  stack->top = stack->top->prev;
  kfree(curr);
  return 0;
}
