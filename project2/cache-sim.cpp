#include "cache-sim.h"

using namespace std;

/*
for caches that need to use LRU policy, have a counter per entry that keeps track of time ticks (increment when not accessed, reset to 0 when accessed)
sort entries/vector/whatever in ascending order? or have aux function to grab LRU entry
*/
string direct(int size, vector<trace> traces){
  int hits = 0, misses = 0, accesses = 0;
  int entries = size/32;

  vector<Block> table(size, Block(0, 0));
  //cout << "Size of traces: " << traces.size() << " size of table: " << table.size() <<endl;
  for(trace t: traces){
    if(t.target == 0){
      cout << "0 address" << endl;
      continue;
    }
    unsigned index = (t.target >> 5) % entries;
    unsigned tag = t.target >> 5+ size;
    //cout << "Target "<< t.target << " Instr: " << t.instruction <<" Index: " <<index << " Tag: " << tag<< endl;

    Block & currentBlock = table[index];
    //cout<< " Current Tag: " << currentBlock.tag << " Valid: " << currentBlock.valid_bit<<endl;
    accesses++;
    if(currentBlock.valid_bit == 0 || currentBlock.tag != tag){
      //cout << "miss"<<endl;
      misses++;
      currentBlock.valid_bit = 1;
      currentBlock.tag = tag;
    }
    else{
      //cout << "hit" <<endl;
      hits++;
    }

  }
  return IntToString(hits)+","+IntToString(accesses)+";";
}

string set_associative(int set_size, vector<trace> traces, int write_miss = 0, int NL_prefetching = 0){
  int env_setting = 0;
  if(write_miss && !NL_prefetching) env_setting = 1;
  if(!write_miss && NL_prefetching) env_setting = 2;
  if(write_miss && NL_prefetching) env_setting = 3;
  //dimensions
  //set size = blocks per set; blocks = lines
  unsigned cache_size = pow(2, 14), block_size = 32; //total cache size is 16KB = 2^14, block size
  unsigned total_entries = cache_size / block_size; //cache lines
  unsigned num_sets = total_entries / set_size; //number of sets
  unsigned entries_per_set = total_entries / num_sets; //lines per set

  //bit break down
  unsigned size_offset = log2(block_size); // 5
  unsigned size_index = log2(num_sets);
  cout<< "cache size: " << cache_size << " num sets: " << num_sets << " entries per set: " << entries_per_set << " "<<endl;
  //for output
  int hits = 0, misses = 0, accesses = 0;
  shift_amnt_SA = size_index + size_offset;
  //cache will be represented by a vector of sets, each set contains n entries where n = set_size
  vector<Set> table(num_sets, Set(set_size));
  
  //cout << "Size of traces: " << traces.size() << " size of table: " << table.size() <<endl;
  for(trace t: traces){
    if(t.target == 0){
      cout << "0 address" << endl;
      continue;
    }
    accesses++;
    unsigned index = t.target >> size_offset;
    unsigned current_set_number = index % num_sets;
    unsigned tag = index >> size_index;
    Set & currentSet = table[current_set_number];

    if(inSet(currentSet, tag)){
      //cout << "hit!" <<endl;
      hits++;
      incrementLRUCounters(currentSet, tag);
      if(env_setting == 2){
        //prefetch next line always (env setting 2)

      }
    }
    else{
      //cout << "miss!" <<endl;
      misses++;
      //normal miss, fetch line
      if(env_setting == 0 || env_setting == 1){
        //if no alloc on write miss is set, we can't write on store instructions
        if(env_setting == 0 || t.instruction != "S") fetchLine(currentSet, t.target, tag);
      }
      //next line prefetching is enabled
      else {
        fetchLine(currentSet, t.target, tag);
        //@TODO fetch next line
      }
     
      incrementLRUCounters(currentSet, tag);
    }
    
    

  }
  shift_amnt_SA = 0;
  return IntToString(hits)+","+IntToString(accesses)+";";
}

int cache_sim(string inputfile, string outputfile){
  vector<trace> traces = parseFile(inputfile, outputfile);
  //Direct Mapped inititalization
  
  vector<string> output (0);
  cout << "Direct Mapped" << endl;
  /*** DIRECT MAPPED CACHE ***/
  int dm_sizes[4] = {10, 12, 14, 15}; //1KB = 2^10, 4KB = 2^ 12, 16KB = 2^14, 32 KB = 2^15
  string l1 = "";
  for(int i = 0; i < 4; i++){
    l1.append(direct((int)pow(2, dm_sizes[i]), traces));
    if(i != 3) l1.append(" ");
  } 
  output.push_back(l1);
  /**************************/
  cout << "Set Associative"<<endl;
  /** SET ASSOCIATIVE CACHE**/
  string l2 = "";
  int sa_ways[4] = {2, 4, 8, 16};
  for(int i = 0; i < 4; i++){
    l2.append(set_associative(sa_ways[i], traces, 0, 0));
    if(i != 3) l2.append(" ");
  }
  cout<< l2<<endl;
  output.push_back(l2);
  return writeResults(output, outputfile);
   
}

int main(int argc, char *argv[]){
  return cache_sim(argv[1], argv[2]);
}
