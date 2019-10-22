#ifndef PREDICTORS_HEADER
#define PREDICTORS_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
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
        unsigned long long index = (address ^ stoi(history_register, nullptr, 2)); //xor address with register to get index
        int prediction = this->table.checkTable(index, behavior);
        // if branch taken, shift left and store a 1, else 0
        history_register = history_register.substr(1, history_register.size()-1);
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
        this->table = std::vector<int>(2048, 0);
        this->tablesize = 2048;
    }
    int checkTable(unsigned long long address, std::string behavior){
        unsigned long long index = address % tablesize;
        int branchTaken = 0;
        if(behavior == "T"){
            branchTaken = 1;
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
        int prediction = -1;
        //select gshare
        int & choice = this->table[index];
        if(choice < 2){
            prediction = gsharePrediction;
            //gshare makes correct prediction
            if(gsharePrediction == branchTaken){
                this->correct++;
                //incorrect bimodal prediction
                if(bimodalPrediction != branchTaken){
                    choice--;
                }
            }
            //gshare makes incorrect prediction
            else{
                //bimodal makes correct prediction
                if(bimodalPrediction == branchTaken){
                    if(choice < 3){
                        choice++;
                    }
                }
            }
        }
        //select bimodal
        else{
            prediction = bimodalPrediction;
            //if bimodal is correct
            if(bimodalPrediction == branchTaken){
                this->correct++;
                //if gshare is wrong
                if(gsharePrediction != branchTaken){
                    if(choice < 3){
                        choice++;
                    }
                }
            }
            //if bimodal is wrong
            else{
                //if gshare correct
                if(gsharePrediction == branchTaken){
                    if(choice > 0){
                        choice--;
                    }
                }
            }
        }
        return prediction;
    }
};
#endif
