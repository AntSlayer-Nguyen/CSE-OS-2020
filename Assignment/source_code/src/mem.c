
#include "mem.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <stdio.h>

static BYTE _ram[RAM_SIZE];

static struct {
	uint32_t proc;	// ID of process currently uses this page
	int index;	// Index of the page in the list of pages allocated
			// to the process.
	int next;	// The next page in the list. -1 if it is the last
			// page.
} _mem_stat [NUM_PAGES];

static pthread_mutex_t mem_lock;
static pthread_mutex_t ram_lock;

void init_mem(void) {
	memset(_mem_stat, 0, sizeof(*_mem_stat) * NUM_PAGES);
	memset(_ram, 0, sizeof(BYTE) * RAM_SIZE);
	pthread_mutex_init(&mem_lock, NULL);
	pthread_mutex_init(&ram_lock, NULL);
}

/* get offset of the virtual address */
static addr_t get_offset(addr_t addr) {
	return addr & ~((~0U) << OFFSET_LEN);
}

/* get the first layer index */
static addr_t get_first_lv(addr_t addr) {
	return addr >> (OFFSET_LEN + PAGE_LEN);
}

/* get the second layer index */
static addr_t get_second_lv(addr_t addr) {
	return (addr >> OFFSET_LEN) - (get_first_lv(addr) << PAGE_LEN);
}

/* Search for page table table from the a segment table */
static struct page_table_t * get_page_table(
		addr_t index, 	// Segment level index
		struct seg_table_t * seg_table) { // first level table
	
	/*
	 * TODO: Given the Segment index [index], you must go through each
	 * row of the segment table [seg_table] and check if the v_index
	 * field of the row is equal to the index
	 *
	 * */
	if (!seg_table) return NULL;
	int i;
	for (i = 0; i < seg_table->size; i++) {
		// Enter your code here
		if(index == seg_table->table[i].v_index) return seg_table->table[i].pages;
	}
	return NULL;

}

/* Translate virtual address to physical address. If [virtual_addr] is valid,
 * return 1 and write its physical counterpart to [physical_addr].
 * Otherwise, return 0 */
static int translate(
		addr_t virtual_addr, 	// Given virtual address
		addr_t * physical_addr, // Physical address to be returned
		struct pcb_t * proc) {  // Process uses given virtual address

	/* Offset of the virtual address */
	addr_t offset = get_offset(virtual_addr);
	/* The first layer index */
	addr_t first_lv = get_first_lv(virtual_addr);
	/* The second layer index */
	addr_t second_lv = get_second_lv(virtual_addr);
	
	/* Search in the first level */
	struct page_table_t * page_table = NULL;
	page_table = get_page_table(first_lv, proc->seg_table);
	if (page_table == NULL) {
		return 0;
	}

	int i;
	for (i = 0; i < page_table->size; i++) {
		if (page_table->table[i].v_index == second_lv) {
			/* TODO: Concatenate the offset of the virtual addess
			 * to [p_index] field of page_table->table[i] to 
			 * produce the correct physical address and save it to
			 * [*physical_addr]  */
			*physical_addr = (page_table->table[i].p_index * PAGE_SIZE) | offset;
			return 1;
		}
	}
	return 0;	
}

