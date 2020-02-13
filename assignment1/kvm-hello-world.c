#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <linux/kvm.h>

/**
 *  CR0 bits 
 * 
It sets bitmasks:
1U << 0 = 1  	binary: 00000000000000000000000000000001
1U << 1 = 2   
1U << 2 = 4

 * */	
#define CR0_PE 1u
#define CR0_MP (1U << 1)
#define CR0_EM (1U << 2)   
#define CR0_TS (1U << 3)
#define CR0_ET (1U << 4)
#define CR0_NE (1U << 5)
#define CR0_WP (1U << 16)
#define CR0_AM (1U << 18)
#define CR0_NW (1U << 29)
#define CR0_CD (1U << 30)
#define CR0_PG (1U << 31)

/* CR4 bits */
#define CR4_VME 1
#define CR4_PVI (1U << 1)
#define CR4_TSD (1U << 2)
#define CR4_DE (1U << 3)
#define CR4_PSE (1U << 4)
#define CR4_PAE (1U << 5)
#define CR4_MCE (1U << 6)
#define CR4_PGE (1U << 7)
#define CR4_PCE (1U << 8)
#define CR4_OSFXSR (1U << 8)
#define CR4_OSXMMEXCPT (1U << 10)
#define CR4_UMIP (1U << 11)
#define CR4_VMXE (1U << 13)
#define CR4_SMXE (1U << 14)
#define CR4_FSGSBASE (1U << 16)
#define CR4_PCIDE (1U << 17)
#define CR4_OSXSAVE (1U << 18)
#define CR4_SMEP (1U << 20)
#define CR4_SMAP (1U << 21)

#define EFER_SCE 1
#define EFER_LME (1U << 8)
#define EFER_LMA (1U << 10)
#define EFER_NXE (1U << 11)

/* 32-bit page directory entry bits */
#define PDE32_PRESENT 1
#define PDE32_RW (1U << 1)
#define PDE32_USER (1U << 2)
#define PDE32_PS (1U << 7)

/* 64-bit page * entry bits */
#define PDE64_PRESENT 1
#define PDE64_RW (1U << 1)
#define PDE64_USER (1U << 2)
#define PDE64_ACCESSED (1U << 5)
#define PDE64_DIRTY (1U << 6)
#define PDE64_PS (1U << 7)
#define PDE64_G (1U << 8)





#define HC_status 0x8000
#define HC_open  (HC_status | 0)
#define HC_read  (HC_status | 1)
#define HC_write  (HC_status | 2)
#define HC_close  (HC_status | 3)
#define HC_seek  (HC_status | 4)

#define MAX_FD 100
enum open_param{pathname,flags,mode};
enum read_write{fd,buf,count};


struct vm {
	int sys_fd;
	int fd;
	char *mem;
};

/**
 * What is the size of the guest memory (that the guest perceives as its physical memory) that is setup in the function vm_init
 * */

