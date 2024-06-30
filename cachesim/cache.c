#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;

void cycle_increase(int n) { cycle_cnt += n; }

struct Cache_Slot{
	bool dirty;
	bool valid;
	uintptr_t tag;
	uint8_t data[BLOCK_SIZE];
};

#define Block_addr_mask (mask_with_len(BLOCK_WIDTH))
#define group_addr_mask (mask_with_len(cache_group_width)<<BLOCK_WIDTH)
#define tag_mask (~mask_with_len(BLOCK_WIDTH + cache_group_width))
#define block_number_mask (~mask_with_len(BLOCK_WIDTH))

#define extract_group_number(addr) ((addr & group_addr_mask) >> BLOCK_WIDTH)
#define extract_tag(addr) ((addr & tag_mask) >> (BLOCK_WIDTH + cache_group_width))
#define extract_block_number(addr) ((addr & block_number_mask) >> BLOCK_WIDTH)
#define align(addr) ((addr & Block_addr_mask) & (~0x3))
#define extract_cache_group_number(addr,offset) ((extract_group_number(addr) << cache_asso_width) + offset)

static struct Cache_Slot *cache; // Cache, which contains many cache_slot.
static uint32_t cache_size_width,cache_asso_width,cache_group_width;

static inline uint32_t random_choice(uint32_t n) { return rand() % n; }

uint32_t cache_read(uintptr_t addr) {
  
  uintptr_t group_head = extract_cache_group_number(addr,0);
  uintptr_t target_tag = extract_tag(addr);
  //printf("In cache_read, addr is : 0x%lx, tag is : 0x%lx\n",addr,target_tag);
  uint32_t group_width = exp2(cache_asso_width);
  //printf("group head is : %ld\n",group_head);
  
  for(int i = group_head; i < group_head + group_width; i++){
  	//printf("test\n");
  	if(cache[i].valid && cache[i].tag == target_tag){
  		return * ((uint32_t*) &cache[i].data[align(addr)]);
  	}
  }
  
  uint32_t index = random_choice(group_width);
  //printf("ramdom choice is : %d\n",index);
  struct Cache_Slot *replaced_slot = & cache[group_head + index];
  if(replaced_slot->dirty){
    uintptr_t block_num = ((replaced_slot->tag << cache_group_width) | extract_group_number(addr)) ;
  	mem_write(block_num, replaced_slot->data);
  	replaced_slot->dirty = false; // To write back to memory.
  }
  
  mem_read(extract_block_number(addr), replaced_slot->data);
  replaced_slot->valid = true;
  replaced_slot->dirty = false;
  replaced_slot->tag = extract_tag(addr);
  
  return * ((uint32_t*) &replaced_slot->data[align(addr)]);
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uintptr_t group_head = extract_cache_group_number(addr,0);
  uintptr_t target_tag = extract_tag(addr);
  uint32_t group_width = exp2(cache_asso_width);
  for(int i = group_head; i < group_head + group_width; i++){
  	if(cache[i].valid && cache[i].tag == target_tag){
  	    uint32_t *data_target = (uint32_t *)(&cache[i].data[align(addr)]); //target data block
  		*data_target = (*data_target & ~wmask) | (data & wmask);
  		cache[i].dirty = true;
  		return ;
  	}
  }
  uint32_t index = random_choice(group_width);
  struct Cache_Slot *replaced_slot = & cache[group_head + index];
  if (replaced_slot->valid){
    if (replaced_slot->dirty){
		uintptr_t block_num = ((replaced_slot->tag << cache_group_width) | extract_group_number(addr)) ;
	  	mem_write(block_num, replaced_slot->data);
	  	replaced_slot->dirty = false; // To write back to memory.
    }
      mem_read(extract_block_number(addr), replaced_slot->data);
	  replaced_slot->valid = true;
	  replaced_slot->dirty = false;
	  replaced_slot->tag = extract_tag(addr);
  }else {
      mem_read(extract_block_number(addr), replaced_slot->data);
	  replaced_slot->valid = true;
	  replaced_slot->dirty = false;
	  replaced_slot->tag = extract_tag(addr);
  }
  uint32_t *data_target = (uint32_t *)(&replaced_slot->data[align(addr)]);
  *data_target = (*data_target & ~wmask) | (data & wmask);
  replaced_slot->dirty = true;
  return ;
}

void init_cache(int total_size_width, int associativity_width) {
	cache_size_width = total_size_width;
	cache_asso_width = associativity_width;
	cache_group_width = cache_size_width - BLOCK_WIDTH - cache_asso_width; 
	
	//printf("size of slot is: %ld \n",sizeof(struct Cache_Slot));
	cache = calloc(exp2(total_size_width)/BLOCK_SIZE,sizeof(struct Cache_Slot));
	//printf("the number of cache is :%d \n",exp2(total_size_width)/BLOCK_SIZE);
	assert(cache);
}

void display_statistic(void) {
}
