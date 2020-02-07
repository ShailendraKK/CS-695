#include <stddef.h>
#include <stdint.h>


/**
 * 
 * Sends a 8/16/32-bit value on a I/O location. 
 * Traditional names are outb, outw and outl respectively. 
 * The a modifier enforces val to be placed in the eax register before the asm command is issued
 *  and Nd allows for one-byte constant values to be assembled as constants, 
 * freeing the edx register for other cases.
 * Memory clobber to force ordering. 
 * tells the compiler that the assembler instruction may change memory locations
 * "memory" Clobber forces the compiler to store all cached values before and reload them 
 * after executing the assembler instructions. And it must retain the sequence.
 * */

static void outb(uint16_t port, uint8_t value) {
	asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

/*
static void str_cpy(uint16_t port,uint32_t addr) {
 
  	asm("mov dx, %0":	:"r" (port) : "rdx");
 	asm("mov eax, %0":	:"r" (addr) : "rax");
	asm("out dx, eax");


}*/


static inline void printVal(uint16_t port, uint32_t value) {
  asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}
static void display(const char *str){
	uint32_t *addr = (uint32_t *) str;
	//str_cpy(0xE5,*addr);
	printVal(0xE5,*addr);
}

static inline uint32_t inb(uint16_t port) {
  uint32_t ret;
  asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
  return ret;
}
static inline uint32_t getNumExits(){
  uint32_t ret;
  ret = inb(0xE7);
  return ret;
}





void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;

/**
 * Port 0xE9 is often used by some emulators to directly send text to the hosts console.
 * */
/**
 * Q7 What port number is used for this communication?
 * Ans Port 0xE9 // 223
 * 
 * 
 * */
	for (p = "Hello, world!\n"; *p; ++p)
		outb(0xE9, *p);
	uint32_t numExits = getNumExits();
	printVal(0xE8,numExits);
		
	
	//*(long **) 0x500 = (long *)str;
	display("HiVM");
	numExits = getNumExits();
	printVal(0xE8,numExits);
	


/**
 * Q8  Can you figure out what's happening with the number 42? 
 * Where is it written in the guest and where is it read out in the hypervisor? 
 * Ans : 
 * */
	*(long *) 0x400 = 42;


	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
