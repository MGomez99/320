#include "cache-sim.h"

using namespace std;

/*
for caches that need to use LRU policy, have a counter per entry that keeps track of time ticks (increment when not accessed, reset to 0 when accessed)
sort entries/vector/whatever in ascending order? or have aux function to grab LRU entry
*/
int cache_sim(string inputfile, string outputfile){
  vector<trace> traces = parseFile(inputfile, outputfile);

  return 0;
   
}

int main(int argc, char *argv[]){
    return cache_sim(argv[1], argv[2]);
}
