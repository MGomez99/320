#include "predictors.h"
#define INPUTFILEPATH "traces/short_trace1.txt"
#define OUTPUTFILEPATH "output.txt"
using namespace std;

void alwaysTaken(unsigned long long address, string behavior, unsigned long long target, int &correct){

    if(behavior == "T"){
        correct++;
    }
    else if (behavior == "NT"){}
    else{cout << "err with parse"<<endl;}
}

void alwaysNonTaken(unsigned long long address, string behavior, unsigned long long target, int &correct){

    if(behavior == "NT"){
        correct++;
    }
    else if (behavior == "T"){}
    else{cout << "err with parse"<<endl;}
}

int predictor(string inputfile, string outputfile){
    /*********************************************************************/
    //Parsing Variables
    unsigned long long address = 0;
    unsigned long long target = 0;
    string behavior;
    string line;
    /*********************************************************************/
    //ALWAYS TAKEN & ALWAYS NON TAKEN
    int atpredictions = 0;
    int antpredictions = 0;
    /*********************************************************************/
    //BIMODAL CLASSES
    vector<Bimodal> singleBitBimodal;
    vector<Bimodal> doubleBitBimodal;
    singleBitBimodal.resize(8);
    doubleBitBimodal.resize(8);
    //Sizes 16 32 64 128 256 512 1024 2048
    int sizes[7] = {16, 32, 128, 256, 512, 1024, 2048};
    for(int i = 0; i < 7; i++){
        singleBitBimodal[i] = Bimodal(sizes[i], 1);
        doubleBitBimodal[i] = Bimodal(sizes[i], 2);
    }
    /*********************************************************************/
    //GSHARE CLASSES
    vector<GShare> GShares;
    GShares.resize(9);
    //3 Bits to 11 Bits
    int sizes2[9] = {3, 4, 5, 6, 7, 8, 9, 10, 11};
    for(int i = 0; i < 9; i++){
        GShares[i] = GShare(sizes2[i]);
    }
    
    /*********************************************************************/
    //TOURNAMENT
    Tournament tournamentPredictor = Tournament();
    /*********************************************************************/
    //BTB
    BTB branchTargetBuffer = BTB();
    /*********************************************************************/
    //Predictions
    ifstream infile(inputfile);
    ofstream f(outputfile);
    if(!infile){
        cout<< "Input file not found" << endl;
        return -1;
    }
    if(!f){
        cout<< "Output File can't be created" << endl;
        return -1;
    }
    int branches = 0;

    while(getline(infile, line)){
        //if(branches > 5) break;
        ++branches;
        stringstream s(line);
        s >> std::hex >> address >> behavior >> std::hex >> target;
        alwaysTaken(address, behavior, target, atpredictions);
        alwaysNonTaken(address, behavior, target, antpredictions);

        for(int i = 0; i < 7; i++){
            if(singleBitBimodal[i].checkTable(address, behavior) == -1) break;
            if(doubleBitBimodal[i].checkTable(address, behavior) == -1) break;
        }
        
        for(int i = 0; i < 9; i++){
            if(GShares[i].checkTable(address, behavior) == -1) break;
        }
        
        if(tournamentPredictor.checkTable(address, behavior) == -1) break;
        if(branchTargetBuffer.checkTable(address, behavior, target) == -1) break;
    }
    /*********************************************************************/
    //OUTPUT
    
    if(f){
        //Always Taken / Always Non Taken
        f << atpredictions <<","<< branches << ";"<< endl;
        f << antpredictions <<","<< branches << ";"<< endl;
        //Single Bit Bimodal
        for(int i = 0; i < 6; i++){
            f << singleBitBimodal[i].correct << ","<< branches << "; ";
        }
        f << singleBitBimodal[6].correct << ","<< branches << ";"<<endl;
        //Two Bit Saturating Bimodal
        for(int i = 0; i < 6; i++){
            f <<doubleBitBimodal[i].correct << ","<< branches << "; ";
        }
        f << doubleBitBimodal[6].correct << ","<< branches << ";"<<endl;
        //Gshare
        for(int i = 0; i < 8; i++){
            f <<GShares[i].correct << ","<< branches << "; ";
        } 
        f << GShares[8].correct << ","<< branches << ";"<<endl;
        //Tournament
        f << tournamentPredictor.correct << ","<< branches << ";"<<endl;
        f << branchTargetBuffer.accesses<<","<<branchTargetBuffer.hits<<";"<<endl;
    }
    f.close();
    return 0;
}

int main(int argc, char *argv[]){
    return predictor(argv[1], argv[2]);
}
