#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
  /*  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>*/

	
#define WRITE_PORT1 0xE9
#define WRITE_PORT2 0xE8

#define HC_status 0x8000
#define HC_open  (HC_status | 0)
#define HC_read  (HC_status | 1)
#define HC_write  (HC_status | 2)
#define HC_close  (HC_status | 3)
#define HC_seek  (HC_status | 4)



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

#  define S_ISVTX	__S_ISVTX


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

/* The possibilities for the third argument to `fseek'.
   These values should not be changed.  */
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */
#ifdef __USE_GNU
# define SEEK_DATA	3	/* Seek to next data.  */
# define SEEK_HOLE	4	/* Seek to next hole.  */
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
	asm volatile("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}



static inline void printVal(uint16_t port, uint32_t value) {
  asm volatile("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}



static void display(const char *str){
	

uint32_t  addr = (uintptr_t) str;
	printVal(0xE5,addr);
}

static inline uint32_t inb(uint16_t port) {
  uint32_t ret;
  asm volatile("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
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

uint32_t check_mode(uint32_t mode){
	uint32_t ret;
	switch (mode)
	{
	case S_IRWXU: ret =mode;
		break;
	
	case S_IRUSR: ret =mode;
		break;
	
	case S_IWUSR: ret =mode;
		break;

	case S_IXUSR: ret =mode;
		break;

	case S_IRWXG: ret =mode;
		break;
	
	case S_IRGRP: ret =mode;
		break;

	case S_IWGRP: ret =mode;
		break;

	case S_IXGRP: ret =mode;
		break;

	case S_IRWXO: ret =mode;
		break;

	case S_IROTH: ret =mode;
		break;

	case S_IWOTH: ret =mode;
		break;

	case S_IXOTH: ret =mode;
		break;

	case S_ISUID: ret =mode;
		break;

	case S_ISGID: ret =mode;
		break;

	case S_ISVTX: ret =mode;
		break;

	default:
		ret =-1;
		break;
	}
	return ret;
}
static inline int open(const char *pathname, uint32_t flags, ...){

	int ret;
	va_list arg;
	va_start(arg, flags); 
	uint32_t mode = va_arg(arg,int);
	uint32_t  addr = (uintptr_t) pathname;
	uint32_t check_mode_value =check_mode(mode); 
	printVal(HC_open,check_mode_value);
	printVal(HC_open,flags);
	printVal(HC_open,addr);
	
	ret = in(HC_open);
	va_end(arg);
	return ret;
}

static inline int close(uint32_t fd){
	int ret;
	printVal(HC_close,fd);
	ret = in(HC_close);
	return ret;

}
/*
simple reading byte by byte

static inline int read(int fd, void *buf, size_t count){
		char * char_buff =(char * )buf;
		printVal(HC_read,count);
	printVal(HC_read,fd);
	
	uint32_t bytes_read= inb(HC_read);
	 char char_buff_temp[bytes_read];

	for(uint32_t i =0;i<bytes_read;i++){
		char_buff[i] = inb(HC_read);
	}
	char_buff = char_buff_temp;
	return bytes_read;
}*/

static inline int read(int fd, void *buf, size_t count){
		//char * char_buff =(char * )buf;
		printVal(HC_read,count);
		uint32_t  addr = (uintptr_t) buf;
		printVal(HC_read,addr);
	printVal(HC_read,fd);
	

	uint32_t bytes_read= inb(HC_read);
	// char char_buff_temp[bytes_read];
	 
//	uint32_t * addr = (uint32_t*)(uintptr_t)inb(HC_read);
	

	//char_buff = (uint8_t)(uintptr_t)inb(HC_read);
	//char_buff[0]=char_buff[0];
	
	return bytes_read;
}
static inline int write(int fd, const void *buf, size_t count){

	int ret;
		uint32_t  addr = (uintptr_t) buf;
		printVal(HC_write,count);
		
	printVal(HC_write,addr);
	printVal(HC_write,fd);
	ret = inb(HC_write);
	return ret;

}

static inline int lseek(int fd, int offset, int whence){
	
		int ret;
		
		printVal(HC_seek,whence);
		printVal(HC_seek,offset);
		printVal(HC_seek,fd);
	ret = inb(HC_seek);
	return ret;

}


void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	int bytes_written;
	int open_fd,open_fd1,open_fd2,open_fd3;
	int bytes_read;
	char read_buff[1000];

/**
 * Port 0xE9 is often used by some emulators to directly send text to the hosts console.
 * */


	for (p = "Hello, world!\n"; *p; ++p)
		outb(WRITE_PORT1, *p);
	
	
	uint32_t numExits = getNumExits();
	display("Number of exits required to print Hello, world! : ");
	printVal(WRITE_PORT2,numExits-1);
	display("\nNumber of exits before diplay : ");

	uint32_t before = getNumExits();
		printVal(WRITE_PORT2,before);
	display("\n HI VM  from guest\n");
	uint32_t after = getNumExits();
		
	display("\nNumber of exits after diplay : ");
	printVal(WRITE_PORT2,after);
	

	open_fd = open("demo_new",O_RDWR | O_CREAT | O_APPEND,S_IRWXU);
	if(open_fd >=0){
	display("\n\nOpened file demo_new with fd : ");
	printVal(WRITE_PORT2,open_fd);
	}
			else{
display("\nError while opening file demo_new");
		}
	
	char tmp[]="This is write demo ";
	 bytes_written= write(open_fd,tmp,sizeof(tmp)-1);
	 if(bytes_written >=0){
	display("\nWrite ");
	printVal(WRITE_PORT2,bytes_written);
	display(" bytes in demo_new\n");
	 }
	 		else{
display("\nError while writing to file demo_new");
		}

	lseek(open_fd,0,SEEK_SET);	
	bytes_read=read(open_fd,read_buff,sizeof(read_buff));
	if(bytes_read >=0)
	{
	display("\nRead ");
	printVal(WRITE_PORT2,bytes_read);
	display(" bytes from demo_new\n");
	display(read_buff);
	}
			else{
display("\nError while reading from  file demo_new_1");
		}
	// close(open_fd);
//	char read_buff1[100];
	open_fd1=open("demo_new_1",O_RDWR | O_CREAT | O_APPEND,S_IRWXU);
	if(open_fd1 >=0){
	display("\n\nOpened file demo_new_1 with fd : ");
	printVal(WRITE_PORT2,open_fd1);
	}
			else{
display("\nError while opening file demo_new_1");
		}
	char tmp1[]="This is write demo 1";
	bytes_written = write(open_fd1,tmp1,sizeof(tmp1)-1);
	if(bytes_written >= 0){
	display("\nWrite ");
	printVal(WRITE_PORT2,bytes_written);
	display(" bytes in demo_new_1\n");
	}
			else{
display("\nError while writing to file demo_new_1");
		}
	lseek(open_fd1,0,SEEK_SET);
	bytes_read=read(open_fd1,read_buff,sizeof(read_buff));
	if(bytes_read >=0){
			display("\nRead ");
	printVal(WRITE_PORT2,bytes_read);
	display(" bytes from demo_new_1\n");
	display(read_buff);
	}
			else{
display("\nError while reading from file demo_new_1");
		}







		open_fd2=open("demo_new_1",O_RDWR | O_TRUNC);
		if(open_fd2 > 0){
	display("\n\nOpened file demo_new_1 without giving mode with fd and trucating previous data : ");
	printVal(WRITE_PORT2,open_fd2);
		}
		else{
display("\nError while opening file demo_new_1");
		}
		
	
	
char tmp3[] = "This is write demo new";
	 bytes_written= write(open_fd2,tmp3,sizeof(tmp3)-1);
	 if(bytes_written >=0){
	display("\nWrite ");
	printVal(WRITE_PORT2,bytes_written);
	display(" bytes in demo_new_1\n");
	 }
	 		else{
display("\nError while writing to file demo_new");
		}
			lseek(open_fd2,0,SEEK_SET);
			bytes_read=read(open_fd2,read_buff,sizeof(read_buff));
	if(bytes_read >=0)
	{
	display("\nRead ");
	printVal(WRITE_PORT2,bytes_read);
	display(" bytes from demo_new_1\n");
	display(read_buff);
	}
			else{
display("\nError while reading from  file demo_new_1");
		}
	



		close(open_fd);
	close(open_fd1);
		close(open_fd2);

	display("\n Closed ");
	printVal(WRITE_PORT2,open_fd);
	display(" , ");
	printVal(WRITE_PORT2,open_fd1);
	display(" , ");
	printVal(WRITE_PORT2,open_fd2);


// No append

		open_fd3=open("demo_new_2",O_RDWR | O_CREAT,S_IRWXU);
		if(open_fd3 > 0){
	display("\n\nOpened file demo_new_2 with no append: ");
	printVal(WRITE_PORT2,open_fd3);
		}
		else{
display("\nError while opening file demo_new_2");
		}

char tmp4[] = "This is write demo 2";
	 bytes_written= write(open_fd3,tmp4,sizeof(tmp4)-1);
	 if(bytes_written >=0){
	display("\nWrite ");
	printVal(WRITE_PORT2,bytes_written);
	display(" bytes in demo_new 2\n");
	 }
	 		else{
display("\nError while writing to file demo_new 2");
		}
			lseek(open_fd3,0,SEEK_SET);
			bytes_read=read(open_fd3,read_buff,sizeof(read_buff));
	if(bytes_read >=0)
	{
	display("\nRead ");
	printVal(WRITE_PORT2,bytes_read);
	display(" bytes from demo_new 2\n");
	display(read_buff);
	}
			else{
display("\nError while reading from  file demo_new_2");
		}
		close(open_fd3);
		display("\n Closed ");
	printVal(WRITE_PORT2,open_fd3);

/*
	display("\n\n Now we will try to operate on random fd that is not created by our guest \n\n");

		

	 bytes_written= write(15,tmp4,sizeof(tmp4)-1);
	 if(bytes_written >=0){
	display("\nWrite ");
	printVal(WRITE_PORT2,bytes_written);
	display(" bytes in demo_new 2\n");
	 }
	 		else{
display("\nError while writing to file demo_new 2 \n");
		}
			lseek(16,0,SEEK_SET);
			bytes_read=read(17,read_buff,sizeof(read_buff));
	if(bytes_read >=0)
	{
	display("\nRead ");
	printVal(WRITE_PORT2,bytes_read);
	display(" bytes from demo_new 2\n");
	display(read_buff);
	}
			else{
display("\nError while reading from  file demo_new_2 \n");
		}
		close(18);
*/
	


/**
 * Q8  Can you figure out what's happening with the number 42? 
 * Where is it written in the guest and where is it read out in the hypervisor? 
 * Ans : It is written below
 * */
	*(long *) 0x400 = 42;


	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
