#ifndef cache_sim_HEADER
#define cache_sim_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>

#define BLOCKSIZE 32
using namespace std;

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
/***************************************/
/*               structs               */
/***************************************/
struct trace{
  
  unsigned long long target;
  string instruction;
  trace(unsigned long long tg, string instr): target(tg), instruction(instr){}
  trace(){target = 0; instruction = "";}
};
struct Block{
  unsigned valid_bit = 0;
  unsigned long long tag = 0;
  Block(unsigned valid, unsigned long long tg): valid_bit(valid), tag(tg) {}
  Block(){valid_bit = 0; tag = 0;}
};

class Cache{
  public:
  int hits, misses, accesses, cache_size, entries;
  vector<Block> table;
  Cache(){
    hits = 0, misses = 0, accesses  = 0, cache_size = 0, entries = 0;
    table.resize(0);
  }
 
};
/***************************************/
/***************************************/

/***************************************/
/*                misc.                */
/***************************************/
//int log2(int n){
  //return (int) (log(n)/log(2));
//}
/***************************************/
/***************************************/
vector<trace> parseFile(string inputfile, string outputfile){
  vector<trace> traces;
  unsigned long long tg = 0;
  string instr;
  ifstream infile(inputfile);

  if(!infile){
    vector<trace> empty;
    cout<< "Input file not found" << endl;
    return empty;
  }

  string line;
  while(getline(infile, line)){
    stringstream s(line);
    s >> instr >> hex >> tg;
    struct trace temp;
    temp.target = tg;
    temp.instruction = instr;
    traces.push_back(temp);    
  }
  infile.close();
  return traces;
}

class DM_Cache{
  public:
  Cache* cache;
  //number of bits for offset, index, and tag
  int size_offset = 0;
  int size_index = 0;
  int size_tag = 0;
  
  DM_Cache(int c_size){
    cache = new Cache();
    cache->cache_size = c_size;
    size_offset = 5; //log2(32)
    size_index = log2(c_size);
    
    size_tag = 32 - 5 - size_index;
    cache->table.resize(c_size);
    cache->entries = cache->cache_size / 32;
  }
  DM_Cache(){
    cache = new Cache();
  }
  ~DM_Cache(){
    delete cache;
  }
  void updateCache(struct trace t){
    if(t.target == 0){
      cout << "0 address" << endl;
      return;
    }
    //calculate tag and index
    int idx = (t.target >> 5) % cache->entries; // index = address/block size % entries
    int tag_t = t.target >> size_offset + size_index; //tag = address >> offsetbits
    cout << "Target "<< t.target << " Instr: " << t.instruction <<" Index: " <<idx << " Tag: " << tag_t<< endl;
    //index into table
    Block & current = cache->table[idx];
    cout<< " Current Tag: " << current.tag << " Valid: " << current.valid_bit<<endl;
    cache->accesses++;
    //line/block not valid
    if(current.valid_bit == 0){
      cout << " bit isnt valid" << endl;
      cache->misses ++;
      current.valid_bit = 1;
      current.tag = tag_t;
    }//block is valid, but tag doesn't match
    else if (current.tag != tag_t){
      cout << " tag doesnt match" << endl;
      cache->misses++;
      current.tag = tag_t;
    }//tag matches
    else{
      cout << " hit!" << endl;
      cache->hits++;
    }
  }

};
#endif
