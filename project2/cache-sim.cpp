#include "cache-sim.h"

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
      std::cout << "0 address" << endl;
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

string set_associative(int set_size, vector<trace> traces, int env_setting = 0, int Fully_associative = 0){
  //env_setting = 0; //default
  //env_setting = 1; //no alloc on WM
  //env_setting = 2; //next line prefetching
  //env_setting = 3; //NL prefetching only on miss
  //Fully_associative = 1 // fully associative cache w/ LRU eviction policy
  //Fully_associative = 2 // fully associative cache w/ pseudo LRU eviction policy
  
  /****** Dimensions & Variable ******/
  /****** HOT/COLD LRU APPROXIM ******/
  //set size = blocks per set; a block is also a line
  unsigned cache_size = pow(2, 14), block_size = 32; //total cache size is 16KB = 2^14, block size
  unsigned total_entries = cache_size / block_size; //cache lines
  unsigned num_sets;
  unsigned entries_per_set;

  if(Fully_associative == 1 || Fully_associative == 2){
    if(Fully_associative == 1){
      //Fully associative cache, so set size = # cache entries
      return set_associative(total_entries, traces, env_setting, 0); //2
    }
    else{
      num_sets = 1;
      entries_per_set = total_entries;
      return fully_associativeHC(total_entries, traces);
    }
  }
  else{
    num_sets = total_entries / set_size; //number of sets
    entries_per_set = total_entries / num_sets; //lines per set
  }


  /*bit break down*/
  unsigned size_offset = log2(block_size); // 5
  unsigned size_index = log2(num_sets);
  //cout<< "cache size: " << cache_size << " num sets: " << num_sets << " entries per set: " << entries_per_set << " "<<endl;
  //for output
  int hits = 0, misses = 0, accesses = 0;
  //cache will be represented by a vector of sets, each set contains n entries where n = set_size
  vector<Set> table(num_sets, Set(set_size));
  
  //if(Fully_associative != 0) std::cout << "Size of traces: " << traces.size() << " size of table: " << table.size() <<endl;

  for(trace t: traces){
    if(t.target == 0){
      std::cout << "0 address" << endl;
      continue;
    }
    accesses++;
    unsigned index = t.target >> size_offset;
    unsigned current_set_number = index % num_sets;
    unsigned tag = index >> size_index;

    unsigned next_index = index+1;
    unsigned next_set_number = next_index % num_sets;
    unsigned next_tag = next_index >> size_index;

    Set & currentSet = table[current_set_number];
    /* LEAST RECENTLY USED POLICY*/
    
    /*  HIT */
    if(inSet(currentSet, tag)){
      //cout << "hit!" <<endl;
      hits++;
        incrementLRUCounters(currentSet, tag);

      if(env_setting == 2){
        //prefetch next line always (env setting 2)
        Set & next_set = table[next_set_number];
        //bring in the next line if it's not in the respective set
        if(!inSet(next_set, next_tag)){
          fetchLine(next_set, 0, next_tag);
        }
        incrementLRUCounters(next_set, next_tag);
      }

    }
    /* MISS */
    else{
      misses++;
      //normal miss, fetch line
      if(env_setting == 0 || env_setting == 1){
        //if no alloc on write miss is set, we can't write on store instructions
        if(env_setting == 0 || t.instruction != "S") fetchLine(currentSet, t.target, tag);
        incrementLRUCounters(currentSet, tag);
      }
      //next line prefetching if env_setting is 2 or 3
      else {
        Set & next_set = table[next_set_number];
        fetchLine(currentSet, t.target, tag);
        incrementLRUCounters(currentSet, tag);
        if(!inSet(next_set, next_tag)){
          fetchLine(next_set, 0, next_tag);
        }
        incrementLRUCounters(next_set, next_tag);
      }
    
      
    }

  }
  
  return IntToString(hits)+","+IntToString(accesses)+";";
}
string fully_associativeHC(int set_size, vector<trace> traces){
  /*
  This Pseudo LRU Eviction policy was too much to deal with using my previous methods (and I didn't know how to incorporate the tree structure)
  so, I implemented the cache using one vect and the H/C bit table using another vect
  */
  unsigned cache_size = pow(2, 14), block_size = 32;
  unsigned total_entries = set_size;
  unsigned size_offset = log2(block_size); // 5
  unsigned size_index = 0;
  unsigned num_sets = 1;

  int accesses = 0;
  int hits = 0;
 
  vector<unsigned long long> cache(total_entries, 0);
  /****** HOT/COLD LRU APPROXIM ******/ 

  vector <int> HCTable(total_entries-1, 0);
  /*
    NOTE
    HCTable[i] == 0 => left hot, right cold
    HCTable[i] == 1 => left cold, right hot
  */
  for(trace t : traces){
    //Technically, index has 0 bits so we don't actually have an index in a fully associatve cache.. This is just calculations following the S-A cache implementation
    unsigned index = t.target >> size_offset;
    unsigned current_set_number = index % num_sets;
    unsigned tag = index >> size_index;

    //will keep track of the target index on hits 
    int ind_in_arr = 0;
    bool hit = false;
    accesses++;
    //check if it's a hit or not
    for(int i = 0; i < cache.size(); i++){
      if(cache[i] == tag){
        hit = true;
        hits++;
        ind_in_arr = i; //keep track of index in cache
        break;
      }
    }
    //divide the array into two halves
    int lower_index = 0;
    int upper_index = total_entries -1;
    int current_index = ROUND_AVG(upper_index, lower_index); 
    /* MISS CASE */
    if(hit == false){
      //traverse the HC table, updating the bits
      //current_index will eventually hold where to store new line into cache
      while(current_index %2){
        //current entry is 0
        if(HCTable[current_index] == 0){
          lower_index = current_index; //upper bound stays the same, shift lower bound
          //switch bit from 0 -> 1
          HCTable[current_index] = 1;
        }
        //current entry is 1
        else{
          upper_index = current_index;  //lower bound stays the same, shift upper bound
          HCTable[current_index] = 0; // switch bit from 1 -> 0
        }
        //split the new half into sub-halves
        current_index = ROUND_AVG(upper_index, lower_index);
      }
      //we reached the case where we are down to pairs
      // need to evict the correct line and bring in the current line
      if(HCTable[current_index] == 0){HCTable[current_index] = 1; cache[current_index + 1] = tag; }
      else{ HCTable[current_index] = 0; cache[current_index] = tag; }
    }
    /* HIT CASE */
    else{
      // UPDATE BITS ALONG THE PATH FIRST
      while(ROUND_POS(ind_in_arr) != current_index){
        //currently to the right of the target index
        if(ROUND_POS(ind_in_arr) > current_index){lower_index = current_index; HCTable[current_index] = 1;}//set the left side to be cold
        //currently to the left of the target index
        else{upper_index = current_index; HCTable[current_index] = 0;}// set the right side to be cold
        current_index = ROUND_AVG(upper_index, lower_index); // new middle index keeps changing untilwe're at the target
      }
      HCTable[current_index] = ind_in_arr - current_index; //to properly set val, offset by size of HC ARR
      cache[ind_in_arr] = tag;
    }
   
  }
  return IntToString(hits)+","+IntToString(accesses)+";";
}