void vm_init(struct vm *vm, size_t mem_size)
{
	/**
	 * Q1.1
	 * What is the size of the guest memory (that the guest perceives as its physical memory) 
	 * that is setup in the function vm_init?
	 * Ans:
	 * 2MB
	 * */
	
	printf("the size of the guest memory (that the guest perceives as its physical memory) : %ld bytes \n",mem_size);
	int api_ver;
	struct kvm_userspace_memory_region memreg;

	/**
	 * 
	 * First, we'll need to open /dev/kvm:

    kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
We need read-write access to the device to set up a virtual machine,
and all opens not explicitly intended for inheritance across exec should use O_CLOEXEC.
	 */ 

	vm->sys_fd = open("/dev/kvm", O_RDWR);
	if (vm->sys_fd < 0) {
		perror("open /dev/kvm");
		exit(1);
	}

	/**
	 * Before you use the KVM API, you should make sure you have a version you can work with.
	 * Early versions of KVM had an unstable API with an increasing version number, 
	 * but the KVM_API_VERSION last changed to 12 with Linux 2.6.22 in April 2007, 
	 * and got locked to that as a stable interface in 2.6.24
	 */

	api_ver = ioctl(vm->sys_fd, KVM_GET_API_VERSION, 0);
	if (api_ver < 0) {
		perror("KVM_GET_API_VERSION");
		exit(1);
	}

	if (api_ver != KVM_API_VERSION) {
		fprintf(stderr, "Got KVM api version %d, expected %d\n",
			api_ver, KVM_API_VERSION);
		exit(1);
	}


	/**
	 *KVM gives us a handle to this VM in the form of a file descriptor:
	 */
	vm->fd = ioctl(vm->sys_fd, KVM_CREATE_VM, 0);
	if (vm->fd < 0) {
		perror("KVM_CREATE_VM");
		exit(1);
	}

	/***
	 * 
	 * KVM_SET_TSS_ADDR
	 * This ioctl defines the physical address of a three-page region in the guest
physical address space.  The region must be within the first 4GB of the
guest physical address space and must not conflict with any memory slot
or any mmio address.  The guest may malfunction if it accesses this memory
region.offset : not needed since file mapping is not used.

This ioctl is required on Intel-based hosts.  This is needed on Intel hardware
because of a quirk in the virtualization implementation
*/
        if (ioctl(vm->fd, KVM_SET_TSS_ADDR, 0xfffbd000) < 0) {
                perror("KVM_SET_TSS_ADDR");
		exit(1);
	}

/**
 *  void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
	
       If addr is NULL, then the kernel chooses the (page-aligned) address
       at which to create the mapping; this is the most portable method of
       creating a new mapping.

		PROT_READ  Pages may be read.

       	PROT_WRITE Pages may be written.

		MAP_PRIVATE
              Create a private copy-on-write mapping.  Updates to the
              mapping are not visible to other processes mapping the same
              file, and are not carried through to the underlying file.
 * 		MAP_ANONYMOUS
              The mapping is not backed by any file; its contents are
              initialized to zero.  The fd argument is ignored; however,
              some implementations require fd to be -1 if MAP_ANONYMOUS (or
              MAP_ANON) is specified, and portable applications should
              ensure this.
		
       MAP_NORESERVE
              Do not reserve swap space for this mapping.  When swap space
              is reserved, one has the guarantee that it is possible to mod‐
              ify the mapping.  When swap space is not reserved one might
              get SIGSEGV upon a write if no physical memory is available.

		offset : not needed since file mapping is not used.
		
 * 
 */



	vm->mem = mmap(NULL, mem_size, PROT_READ | PROT_WRITE,
		   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);

	/**
	 * 
	 * 	Q3.2 Range of guest code in host virtual address space
	 * */
	printf("\n Host memory mapping for guest code: %p to %p \n",vm->mem,vm->mem + mem_size);

	if (vm->mem == MAP_FAILED) {
		perror("mmap mem");
		exit(1);
	}

/**
 * Q1.3
 * At what virtual address is this memory mapped into the virtual address space of this simple hypervisor?
 * Ans:
 * */
	printf("Virtual address where the VAS of this simple hypervisor is memory mapped %p\n",vm->mem);


	
/***
 *	 The madvise() system call is used to give advice or directions to the
       kernel about the address range beginning at address addr and with
       size length bytes In most cases, the goal of such advice is to
       improve system or application performance.
 * 
 *  MADV_MERGEABLE :
              Enable Kernel Samepage Merging (KSM) for the pages in the
              range specified by addr and length.  The kernel regularly
              scans those areas of user memory that have been marked as
              mergeable, looking for pages with identical content.  These
              are replaced by a single write-protected page (which is
              automatically copied if a process later wants to update the
              content of the page).  KSM merges only private anonymous pages 
 */
	madvise(vm->mem, mem_size, MADV_MERGEABLE);


/**
 * 
 * Q1.2
 * How and where in the hyprevisor code is this guest memory allocated from the host OS?
 * Ans: 
 * */

/**
 *The slot field provides an integer index identifying each region of memory we hand to KVM; 
 calling KVM_SET_USER_MEMORY_REGION again with the same slot will replace this mapping, 
 while calling it with a new slot will create a separate mapping.
  guest_phys_addr specifies the base "physical" address as seen from the guest, 
  and userspace_addr points to the backing memory in our process that we allocated with mmap();
note that these always use 64-bit values, even on 32-bit platforms. 
memory_size specifies how much memory to map: one page, 0x1000 bytes i.e 4096 Bytes i.e 4KB
therefor 0x200000 = 2MB.
 */



	memreg.slot = 0;
	memreg.flags = 0;
	memreg.guest_phys_addr = 0;
	memreg.memory_size = mem_size;
	memreg.userspace_addr = (unsigned long)vm->mem;
        if (ioctl(vm->fd, KVM_SET_USER_MEMORY_REGION, &memreg) < 0) {
		perror("KVM_SET_USER_MEMORY_REGION");
                exit(1);
	}

}

