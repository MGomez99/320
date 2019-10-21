#include "predictors.h"
//#define FILEPATH "/traces/short_trace1.txt"
#define INPUTFILEPATH "short_trace1.txt"
#define OUTPUTFILEPATH "test_output.txt"
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

void predictor(){
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
    for(int i = 0; i < 9; i++){
        GShares[i] = GShare(3+i);
    }
    //3 Bits to 11 Bits
    /*********************************************************************/
    //TOURNAMENT
    Tournament tournamentPredictor = Tournament();
    /*********************************************************************/
    //Predictions
    ifstream infile(INPUTFILEPATH);
    int branches = 0;
    while(getline(infile, line)){
        ++branches;
        stringstream s(line);
        s >> std::hex >> address >> behavior >> std::hex >> target;
        alwaysTaken(address, behavior, target, atpredictions);
        alwaysNonTaken(address, behavior, target, antpredictions);

        for(int i = 0; i < 7; i++){
            singleBitBimodal[i].checkTable(address, behavior);
            doubleBitBimodal[i].checkTable(address,behavior);
        }
        
        for(int i = 0; i < 9; i++){
            if(GShares[i].checkTable(address, behavior) == -1) break;
        }
        
        if(tournamentPredictor.checkTable(address, behavior) == -1) break;
    }
    /*********************************************************************/
    //OUTPUT
    ofstream f(OUTPUTFILEPATH);
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
    }
    f.close();
}

int main(int argc, char *argv[]){
    predictor();
    return 0;
}
