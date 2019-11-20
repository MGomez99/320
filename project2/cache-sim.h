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

using namespace std;

/***************************************/
/*               structs               */
/***************************************/
struct trace{
  
  unsigned long long target;
  std::string instruction;
  trace(unsigned long long tg, std::string instr): target(tg), instruction(instr){}
  trace(){target = 0; instruction = "";}
};
struct Block{
  unsigned valid_bit = 0;
  unsigned long long tag = 0;
  unsigned count = 0;
 unsigned long long target = 0;
  Block(unsigned valid, unsigned long long tg, unsigned ct = 0, unsigned long long trg = 0): valid_bit(valid), tag(tg), count(ct), target(trg){}
  Block(){valid_bit = 0, tag = 0, count = 0, target = 0;} //removed 0s
};

struct Set{
  unsigned size = 0;
  vector<Block> * ptr;
  vector <Block> set;
  Set(unsigned s){
    size = s;
    ptr = new vector<Block>(s, Block(0, 0, 0));
    set = *ptr;
    
  }
  Set(){
    size = 0; ptr = 0; set.resize(0);
  }
  ~Set(){ delete &ptr;}

};
/***************************************/
/***************************************/

/***************************************/
/*                misc.                */
/***************************************/

string IntToString(int a)
{
  ostringstream temp;
  temp << a;
  return temp.str();
}

bool inSet(Set s, unsigned long long tag){
  if(shift_amnt_SA == 0) cout <<"ERR"<<endl;
  for(Block b : s.set){
    if(b.tag == tag) return true;
  }
  return false;
}

unsigned indexToEvict(Set s, unsigned long long tag){
  int flag = 0;
  unsigned index_LRU = -1;
  int highest_count = -1;
  for(int i = 0; i < s.set.size(); i++){
    if(s.set[i].valid_bit == 0) return i;
    //find first possible eviction candidate
    if(!flag){
      if(s.set[i].tag != tag){
        flag = 1;
        index_LRU = i;
        highest_count = s.set[index_LRU].count;
      }
    }
    //find a more qualified candidate (can't be the index containing the current instruction's target's tag [useful for prefetching])
    else if(s.set[i].count > highest_count && s.set[i].tag != tag){
      index_LRU = i;
      highest_count = s.set[index_LRU].count;
    }
  }
  return index_LRU;
}
void incrementLRUCounters(Set & s, unsigned long long tag, unsigned long long prefetch = 0){
  if(prefetch == 0){
    for(int i = 0; i < s.set.size(); i++){
      
      if(s.set[i].tag == tag){

        s.set[i].count = 0;
      }
      else{
        s.set[i].count++;
      }
    }
  }
}

void fetchLine(Set & s, unsigned long long target, unsigned long long tag, unsigned long long prefetch = 0){
  //fetches line on a miss / prefetches too; sets counters to 0 for LRU
  if(prefetch == 0){
    int index = indexToEvict(s, tag);
    s.set[index].count = 0;
    s.set[index].tag = tag;
    s.set[index].target = target;
    s.set[index].valid_bit = 1;
  }//@TODO prefetching
}
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

int writeResults(vector<string> output, string outputfile){
  ofstream outfile(outputfile);
  if(!outfile){
    cout<< "Output File can't be created" << endl;
    return -1;
  }
  for(string l : output) outfile << l << endl;
  outfile.close();
  return 0;
}

#endif
