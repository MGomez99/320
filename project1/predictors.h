#ifndef PREDICTORS_HEADER
#define PREDICTORS_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
void alwaysTaken(unsigned long long , std::string , unsigned long long , int &);
void alwaysNonTaken(unsigned long long , std::string , unsigned long long , int &);
int predictor();

class Bimodal{
public:    
    std::vector<int> table;
    unsigned long long tablesize;
    int bits;
    int correct;

    Bimodal(){
        this->table.resize(0);
        this->bits = 0;
        this->tablesize = 0;
        correct = 0;
    }
    Bimodal(unsigned size, unsigned num_bits){
    /*
    0 = NT / Strongly NT
    1 = __ / Weakly NT
    2 = __ / Weakly T
    3 = T  / Strongly T

    */
        this->bits = num_bits;
        this->tablesize = size;
        this->correct = 0;
        if(num_bits == 1){
            this->table =  std::vector<int>(tablesize, 1);
        }else{
            this->table =  std::vector<int>(tablesize, 3);
        }
    }
    int checkTable(unsigned long long index, std::string behavior){
        //check and modify table
        /* Returns the Prediction based on the following:
        *  -1 = err
        *  1 = Taken
        *  0 = Non Taken
        */
        unsigned int entry_number = index % this->tablesize;
        if(behavior != "T" && behavior != "NT"){
            std::cout<< "incorrect behavior"<<std::endl;
            return -1;
        }
        int branchTaken = 0;
        if(behavior == "T"){
            branchTaken = 1;
        }
        // 0 <= entry <= tablesize
        if(bits == 1){
            int & prediction = table[entry_number];
            int retval;
            if(branchTaken == prediction){
                this->correct++;
                return prediction;
            }
            //incorrectly predict taken
            if(prediction == 1){
                retval = 1;
                prediction--;
            }//incorrectly predict nontaken
            else{
                retval = 0;
                prediction++;
            }
            return retval;

        }
        else if(bits == 2){
            
            int & prediction = table[entry_number];
            if(prediction < 0) std::cout<<"ERR, NEGATIVE PREDICTION"<<std::endl;
            
            int retval = -1;
           //predict non taken
            if(prediction < 2){
                retval = 0;
                //correctly predicts non taken
                if(branchTaken == 0){
                    this->correct++;
                    //move towards NN
                    if(prediction > 0){
                        prediction --;
                    }
                }//incorrect, behavior is taken
                else{
                    //move towards TT
                    prediction++;
                }
            }
            //predict taken
            else{
                retval = 1;
                //correct taken prediction
                if(branchTaken == 1){
                    //move towards TT
                    this->correct++;
                    if(prediction < 3){
                        prediction++;
                    }
                }
                //incorrect, behavior is non taken
                else{
                    //move towards NN
                    prediction --;
                }
            }
            return retval;
        }
        else{
            std::cout <<  "err with bit num" <<std::endl;
        }
        return -1;
    }
    

};
class GShare{
public:    
    Bimodal table;
    int history_bits;
    int correct;
    std::string history_register; //store register as string for length enforcement
    GShare(){
        this->history_bits = 0;
        correct = 0;
    }
    GShare(unsigned history_bits){
        history_register = "";
        for(int i = 0; i < history_bits; i++) {history_register.append("0");}
        this->history_bits = history_bits;
        int tablesize;
        correct = 0;
        this->table =  Bimodal(2048, 2);
    }
    int checkTable(unsigned long long address, std::string behavior){
        //check and modify table
        /* Returns the Prediction based on the following:
        *  -1 = err
        *  1 = Taken
        *  0 = Non Taken
        */
        unsigned long long index = address % 2048;
        index = (address ^ stoi(history_register, nullptr, 2)); //xor address with register to get index
        int prediction = this->table.checkTable(index, behavior);
        // if branch taken, shift left and store a 1, else 0
        history_register = history_register.substr(1, this->history_bits-1);
        if(behavior == "T"){
            history_register.append("1");
        }else{
            history_register.append("0");
        }
        this->correct = this->table.correct;
        return prediction;
    }
};
class Tournament{
public:
    GShare GS;
    Bimodal BM;
    int correct;
    std::vector<int> table;
    unsigned long long tablesize;
    Tournament(){
        this->correct = 0;
        this->GS = GShare(11);
        this->BM = Bimodal(2048, 2);
        this->table = std::vector<int>(2048, 00);
        this->tablesize = 2048;
    }
    int checkTable(unsigned long long address, std::string behavior){
        unsigned long long index = address % tablesize;
        int branchTaken = 1;
        if(behavior == "NT"){
            branchTaken = 0;
        }
        //check and modify table
        // 0 = Strong GS, 1 = Weak GS, 2 = Weak BM, 3 = Strong BM
        /* Returns the Prediction based on the following:
        *  -1 = err
        *  1 = Taken
        *  0 = Non Taken
        */
        int gsharePrediction = this->GS.checkTable(address, behavior);
        int bimodalPrediction = this->BM.checkTable(address, behavior);
        if(gsharePrediction == -1 || bimodalPrediction == -1){
            std::cout<<"one subpredictor is returning negative in tournament predictor"<<std::endl;
            return -1;
        }
        int prediction = -1;
        // choice is the selector: 0/1->gshare; 2/3->bimodal
        int & choice = this->table[index];
        /*          SAME PREDICTION            */
        if(bimodalPrediction == gsharePrediction){
            //check if prediction is correct
            if(bimodalPrediction == branchTaken){
                this->correct++;
                return bimodalPrediction;
            }
            else{
                return bimodalPrediction;
            }
        }
        //     ***If not same, one must be wrong***
        //select gshare
        else if(choice < 2){
            prediction = gsharePrediction;
            //gshare makes correct prediction
            if(gsharePrediction == branchTaken){
                this->correct++;
                //incorrect bimodal prediction
                if(choice > 0){choice--;}  
            }
            //gshare makes incorrect prediction
            else{
                //bimodal must be correct (increment)
                choice++;

            }
        }
        //select bimodal
        else if (choice >= 2){
            prediction = bimodalPrediction;
            //bimodal makes correct prediction
            if(bimodalPrediction == branchTaken){
                this->correct++;
                //gshare must be wrong
                if(choice < 3){choice++;}
            }
            //bimodal makes incorrect prediction
            else{
                //gshare must be correct (decrement)
                choice--;
            }
        }else{
            std::cout<<"choice out of bounds in tournament predictor"<<std::endl;
        }
        return prediction;
    }
};

class BTB{
public:
    Bimodal BM;
    std::vector<std::pair<unsigned long long, unsigned long long>> btb;
    int btbSize;
    int accesses;
    int hits;
    BTB(){
        this->BM = Bimodal(512, 1);
        this->btb = std::vector<std::pair<unsigned long long, unsigned long long>> (128, std::make_pair(0, 0));
        this->btbSize = 128;
        this->accesses = 0;
        this->hits = 0;
    }
    int checkTable(unsigned long long address, std::string behavior, unsigned long long target){
        int branch_taken_prediction = this->BM.checkTable(address, behavior);
        //if prediction is taken, read from BTB
        if(branch_taken_prediction == 1){
            this->accesses++;
            std::pair<unsigned long long, unsigned long long> & entry = this->btb[address % btbSize];
            //miss
            if(entry.first != address){
                //update entry
                entry = std::make_pair(address, target);
            }//hit
            else{
                if(entry.second == target){
                    this->hits++;
                }
            }
            
        }//predict non taken, dont read
        return branch_taken_prediction;
    }

};
#endif