int cache_sim(string inputfile, string outputfile){
  vector<trace> traces = parseFile(inputfile, outputfile);
  //Direct Mapped inititalization
  
  vector<string> output (0);
  std::cout << "Direct Mapped" << endl;
  /*** DIRECT MAPPED CACHE ***/
  int dm_sizes[4] = {10, 12, 14, 15}; //1KB = 2^10, 4KB = 2^ 12, 16KB = 2^14, 32 KB = 2^15
  string l1 = "";
  for(int i = 0; i < 4; i++){
    l1.append(direct((int)pow(2, dm_sizes[i]), traces));
    if(i != 3) l1.append(" ");
  } 
  output.push_back(l1);
  /**************************/
  std::cout << "Set Associative (All Cases)"<<endl;
  /** SET ASSOCIATIVE CACHE **/
  string l2 = "";
  string l5 = "";
  string l6 = "";
  string l7 = "";
  
  int sa_ways[4] = {2, 4, 8, 16};
  for(int i = 0; i < 4; i++){
    l2.append(set_associative(sa_ways[i], traces, 0)); //Standard Set Associative
    l5.append(set_associative(sa_ways[i], traces, 1)); //SA w/ No Alloc on WM
    l6.append(set_associative(sa_ways[i], traces, 2)); //SA w/ Next Line  prefetching
    l7.append(set_associative(sa_ways[i], traces, 3)); //SA w/ NLPF on WM
    if(i != 3){
      l2.append(" ");
      l5.append(" ");
      l6.append(" ");
      l7.append(" ");
    } 
  }
  output.push_back(l2);
  /**************************/

  /** FULLY ASSOCIATIVE CACHE **/  
  std::cout << "Fully Associative LRU"<<endl;
  string l3 = "";
  l3.append(set_associative(0, traces, 0, 1));

  std::cout << "Fully Associative Pseudo LRU"<<endl;
  string l4 = "";
  l4.append(set_associative(0, traces, 0, 2));
  output.push_back(l3);
  output.push_back(l4);
  /**************************/
  output.push_back(l5);
  output.push_back(l6);
  output.push_back(l7);
  cout << "DONE!" << endl;
  return writeResults(output, outputfile);
   
}

int main(int argc, char *argv[]){
  return cache_sim(argv[1], argv[2]);
}
