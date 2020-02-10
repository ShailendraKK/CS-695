#include <stddef.h>
#include <stdint.h>
  /*  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>*/

#define HC_status 0x8000
#define HC_open  (HC_status | 0)
#define HC_read  (HC_status | 1)
#define HC_write  (HC_status | 2)



#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#ifndef O_CREAT
# define O_CREAT	   0100	/* Not fcntl.  */
#endif
#ifndef O_EXCL
# define O_EXCL		   0200	/* Not fcntl.  */
#endif
#ifndef O_NOCTTY
# define O_NOCTTY	   0400	/* Not fcntl.  */
#endif
#ifndef O_TRUNC
# define O_TRUNC	  01000	/* Not fcntl.  */
#endif
#ifndef O_APPEND
# define O_APPEND	  02000
#endif
#ifndef O_NONBLOCK
# define O_NONBLOCK	  04000
#endif
#ifndef O_NDELAY
# define O_NDELAY	O_NONBLOCK
#endif
#ifndef O_SYNC
# define O_SYNC	       04010000
#endif
#define O_FSYNC		O_SYNC
#ifndef O_ASYNC
# define O_ASYNC	 020000
#endif
#ifndef __O_LARGEFILE
# define __O_LARGEFILE	0100000
#endif

#ifndef __O_DIRECTORY
# define __O_DIRECTORY	0200000
#endif
#ifndef __O_NOFOLLOW
# define __O_NOFOLLOW	0400000
#endif
#ifndef __O_CLOEXEC
# define __O_CLOEXEC   02000000
#endif
#ifndef __O_DIRECT
# define __O_DIRECT	 040000
#endif
#ifndef __O_NOATIME
# define __O_NOATIME   01000000
#endif
#ifndef __O_PATH
# define __O_PATH     010000000
#endif
#ifndef __O_DSYNC
# define __O_DSYNC	 010000
#endif
#ifndef __O_TMPFILE
# define __O_TMPFILE   (020000000 | __O_DIRECTORY)
#endif

#ifndef F_GETLK
# ifndef __USE_FILE_OFFSET64
#  define F_GETLK	5	/* Get record locking info.  */
#  define F_SETLK	6	/* Set record locking info (non-blocking).  */
#  define F_SETLKW	7	/* Set record locking info (blocking).  */
# else
#  define F_GETLK	F_GETLK64  /* Get record locking info.  */
#  define F_SETLK	F_SETLK64  /* Set record locking info (non-blocking).*/
#  define F_SETLKW	F_SETLKW64 /* Set record locking info (blocking).  */
# endif
#endif
#ifndef F_GETLK64
# define F_GETLK64	12	/* Get record locking info.  */
# define F_SETLK64	13	/* Set record locking info (non-blocking).  */
# define F_SETLKW64	14	/* Set record locking info (blocking).  */
#endif
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

static inline int in(uint16_t port) {
  int ret;
  asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
  return ret;
}

static inline uint32_t getNumExits(){
  uint32_t ret;
  ret = inb(0xE7);
  return ret;
}

static inline int open(const char *pathname, uint32_t flags, uint32_t mode){

	int ret;
	uint32_t  addr = (uintptr_t) pathname;
	printVal(HC_open,mode);
	printVal(HC_open,flags);
	printVal(HC_open,addr);

	ret = in(HC_open);
	return ret;
}
static inline int read(int fd, void *buf, size_t count){
	//uint32_t *addr = (uint32_t *) str;
	//char* ret;
		//uint32_t *addr =  &fd;
		char * char_buff =(char * )buf;
		printVal(HC_read,count);
	printVal(HC_read,fd);
	
	uint32_t bytes_read= inb(HC_read);
	for(uint32_t i =0;i<bytes_read;i++){
		char_buff[i] = inb(HC_read);
		//display(buff);
	}
	
	return bytes_read;
}
static inline int write(int fd, const void *buf, size_t count){
	//uint32_t *addr = (uint32_t *) str;
	//char* ret;
	int ret;
		uint32_t  addr = (uintptr_t) buf;
		printVal(HC_write,count);
		
	printVal(HC_write,addr);
	printVal(HC_write,fd);
	ret = inb(HC_write);
	return ret;

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
	uint32_t open_fd = open("demo_new",O_RDWR | O_CREAT,00700);
	printVal(0xE8,open_fd);
	char tmp[]="This is write demo";
	int bytes_written = write(open_fd,tmp,sizeof(tmp));
	printVal(0xE8,bytes_written);
	// write("this is test 1");
	// write("this is test 2");
	char read_buff[bytes_written];
	read(open_fd,read_buff,sizeof(read_buff));
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