struct vcpu {
	int fd;
	struct kvm_run *kvm_run;
};
/*
static int fd_list[100];
static int fd_use[100];


void init_fd(){
	static int first_time_init=0;
	if(first_time_init ==0)
	{
		first_time_init =1;
	for(int i=0;i<=2;i++){
		fd_list[i]=i;
		fd_use[i]=1;
	}
   for(int i=3;i<MAX_FD;i++){
	   fd_use[i]=0;

   }
}


}
int find_min_fd(){
	for(int i=3;i<MAX_FD;i++){
		if(fd_use[i]==0){
			return i;
		}
	}
	return -1;
}
*/
static void hyper_close(struct vcpu *vcpu)
{

	static int ret_status=0;
	static int fd;
	uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);

	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
		
		fd = *offset;
		ret_status =close(fd);

	}
	else{
		
		*offset = ret_status;
	}
	
}
static void hyper_open(struct vm *vm,struct vcpu *vcpu)
{
	static int ret_fd=0;
	static int param_num =2;
	char* filename;
	
	static int open_flags;
	static int open_mode;
	// void init_fd();
	
	uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);

	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
				
		switch (param_num)
		{
		case pathname:
		filename = &vm->mem[*offset];
		// int min_fd = find_min_fd();
		// if(min_fd==-1){
		// 	perror("KVM_IO");
		// 	exit(1);
		// }

		// int fd = open(filename,O_RDWR | O_CREAT,00700);
		
		if(open_mode > 0)
		{
		 ret_fd= open(filename,open_flags,open_mode);
		}
		 else
		 {
			ret_fd = open(filename,open_flags);
		 }
		 
		
		if(fd<0){
			perror("KVM_IO");
			
			
		}
			
		
		param_num =2;
			break;
		
		case flags:
			open_flags = *offset;
			param_num--;
			break;
		case mode:
			open_mode = *offset;
			param_num--;
			break;

		default:
			break;
		}
		


	}
	else{
		
			*offset = ret_fd;
			

	
	}
	
}
/* simple read sending data one by one
static void hyper_read(struct vcpu *vcpu)
{
			static int read_fd;
		static size_t read_count;
		static int param_num=1;
		static int i=0;
		static int bytes_read;
		static char * data;
		static int flag=0;
	uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);

	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
		
		flag=1;
		i=0;
		switch (param_num)
		{
		case fd:
					read_fd = *offset;
					data = (char *) malloc(read_count * sizeof(char));

					bytes_read=read(read_fd,data,read_count);
					param_num = 1;
					if(bytes_read < 0){
					perror("KVM_IO");
						exit(1);
					}
			break;
		

		case (count-1):
			read_count = *offset;
			param_num --;
		break;
		
		default:
			break;
		}


	}
	else{
				if(flag ==2){
			*offset = data[i];
			i++;
		}
		if(flag ==1){
		flag =2;	
		*offset = bytes_read;
		}

	}
}
*/

static void hyper_read(struct vm *vm,struct vcpu *vcpu)
{
	
			static int read_fd;
		static size_t read_count;
		static int param_num=2;
		static int bytes_read;
		static char * data;
	uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);

	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
		
		//flag=1;
		//i=0;
		switch (param_num)
		{
		case fd:
					read_fd = *offset;
					memset(data,0,read_count);
					bytes_read=read(read_fd,data,read_count);
					param_num = 2;
					if(bytes_read < 0){
					perror("KVM_IO");
						
					}
			break;
		
		case buf:
				data = &vm->mem[*offset];
				param_num --;
		break;
		

		case (count):
			read_count = *offset;
			param_num --;
		break;
		
		default:
			break;
		}

	}
	else{
		
		*offset = bytes_read;
		

	}
}

