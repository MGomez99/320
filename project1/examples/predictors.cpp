#include "predictors.h"
//#define FILEPATH "/traces/short_trace1.txt"
#define FILEPATH "test_input.txt"
using namespace std;

void alwaysTaken(unsigned long long address, string behavior, unsigned long long target, int &correct){
    //cout << "ALWAYS TAKEN:\t";
    //cout << address << "\t Prediction: T" << "\tActual: ";
    if(behavior == "T"){
        //cout << "T"<<endl;
        correct++;
    }
    else if (behavior == "NT"){
        //cout << "NT" <<endl;
    }
    else{
        cout << "err with parse"<<endl;
    }
}

void alwaysNonTaken(unsigned long long address, string behavior, unsigned long long target, int &correct){
    //cout << "ALWAYS NON TAKEN:\t";
    //cout << address << "\t Prediction: NT" << "\tActual: ";
    if(behavior == "NT"){
        //cout << "NT"<<endl;
        correct++;
    }
    else if (behavior == "T"){
        //cout << "T" <<endl;
    }
    else{
        cout << "err with parse"<<endl;
    }
}

void predictor(){

    unsigned long long address = 0;
    unsigned long long target = 0;
    string behavior;
    string line;

    int atpredictions = 0;
    int antpredictions = 0;

    ifstream infile(FILEPATH);
    while(getline(infile, line)){
        stringstream s(line);
        s >> std::hex >> address >> behavior >> std::hex >> target;
        alwaysTaken(address, behavior, target, atpredictions);
        alwaysNonTaken(address, behavior, target, antpredictions);
    }
    cout << "AT Correct: "<<atpredictions<<"\tNT Correct: "<< antpredictions<<endl;

}

int main(int argc, char *argv[]){
    predictor();
    return 0;
}
