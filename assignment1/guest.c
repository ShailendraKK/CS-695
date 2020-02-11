#include <stddef.h>
#include <stdint.h>
  /*  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>*/

#define HC_status 0x8000
#define HC_open  (HC_status | 0)
#define HC_read  (HC_status | 1)
#define HC_write  (HC_status | 2)
#define HC_close  (HC_status | 3)



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


/* Protection bits.  */

# define S_ISUID	__S_ISUID       /* Set user ID on execution.  */
# define S_ISGID	__S_ISGID       /* Set group ID on execution.  */

# if defined __USE_MISC || defined __USE_XOPEN
/* Save swapped text after use (sticky bit).  This is pretty well obsolete.  */
#  define S_ISVTX	__S_ISVTX
# endif

# define S_IRUSR	__S_IREAD       /* Read by owner.  */
# define S_IWUSR	__S_IWRITE      /* Write by owner.  */
# define S_IXUSR	__S_IEXEC       /* Execute by owner.  */
/* Read, write, and execute by owner.  */
# define S_IRWXU	(__S_IREAD|__S_IWRITE|__S_IEXEC)

# define S_IRGRP	(S_IRUSR >> 3)  /* Read by group.  */
# define S_IWGRP	(S_IWUSR >> 3)  /* Write by group.  */
# define S_IXGRP	(S_IXUSR >> 3)  /* Execute by group.  */
/* Read, write, and execute by group.  */
# define S_IRWXG	(S_IRWXU >> 3)

# define S_IROTH	(S_IRGRP >> 3)  /* Read by others.  */
# define S_IWOTH	(S_IWGRP >> 3)  /* Write by others.  */
# define S_IXOTH	(S_IXGRP >> 3)  /* Execute by others.  */
/* Read, write, and execute by others.  */
# define S_IRWXO	(S_IRWXG >> 3)

#ifdef	__USE_MISC
# ifndef R_OK			/* Verbatim from <unistd.h>.  Ugh.  */
/* Values for the second argument to access.
   These may be OR'd together.  */
#  define R_OK	4		/* Test for read permission.  */
#  define W_OK	2		/* Test for write permission.  */
#  define X_OK	1		/* Test for execute permission.  */
#  define F_OK	0		/* Test for existence.  */
# endif
#endif /* Use misc.  */

/* XPG wants the following symbols.   <stdio.h> has the same definitions.  */
#if defined __USE_XOPEN || defined __USE_XOPEN2K8
# define SEEK_SET	0	/* Seek from beginning of file.  */
# define SEEK_CUR	1	/* Seek from current position.  */
# define SEEK_END	2	/* Seek from end of file.  */
#endif	/* XPG */


/* Encoding of the file mode.  */

#define	__S_IFMT	0170000	/* These bits determine file type.  */

/* File types.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define	__S_IFIFO	0010000	/* FIFO.  */
#define	__S_IFLNK	0120000	/* Symbolic link.  */
#define	__S_IFSOCK	0140000	/* Socket.  */

/* POSIX.1b objects.  Note that these macros always evaluate to zero.  But
   they do it by enforcing the correct use of the macros.  */
#define __S_TYPEISMQ(buf)  ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSEM(buf) ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSHM(buf) ((buf)->st_mode - (buf)->st_mode)

/* Protection bits.  */

#define	__S_ISUID	04000	/* Set user ID on execution.  */
#define	__S_ISGID	02000	/* Set group ID on execution.  */
#define	__S_ISVTX	01000	/* Save swapped text after use (sticky).  */
#define	__S_IREAD	0400	/* Read by owner.  */
#define	__S_IWRITE	0200	/* Write by owner.  */
#define	__S_IEXEC	0100	/* Execute by owner.  */

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

static inline int close(uint32_t fd){
	int ret;
	printVal(HC_close,fd);
	ret = in(HC_close);
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
	uint32_t open_fd = open("demo_new",O_RDWR | O_CREAT | O_APPEND,S_IRWXU);
	printVal(0xE8,open_fd);
	

	char tmp[]="This is write demo";
	int bytes_written = write(open_fd,tmp,sizeof(tmp)-1);
	printVal(0xE8,bytes_written);
	char read_buff[100];
	read(open_fd,read_buff,sizeof(read_buff));
	display(read_buff);
	uint32_t status = close(open_fd);
	printVal(0xE8,status);

	// write("this is test 1");
	// write("this is test 2");
	
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