static void hyper_write(struct vm *vm,struct vcpu *vcpu)
{
		static int write_fd;
		static size_t write_count;
		static int param_num=2;
		static int bytes_written;
		static char * data;
uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);
	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
		
		switch (param_num)
		{
				
		
		case fd:
			write_fd = *offset;
			
					bytes_written=write(write_fd,data,write_count);
					param_num = 2;
		if(bytes_written < 0){
			perror("KVM_IO");
		}
		break;
		case buf:
					data = &vm->mem[*offset];
					param_num --;

		break;

		case count:
			write_count = *offset;
			param_num --;
		break;


		
		default:
			break;
		}
		

	}
	else{
		*offset = bytes_written;
	}
	
}

static void hyper_seek(struct vcpu *vcpu)
{
		static int seek_fd;
		static int seek_offset;
			static int whence;
		static int param_num=2;
		static int ret;
	
uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);
	if(vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT){
		
		switch (param_num)
		{
				
		
		case fd:
			seek_fd = *offset;
			
					ret=lseek(seek_fd,seek_offset,whence);
					param_num = 2;
		if(ret < 0){
			perror("KVM_IO");
			
		}
		break;
		case buf:
					seek_offset = *offset;
					param_num --;

		break;

		case count:
			whence = *offset;
			param_num --;
		break;


		
		default:
			break;
		}
		

	}
	else{
		*offset = ret;
	}
	
}
void vcpu_init(struct vm *vm, struct vcpu *vcpu)
{

	int vcpu_mmap_size;

/**
 * 
 *  KVM gives us a handle to this VCPU in the form of a file descriptor:
 * The 0 here represents a sequential virtual CPU index.
 *  A VM with multiple CPUs would assign a series of small identifiers here, 
 * from 0 to a system-specific limit (obtainable by checking the KVM_CAP_MAX_VCPUS capability with KVM_CHECK_EXTENSION).
 */ 
	vcpu->fd = ioctl(vm->fd, KVM_CREATE_VCPU, 0);
        if (vcpu->fd < 0) {
		perror("KVM_CREATE_VCPU");
                exit(1);
	}

/**
 *  We map kvm_run structure into user space using mmap(), 
 * but first, we need to know how much memory to map, 
 * which KVM tells us with the KVM_GET_VCPU_MMAP_SIZE ioctl().
 * Note that the mmap size typically exceeds that of the kvm_run structure,
 * as the kernel will also use that space to store other transient structures that kvm_run may point to.
 */
	vcpu_mmap_size = ioctl(vm->sys_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
        if (vcpu_mmap_size <= 0) {
		perror("KVM_GET_VCPU_MMAP_SIZE");
                exit(1);
	}
/**
 * Q2.1  What is the size of VCPU?
 * Ans: 12KB
 *  */
	printf("Size of VCPU Mmap area : %d bytes \n",vcpu_mmap_size);

/***
 * 
       MAP_SHARED
              Share this mapping.  Updates to the mapping are visible to
              other processes mapping the same region, and (in the case of
              file-backed mappings) are carried through to the underlying
              file.
		since kvm_run is used to exchange information between the QEMU userspace process and KVM
 */

/**
 * Q2.1 In which lines of the program is this memory allocated
 * Ans: In below line
 * */
	
	vcpu->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, vcpu->fd, 0);
	if (vcpu->kvm_run == MAP_FAILED) {
		perror("mmap kvm_run");
		exit(1);
	}

/**
 * Q2.3  where is VCPU mmap located in the virutal address space of the hypervisor ?
 * 
 * */
	printf("Location of VCPU mmap area in virtual address space of hypervisor : %p\n",vcpu->kvm_run);		
}

