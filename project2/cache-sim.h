#ifndef cache_sim_HEADER
#define cache_sim_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
struct trace{
  unsigned long long target;
  std::string instruction;
  
};

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

  while(infile >> instr >> tg){
    struct trace temp;
    temp.target = tg;
    temp.instruction = instr;
    traces.push_back(temp);    
  }
  return traces;
}

class DM_Cache{
  int cache_size = 0;
  int line_size = 0;
  
  DM_Cache(int c_size, int l_size){
    cache_size = c_size;
    line_size = l_size;
  }

};
#endif