addr_t alloc_mem(uint32_t size, struct pcb_t * proc) {
	pthread_mutex_lock(&mem_lock);
	addr_t ret_mem = 0;
	/* TODO: Allocate [size] byte in the memory for the
	 * process [proc] and save the address of the first
	 * byte in the allocated memory region to [ret_mem].
	 * */
	
	uint32_t num_pages = (size % PAGE_SIZE) ? size / PAGE_SIZE + 1:
		size / PAGE_SIZE ; // Number of pages we will use
	int mem_avail = 0; // We could allocate new memory region or not?

	/* First we must check if the amount of free memory in
	 * virtual address space and physical address space is
	 * large enough to represent the amount of required 
	 * memory. If so, set 1 to [mem_avail].
	 * Hint: check [proc] bit in each page of _mem_stat
	 * to know whether this page has been used by a process.
	 * For virtual memory space, check bp (break pointer).
	 * */
	int num_pages_avail = 0;
	int i;
	for(i = 0; i < NUM_PAGES; i++){
		if(_mem_stat[i].proc == 0) {
			num_pages_avail++;
			if(num_pages_avail == num_pages && proc->bp + num_pages * PAGE_SIZE < RAM_SIZE){
			    mem_avail = 1;
			    break;
		    }
		}
	}

	if (mem_avail) {
		/* We could allocate new memory region to the process */
		ret_mem = proc->bp;
		proc->bp += num_pages * PAGE_SIZE;
		/* Update status of physical pages which will be allocated
		 * to [proc] in _mem_stat. Tasks to do:
		 * 	- Update [proc], [index], and [next] field
		 * 	- Add entries to segment table page tables of [proc]
		 * 	  to ensure accesses to allocated memory slot is
		 * 	  valid. */
		int num_pages_alloc = 0;
		addr_t curr_v_addr;
		int seg_idx,page_idx;
		int prev_idx;                       // variable to keep track the previous page
		for(i = 0; i < NUM_PAGES; i++){
			if(_mem_stat[i].proc == 0){     // page that can be used to allocate
				_mem_stat[i].proc = proc->pid;        /*------------------*/  
				_mem_stat[i].index = num_pages_alloc; /*------------------*/
				                                      /*------------------*/
				if(_mem_stat[i].index != 0){          /* update RAM status*/
					_mem_stat[prev_idx].next = i;     /*------------------*/
				}                                     /*------------------*/
				prev_idx = i;
				struct seg_table_t  *seg_table = proc->seg_table;
				if(!seg_table->table[0].pages) seg_table->size = 0;
				curr_v_addr = ret_mem + (num_pages_alloc << OFFSET_LEN); // virtual address of this page
				seg_idx = get_first_lv(curr_v_addr);  
				page_idx = get_second_lv(curr_v_addr);
				//int j;
				struct page_table_t  *curr_page_table = get_page_table(seg_idx,seg_table); /*search for the 
                                                                                            *page table */
				if(curr_page_table){  // if found, we fill new address into page table
					curr_page_table->table[curr_page_table->size].v_index = page_idx; 
					curr_page_table->table[curr_page_table->size].p_index = i;
					curr_page_table->size++;	
				}
				else{               // if not, we add new entry (page table)
					seg_table->table[seg_table->size].v_index = seg_idx;
					seg_table->table[seg_table->size].pages =(struct page_table_t*) malloc (sizeof(struct page_table_t));
					seg_table->table[seg_table->size].pages->table[0].v_index = page_idx;
					seg_table->table[seg_table->size].pages->table[0].p_index = i;
					seg_table->table[seg_table->size].pages->size = 1;
					seg_table->size++;
				}
				
				num_pages_alloc++;
				if(num_pages_alloc == num_pages) {  // last page in mem list
					_mem_stat[i].next = -1;
					break;
				}
			}
		}
	}
	if(MEM_STAT){
		puts("----------------ALLOCATION--------------------\n");
    	dump();
	}
	pthread_mutex_unlock(&mem_lock);
	return ret_mem;
}