int run_vm(struct vm *vm, struct vcpu *vcpu, size_t sz)
{
	struct kvm_regs regs;
	uint32_t numExits=0;
	uint64_t memval = 0;
	for (;;) {
		/**
		 * Q6 At which line in the hypervisor program does the control switch from running the hypervisor to 
		 * running the guest? 
		 * Ans : below line
		 * */
		if (ioctl(vcpu->fd, KVM_RUN, 0) < 0) {
			perror("KVM_RUN");
			exit(1);
		}
		/**
		 * Q6 At which line does the control switch back to the hypervisor from the guest? 
		 * Ans: below line
		 * */
		switch (vcpu->kvm_run->exit_reason) {
		case KVM_EXIT_HLT:
		printf("\n In KVM HLT\n");
			 numExits++;
			goto check;


		/**
		 * 
		 * To let the virtualized code output its result, 
		 * we emulate a serial port on I/O port 0xE9. 
		 * Fields in run->io indicate the direction (input or output), 
		 * the size (1, 2, or 4), the port, and the number of values. 
		 * To pass the actual data, the kernel uses a buffer mapped after the kvm_run structure,
		 *  and kvm_run->io.data_offset provides the offset from the start of that mapping.
		 */
		case KVM_EXIT_IO:
		/**
		 * Q7. 
		 * 
		 * How can you read the port number and the value written to the port within the hypervisor?
		 * Ans: Using IO exit as below
		 * 
		 * Which memory buffer is used to communicate the value written by the guest to the hypervisor?
		 * Ans: kvm_run
		 * 
		 * How many exits (from guest to hypervisor) are required to print out the complete string
		 * Ans: 14
		 * 
		 * */
		 numExits++;
	
			if (vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT
			    && vcpu->kvm_run->io.port == 0xE9) {
				char *p = (char *)vcpu->kvm_run;
				fwrite(p + vcpu->kvm_run->io.data_offset,
				       vcpu->kvm_run->io.size, 1, stdout);
				fflush(stdout);
				continue;
			}
				if (vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT
			    && vcpu->kvm_run->io.port == 0xE8) {
				uint32_t *offset = (uint32_t*)((uint8_t*)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);

				printf("%d",*offset);

				continue;
			}
			if (vcpu->kvm_run->io.direction == KVM_EXIT_IO_OUT
			    && vcpu->kvm_run->io.port == 0xE5) {
					uint32_t *offset = (uint32_t*)((uintptr_t)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);
					printf("%s",(&vm->mem[*offset]));
			
				continue;
			}
			if (vcpu->kvm_run->io.direction == KVM_EXIT_IO_IN
			    && vcpu->kvm_run->io.port == 0xE7) {
				uint32_t *offset = (uint32_t*)((uint8_t*)vcpu->kvm_run + vcpu->kvm_run->io.data_offset);
				*offset = numExits;
				continue;
			}
					if(vcpu->kvm_run->io.port & HC_status){
				switch(vcpu ->kvm_run->io.port){
					case HC_open: hyper_open(vm,vcpu);
									continue;
					case HC_read: 
									hyper_read(vm,vcpu);
									continue;
					case HC_write: 	
									hyper_write(vm,vcpu);
									continue;
					case HC_close: hyper_close(vcpu);
									continue;
					case HC_seek : hyper_seek(vcpu);
									continue;
				}		
			}
			/* fall through */
		default:
		printf("\n In Default\n");
			numExits++;
			fprintf(stderr,	"Got exit_reason %d,"
				" expected KVM_EXIT_HLT (%d)\n",
				vcpu->kvm_run->exit_reason, KVM_EXIT_HLT);
			exit(1);
		}
	}

 check:
	if (ioctl(vcpu->fd, KVM_GET_REGS, &regs) < 0) {
		perror("KVM_GET_REGS");
		exit(1);
	}

	/**
	 * Q8  Can you figure out what's happening with the number 42?
	 * Where is it written in the guest and where is it read out in the hypervisor? 
	 * Ans: Number 42 is read in hypervisor here
	 * It is generally used to check whether the exit is done manually by us since we are storing value 42 at location 0x400 and reg a
	 * and checking here that the value is present at both the location. It is used to exit from the loop and hlt the guest.
	 * */
	if (regs.rax != 42) {
		printf("Wrong result: {E,R,}AX is %lld\n", regs.rax);
		return 0;
	}

	memcpy(&memval, &vm->mem[0x400], sz);
	if (memval != 42) {
		printf("Wrong result: memory at 0x400 is %lld\n",
		       (unsigned long long)memval);
		return 0;
	}

	return 1;
}

extern const unsigned char guest16[], guest16_end[];

