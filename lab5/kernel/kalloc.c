// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// 内存引用计数的结构体
struct 
{
  struct spinlock lock;// 若有多个进行同时对数组进行操作，需要上锁
  int mem_count[PHYSTOP/PGSIZE];
}mem_ref_struct;

int get_mem_count(uint64 pa){
  int count; 
  acquire(&mem_ref_struct.lock);
  count = mem_ref_struct.mem_count[(uint64)pa / PGSIZE];
  release(&mem_ref_struct.lock);
  return count;
}
void mem_count_up(uint64 pa){
  acquire(&mem_ref_struct.lock);
  ++ mem_ref_struct.mem_count[(uint64)pa / PGSIZE];
  release(&mem_ref_struct.lock);
}

int mem_count_down(uint64 pa){
  int flag = 0;
  acquire(&mem_ref_struct.lock);
  if((-- mem_ref_struct.mem_count[(uint64)pa / PGSIZE]) == 0){
    flag = 1;
  }
  release(&mem_ref_struct.lock);
  return flag;
}

void mem_count_set_one(uint64 pa){
  acquire(&mem_ref_struct.lock);
  mem_ref_struct.mem_count[(uint64)pa / PGSIZE] = 1;
  release(&mem_ref_struct.lock);
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  //初始化自旋锁
  initlock(&mem_ref_struct.lock, "mem_ref");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    mem_count_set_one((uint64)p);
    kfree(p);
  }
    
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    // 设置内存引用为1
    mem_count_set_one((uint64)r);
  }
    
  release(&kmem.lock);
  if(r)
    memset((char*)r, 5, PGSIZE); 
  return (void*)r;
}

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  
  r = (struct run*)pa;
  if(mem_count_down((uint64)pa) == 1){
    //此时内存引用为0， 需要释放物理内存
    memset(pa, 1, PGSIZE);
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  
}