int free_mem(addr_t address, struct pcb_t * proc) {
	/*TODO: Release memory region allocated by [proc]. The first byte of
	 * this region is indicated by [address]. Task to do:
	 * 	- Set flag [proc] of physical page use by the memory block
	 * 	  back to zero to indicate that it is free.
	 * 	- Remove unused entries in segment table and page tables of
	 * 	  the process [proc].
	 * 	- Remember to use lock to protect the memory from other
	 * 	  processes.  */
	
	pthread_mutex_lock(&mem_lock);
    /* - First, we find physical page in memory
	 * using translation function to map  
	 * - if successful, we begin to traverse in mem list */
	addr_t 	physic_address = 0;      // physical address to free 
	/* check if valid address (had been allocated) */
	int isValid = translate(address,&physic_address,proc);
	if(!isValid) return 1; // invalid v_address
	/*if valid, */
	int p_index = physic_address >> OFFSET_LEN;    // remove OFFSET part
	int num_free_pages = 0;
	for(int i=p_index;i!=-1;i=_mem_stat[i].next){  // traverse and update  
	    num_free_pages++;                          // mem_stat
		_mem_stat[i].proc = 0;
	}
    // clear virtual page stored in process
	for(int i=0;i<num_free_pages;i++){
		addr_t curr_v_address = address + PAGE_SIZE * i;
		int seg_index = get_first_lv(curr_v_address);
		int page_index = get_second_lv(curr_v_address);
		struct page_table_t  *page_table = get_page_table(seg_index,proc->seg_table);
		if(!page_table) {
			puts("-----------ERROR DEALLOCATION-----------\n");
			continue;
		}
		for(int j=0;j<page_table->size;j++){
			if(page_table->table[j].v_index == page_index){
				page_table->size--;
				for(int k = j; k<page_table->size;k++){
					page_table->table[k] = page_table->table[k+1]; // shift the page to 
				}                                                  // rearrange the page table
			                                                       // remove the remainder 
			}
		}
		if(page_table->size == 0){ // if empty after removing
			free(page_table);      // remove the whole page table 
			int m;                 // and also remove the segment row that points to it
			for(m = 0;m<proc->seg_table->size;m++){
				if(seg_index == proc->seg_table->table[m].v_index) break;
			}
			int n;
			for(n = m; n<proc->seg_table->size-1; n++){
				proc->seg_table->table[n] = proc->seg_table->table[n + 1];
			}
			
			proc->seg_table->size--;
		}
	}
	// Update break pointer
	addr_t seg_page = address >> OFFSET_LEN;
	if (seg_page + num_free_pages * PAGE_SIZE == proc->bp) {
		while (proc->bp >= PAGE_SIZE) {
		    addr_t last_addr = proc->bp - PAGE_SIZE;
		    addr_t last_segment = get_first_lv(last_addr);
		    addr_t last_page = get_second_lv(last_addr);
		    struct page_table_t * page_table = get_page_table(last_segment, proc->seg_table);
		    if (page_table == NULL) break;
		    while (last_page >= 0) {
			    int i;
			    for (i = 0; i < page_table->size; i++) {
				    if (page_table->table[i].v_index == last_page) {
					    proc->bp -= PAGE_SIZE;
					    last_page--;
					    break;
				    }
			    }
			    if (i == page_table->size) break;
		    }
		    if (last_page >= 0) break;
	    }
	}

    
	pthread_mutex_unlock(&mem_lock);
	if(MEM_STAT){
		puts("---------------DEALLOCATION-------------------\n");
		dump();
	}
	return 0;
}

int read_mem(addr_t address, struct pcb_t * proc, BYTE * data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		pthread_mutex_lock(&ram_lock);
		*data = _ram[physical_addr];
		pthread_mutex_unlock(&ram_lock);
		return 0;
	}else{
		return 1;
	}
}

int write_mem(addr_t address, struct pcb_t * proc, BYTE data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		pthread_mutex_lock(&ram_lock);
		_ram[physical_addr] = data;
		pthread_mutex_unlock(&ram_lock);
		return 0;
	}else{
		return 1;
	}
}

void dump(void) {
	int i;
	for (i = 0; i < NUM_PAGES; i++) {
		if (_mem_stat[i].proc != 0) {
			printf("%03d: ", i);
			printf("%05x-%05x - PID: %02d (idx %03d, nxt: %03d)\n",
				i << OFFSET_LEN,
				((i + 1) << OFFSET_LEN) - 1,
				_mem_stat[i].proc,
				_mem_stat[i].index,
				_mem_stat[i].next
			);
			int j;
			for (	j = i << OFFSET_LEN;
				j < ((i+1) << OFFSET_LEN) - 1;
				j++) {
				
				if (_ram[j] != 0) {
					printf("\t%05x: %02x\n", j, _ram[j]);
				}
					
			}
		}
	}
}