int run_real_mode(struct vm *vm, struct vcpu *vcpu)
{
	struct kvm_sregs sregs;
	struct kvm_regs regs;

	printf("Testing real mode\n");

        if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

	sregs.cs.selector = 0;
	sregs.cs.base = 0;

        if (ioctl(vcpu->fd, KVM_SET_SREGS, &sregs) < 0) {
		perror("KVM_SET_SREGS");
		exit(1);
	}
	
	memset(&regs, 0, sizeof(regs));
	/* Clear all FLAGS bits, except bit 1 which is always set. */
	regs.rflags = 2;
	regs.rip = 0;

	if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

	memcpy(vm->mem, guest16, guest16_end-guest16);
	return run_vm(vm, vcpu, 2);
}

static void setup_protected_mode(struct kvm_sregs *sregs)
{
	struct kvm_segment seg = {
		.base = 0,
		.limit = 0xffffffff,
		.selector = 1 << 3,
		.present = 1,
		.type = 11, /* Code: execute, read, accessed */
		.dpl = 0,
		.db = 1,
		.s = 1, /* Code/data */
		.l = 0,
		.g = 1, /* 4KB granularity */
	};

	sregs->cr0 |= CR0_PE; /* enter protected mode */

	sregs->cs = seg;

	seg.type = 3; /* Data: read/write, accessed */
	seg.selector = 2 << 3;
	sregs->ds = sregs->es = sregs->fs = sregs->gs = sregs->ss = seg;
}

extern const unsigned char guest32[], guest32_end[];

int run_protected_mode(struct vm *vm, struct vcpu *vcpu)
{
	struct kvm_sregs sregs;
	struct kvm_regs regs;

	printf("Testing protected mode\n");

        if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

	setup_protected_mode(&sregs);

        if (ioctl(vcpu->fd, KVM_SET_SREGS, &sregs) < 0) {
		perror("KVM_SET_SREGS");
		exit(1);
	}

	memset(&regs, 0, sizeof(regs));
	/* Clear all FLAGS bits, except bit 1 which is always set. */
	regs.rflags = 2;
	regs.rip = 0;

	if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

	memcpy(vm->mem, guest32, guest32_end-guest32);
	return run_vm(vm, vcpu, 4);
}

static void setup_paged_32bit_mode(struct vm *vm, struct kvm_sregs *sregs)
{
	uint32_t pd_addr = 0x2000;
	uint32_t *pd = (void *)(vm->mem + pd_addr);

	/* A single 4MB page to cover the memory region */
	pd[0] = PDE32_PRESENT | PDE32_RW | PDE32_USER | PDE32_PS;
	/* Other PDEs are left zeroed, meaning not present. */

	sregs->cr3 = pd_addr;
	sregs->cr4 = CR4_PSE;
	sregs->cr0
		= CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
	sregs->efer = 0;
}

int run_paged_32bit_mode(struct vm *vm, struct vcpu *vcpu)
{
	struct kvm_sregs sregs;
	struct kvm_regs regs;

	printf("Testing 32-bit paging\n");

        if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

	setup_protected_mode(&sregs);
	setup_paged_32bit_mode(vm, &sregs);

        if (ioctl(vcpu->fd, KVM_SET_SREGS, &sregs) < 0) {
		perror("KVM_SET_SREGS");
		exit(1);
	}

	memset(&regs, 0, sizeof(regs));
	/* Clear all FLAGS bits, except bit 1 which is always set. */
	regs.rflags = 2;
	regs.rip = 0;

	if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

	memcpy(vm->mem, guest32, guest32_end-guest32);
	return run_vm(vm, vcpu, 4);
}

extern const unsigned char guest64[], guest64_end[];

static void setup_64bit_code_segment(struct kvm_sregs *sregs)
{
	struct kvm_segment seg = {
		.base = 0,
		.limit = 0xffffffff,
		.selector = 1 << 3,
		.present = 1,
		.type = 11, /* Code: execute, read, accessed */
		.dpl = 0,
		.db = 0,
		.s = 1, /* Code/data */
		.l = 1,
		.g = 1, /* 4KB granularity */
	};

	sregs->cs = seg;
	/**
	 * Q3.2 Range of guest code
	 * */
	printf("\n Range of guest code: %lld to %d \n",sregs->cs.base,seg.limit);

	seg.type = 3; /* Data: read/write, accessed */
	seg.selector = 2 << 3;
	sregs->ds = sregs->es = sregs->fs = sregs->gs = sregs->ss = seg;
}

