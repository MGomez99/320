#include "cache-sim.h"

using namespace std;

/*
for caches that need to use LRU policy, have a counter per entry that keeps track of time ticks (increment when not accessed, reset to 0 when accessed)
sort entries/vector/whatever in ascending order? or have aux function to grab LRU entry
*/
int cache_sim(string inputfile, string outputfile){
  vector<trace> traces = parseFile(inputfile, outputfile);
  cout << "traces parsed from file" <<endl;
  //Direct Mapped inititalization
  int dm_sizes[4] = {10, 12, 14, 15}; //1KB = 2^10, 4KB = 2^ 12, 16KB = 2^14, 32 KB = 2^15
  vector<DM_Cache> DMs;
  DMs.resize(4);
  for(int i = 0; i < 4; i++){
    DMs[i] = DM_Cache(pow(2, dm_sizes[i]));
  }

  //test all caches
  cout << "testing caches" <<endl;
  cout << "line 1 things:  " << traces[0].instruction << traces[0].target << endl;
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < traces.size(); j++){
      DMs[i].updateCache(traces[j]);
    }
  }
  
  cout << "done" <<endl;

  //print results
  for(int i = 0; i < 4; i++){
    cout << DMs[i].cache->hits << ","<<DMs[i].cache->accesses <<";";
    cout << endl;
  }
  return 0;
   
}

int main(int argc, char *argv[]){
    return cache_sim(argv[1], argv[2]);
}
