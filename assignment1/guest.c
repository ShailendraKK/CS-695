#include <stddef.h>
#include <stdint.h>

#define HC_status 0x8000
#define HC_open  (HC_status | 0)
#define HC_read  (HC_status | 1)
#define HC_write  (HC_status | 2)

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
	asm("out dx, eax":	:	:"memory");


}*/


static inline void printVal(uint16_t port, uint32_t value) {
  asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
 // asm("outl %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}



static void display(const char *str){
	
//	*(long *) 0x500 = addr;
	//uint32_t addr_temp =
	//uint32_t addr_temp =(uin)
    //uint32_t addrtemp = 
//	str_cpy(0xE5,*addr);
uint32_t  addr = (uintptr_t) str;
	printVal(0xE5,addr);
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

static inline uint32_t open(const char *str){

	uint32_t ret;
	uint32_t  addr = (uintptr_t) str;
	printVal(HC_open,addr);

	ret = inb(HC_open);
	return ret;
}
static inline char * read(uint32_t fd,char buff[]/*,size_t count*/){
	//uint32_t *addr = (uint32_t *) str;
	//char* ret;
		//uint32_t *addr =  &fd;
	printVal(HC_read,fd);
	
	uint32_t num= inb(HC_read);
	for(uint32_t i =0;i<num;i++){
		buff[i] = inb(HC_read);
		//display(buff);
	}
	
	return buff;
}
static inline void/* char * */ write(/*uint32_t fd,*/char *buff/*,size_t count*/){
	//uint32_t *addr = (uint32_t *) str;
	//char* ret;
		uint32_t  addr = (uintptr_t) buff;
	printVal(HC_write,addr);

	//*buff = inb(HC_write);
	//return buff;
}


void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	//char * bytes_written=0;

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
	//char demo[]="Hi VM Hello";
	display("HI VM from guest");
	numExits = getNumExits();
	printVal(0xE8,numExits);
	uint32_t open_fd = open("demo1234");
	printVal(0xE8,open_fd);
	write("this is test 1");
	write("this is test 2");
	char read_buff[100];
	read(open_fd,read_buff);
	display(read_buff);
	//char read_buff[10];
	//read(open_fd,read_buff);
	//printVal(0xE8,bytes_read);
	//display(read_buff);
	//write("xyz123");
	//bytes_written = write("xyz");
	//display(bytes_written);
	


/**
 * Q8  Can you figure out what's happening with the number 42? 
 * Where is it written in the guest and where is it read out in the hypervisor? 
 * Ans : 
 * */
	*(long *) 0x400 = 42;


	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
