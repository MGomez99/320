#ifndef cache_sim_HEADER
#define cache_sim_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>

#define NUM_BITS 32
#define NUM_BLOCKS 32

/*
//example from class
cache with 2 entries, 4 blocks per entry

address: 38
binary: 100110

index = (address / blocks) % entries
= (38 / 4) % 2 
= 1
address binary: 100110

the first two bits in the binary is ignored because that's the offset to get to the 3rd block (00, 01, 10, 11)
so the index resides in the 3rd bit, which is 1.

the tag is the rest of the data that isn't the offset or index, which is binary: 100 in this case

tag = (address / blocks) / entries
= (38 / 4) / 2
= 9 / 2
= 4 binary: 100
*/
struct trace{
  unsigned long long target;
  std::string instruction;
  
};
struct Block{
  unsigned valid_bit;
  unsigned long long tag;
  unsigned dirty_bit;
};v
struct Cache{
  int hits, misses,writes, reads, cache_size, block_size, entries, line_size;
  std::vector<Block> blocks;
};
int log2(int n){
  return (int) (log(n)/log(2));
}
std::vector<trace> parseFile(std::string inputfile, std::string outputfile){
  std::vector<trace> traces;
  unsigned long long tg = 0;
  std::string instr;
  std::ifstream infile(inputfile);
  std::ofstream outfile(outputfile);
  if(!infile){
    std::vector<trace> empty;
    std::cout<< "Input file not found" << std::endl;
    return empty;
  }
  if(!outfile){
    std::vector<trace> empty;
    std::cout<< "Output File can't be created" << std::endl;
    return empty;
  }

  while(infile >> instr >> std::hex >> t5g){
    struct trace temp;
    temp.target = tg;
    temp.instruction = instr;
    traces.push_back(temp);    
  }
  return traces;
}

class DM_Cache{
  struct Cache* cache;
  //number of bits for offset, index, and tag
  int size_offset;
  int size_index;
  int size_tag;
  
  DM_Cache(int c_size, int l_size){
    cache->cache_size = c_size;
    cache->line_size = l_size;
    cache->hits = cache->misses = cache->writes = cache->reads = 0;
    size_offset = log2(l_size);
    size_index = log2(c_size);
    tag = NUM_BITS - size_offset - size_index;
    cache->blocks.resize(c_size);
    //@TODO initiallize blocks to #[c_size] empty structs
    cache->entries = cache_size / NUM_BLOCKS;
  }
  DM_Cache(){}
  void updateCache(struct trace t){
    int idx = (t->target / NUM_BLOCKS) % cache->entries;
    
  }

};
#endif