static void setup_long_mode(struct vm *vm, struct kvm_sregs *sregs)
{
	/**
	 * outermost page table, called the PML4 (page map level 4)
	 * size 4KB
	 */
	uint64_t pml4_addr = 0x2000;
	uint64_t *pml4 = (void *)(vm->mem + pml4_addr);

	/**
	 * PDP (page directory pointer) table
	 * size 4KB
	 */
	uint64_t pdpt_addr = 0x3000;
	uint64_t *pdpt = (void *)(vm->mem + pdpt_addr);

	/**
	 * pgdir (page directory) table
	 * size 4KB
	 */

	uint64_t pd_addr = 0x4000;
	uint64_t *pd = (void *)(vm->mem + pd_addr);

	/**
	 * There are some control bits records in the tables, 
	 * including if the page is mapped, is writable, and can be accessed in user-mode.
	 *  e.g. 3 (PDE64_PRESENT|PDE64_RW) stands for the memory is mapped and writable, 
	 * and 0x80 (PDE64_PS) stands for it's 2M paging instead of 4K. 
	 *
	 * As a result, these page tables can map address below 0x200000 to itself
	 * (i.e. virtual address equals to physical address).
	 * 
	 * And another difference is the permission setting in page tables. 
	 * In the above example if I set all entries as non-user-accessible:
  		pml4[0] = 3 | pdpt_addr; -- PDE64_PRESENT | PDE64_RW | pdpt_addr
  		pdpt[0] = 3 | pd_addr; -- PDE64_PRESENT | PDE64_RW | pd_addr
  		pd[0] = 3 | 0x80; -- PDE64_PRESENT | PDE64_RW | PDE64_PS
		  
	 If kernel wants to create virtual memory for user-space, 
	 such as handling mmap syscall from user, the page tables must set the 3rd bit, 
	 i.e. have bit (1 << 2) set, then the page can be accessed in user-space. For example,
  		pml4[0] = 7 | pdpt_addr; -- PDE64_USER | PDE64_PRESENT | PDE64_RW | pdpt_addr
 		 pdpt[0] = 7 | pd_addr; -- PDE64_USER | PDE64_PRESENT | PDE64_RW | pd_addr
  		pd[0] = 7 | 0x80; -- PDE64_USER | PDE64_PRESENT | PDE64_RW | PDE64_PS
		This is just an example, we should NOT set user-accessible pages in hypervisor,
		 user-accessible pages should be handled by our kernel.
	 */ 
	/**
	 * Q3.1. Guest page table setup --  This is 2M paging, with the PT (page table) removed, 
	 * the PDT entries point to physical address.
	 * */
	/**
	 * Q4 Understand the structure of the quest page table, 
	 * and how the guest virtual address space is mapped to its physical address space using its page table.
	 * How many levels does the guest page table have in long mode? 
	 * Ans: 4 levels since PDT directly points to physical address.
	 *  How many pages does it occupy? 
	 * Ans: 512 pages   
	 * */
	pml4[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pdpt_addr;
	pdpt[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | pd_addr;
	pd[0] = PDE64_PRESENT | PDE64_RW | PDE64_USER | PDE64_PS;

	printf("\n plm4 : %ld \n",*pml4);
	printf("\n pdpt : %ld \n",*pdpt);
	printf("\n pd : %ld \n",*pd);

	/**
	 *   sregs->cr3 = pml4_addr;
  		sregs->cr4 = 1 << 5; // CR4_PAE;
  		sregs->cr4 |= 0x600; // CR4_OSFXSR | CR4_OSXMMEXCPT;  --  enable SSE instructions 
  		sregs->cr0 = 0x80050033; // CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG
  		sregs->efer = 0x500; // EFER_LME | EFER_LMA

		  There's a special register that can enable syscall/sysenter instruction: 
		  EFER (Extended Feature Enable Register). We have used it for entering long mode before:
		sregs->efer = 0x500; // EFER_LME | EFER_LMALME and LMA stand for Long Mode Enable
		 and Long Mode Active, respectively.
	 */ 
	sregs->cr3 = pml4_addr;
	sregs->cr4 = CR4_PAE;
	sregs->cr0
		= CR0_PE | CR0_MP | CR0_ET | CR0_NE | CR0_WP | CR0_AM | CR0_PG;
	sregs->efer = EFER_LME | EFER_LMA;



	setup_64bit_code_segment(sregs);
}

int run_long_mode(struct vm *vm, struct vcpu *vcpu)
{

	/**
	 * 
	 * VCPU also includes the processor's register state,
	 *  broken into two sets of registers: standard registers and "special" registers. 
	 * These correspond to two architecture-specific data structures: struct kvm_regs and struct kvm_sregs,
	 *  respectively. On x86, the standard registers include general-purpose registers,
	 *  as well as the instruction pointer and flags; 
	 *  the "special" registers primarily include segment registers and control registers.
	 */
	struct kvm_sregs sregs;
	struct kvm_regs regs;

	printf("Testing 64-bit mode\n");

        if (ioctl(vcpu->fd, KVM_GET_SREGS, &sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

	setup_long_mode(vm, &sregs);

        if (ioctl(vcpu->fd, KVM_SET_SREGS, &sregs) < 0) {
		perror("KVM_SET_SREGS");
		exit(1);
	}

	/**
	 * For the standard registers, we set most of them to 0, 
	 * and the initial state of the flags (specified as 0x2 by the x86 architecture; 
	 * starting the VM will fail with this not set):
	 * memset function converts the value of a character to unsigned character and 
	 * copies it into each of first n character of the object pointed by the given str[].
	 */ 
	memset(&regs, 0, sizeof(regs));
	/** Clear all FLAGS bits, except bit 1 which is always set.
	 * in x86 the 0x2 bit should always be set 
	*/
	regs.rflags = 2;
	/**
	 * user entry
	 * */
	regs.rip = 0;
	/* Create stack at top of 2 MB page and grow down.   stack address 0x200000s */
	/**
	 * Q3.1 Kernal stack range
	 * */
	regs.rsp = 2 << 20;
	printf("\n Kernal Stack in guest physical address : %lld to %lld \n",regs.rsp,sregs.ss.base);
	printf("\n Kernal Stack in host virtual address : %p to %p \n",vm->mem + 0x20000,vm->mem );

	if (ioctl(vcpu->fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

	/**
	 * Q3.1 guest memory area is formatted to contain the guest code
	 * */
	printf("\n Guest code range in Host Virtual address %p to %p \n",vm->mem,vm->mem + 0x20000);
	memcpy(vm->mem, guest64, guest64_end-guest64);
/*
	int ptfd = ioctl(vm->fd,KVM_CAP_PPC_HTAB_FD,0);
	if(ptfd<0){
		perror("KVM_PT_ERROR");
		
	}
	char buf[10000];
	int rd =read(ptfd, buf,10000);
	printf(" Page table %d %s",rd,buf);
	*/
	
	//printf("\n Guest code in guest memory %p \n",guest64);
	return run_vm(vm, vcpu, 8);
}


int main(int argc, char **argv)
{

	struct vm vm;
	struct vcpu vcpu;
	
	enum {
		REAL_MODE,
		PROTECTED_MODE,
		PAGED_32BIT_MODE,
		LONG_MODE,
	} mode = REAL_MODE;
	int opt;

	while ((opt = getopt(argc, argv, "rspl")) != -1) {
		switch (opt) {
		case 'r':
			mode = REAL_MODE;
			break;

		case 's':
			mode = PROTECTED_MODE;
			break;

		case 'p':
			mode = PAGED_32BIT_MODE;
			break;

		case 'l':
			mode = LONG_MODE;
			break;

		default:
			fprintf(stderr, "Usage: %s [ -r | -s | -p | -l ]\n",
				argv[0]);
			return 1;
		}
	}



	vm_init(&vm, 0x200000);
	vcpu_init(&vm, &vcpu);

	switch (mode) {
	case REAL_MODE:
		return !run_real_mode(&vm, &vcpu);

	case PROTECTED_MODE:
		return !run_protected_mode(&vm, &vcpu);

	case PAGED_32BIT_MODE:
		return !run_paged_32bit_mode(&vm, &vcpu);

	case LONG_MODE:
		return !run_long_mode(&vm, &vcpu);
	}

	return 1;
}