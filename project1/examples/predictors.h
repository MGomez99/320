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
void predictor();

class Bimodal{
public:    
    std::vector<int> table;
    int tablesize;
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
        this->table =  std::vector<int>(tablesize, 3);
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
        // 0 <= entry <= tablesize
        if(bits == 1){
            //Correct TAKEN PREDICTION
            if(behavior == "T" && table[entry_number] == 3){
                correct ++;
                return 1;
            }//InCorrect NON TAKEN PREDICTION
            else if(behavior == "T" && table[entry_number] == 0){
                table[entry_number] = 3;
                return 0;
            }//InCorrect TAKEN PREDICTION
            else if(behavior == "NT" && table[entry_number] == 3){
                table[entry_number] = 0;
                return 1;
            }//Correct NON TAKEN PREDICTION
            else if(behavior == "NT" && table[entry_number] == 0){
                correct++;
                return 0;
            }
            else{
                /*std::cout<<"Bug in Code, index = "<< index<<", entry no = "<<entry_number<< 
                ", BITS = " << this->bits <<", SIZE = "<<this->tablesize <<", VALUE = " <<table[entry_number]<<
                ", BEHAVIOR = "<< behavior << std::endl;*/
                return -1;
            } 
        }
        else if(bits == 2){
            int prediction = table[entry_number];
            if(prediction < 0) std::cout<<"ERR, NEGATIVE PREDICTION"<<std::endl;
            //Correct TAKEN PREDICTION
            if(prediction >=2 && behavior == "T"){ 
                //std::cout << "CORRECT" << std::endl;
                correct++;
                table[entry_number] = 3; //go to strongly taken if not already
                return 1; 
            }//Correct NON TAKEN PREDICTION
            else if(prediction < 2 && behavior == "NT"){
                //std::cout << "CORRECT" << std::endl;
                correct++;
                table[entry_number] = 0;
                return 0;
            }
            //InCorrect TAKEN PRECICTION
            else if(prediction < 2 && behavior == "T"){
                if(table[entry_number] == 3) std::cout << "prediction err"<<std::endl;
                else table[entry_number] += 1;
                return 1;
            }
            //InCorrect NONTAKEN PRECICTION
            else if (prediction >= 2 && behavior == "NT"){
                if(table[entry_number] == 0) std::cout << "prediction err"<<std::endl;
                else table[entry_number] -= 1;
                return 0;
            }
            else{ std::cout<<"err line 95"<<std::endl;}
            return -1;
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
        if(prediction == 1){
            history_register = history_register.substr(1, history_register.size()) + "1";
        }else{
            history_register = history_register.substr(1, history_register.size()) + "0";
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
    Tournament(){
        this->correct = 0;
        this->GS = GShare(11);
        this->BM = Bimodal(2048, 1);
        this->table = std::vector<int>(2048, 0);
    }
    int checkTable(unsigned long long address, std::string behavior){
        int index = address%2048;
        //check and modify table
        // 0 = Strong GS, 1 = Weak GS, 2 = Weak BM, 3 = Strong BM
        /* Returns the Prediction based on the following:
        *  -1 = err
        *  1 = Taken
        *  0 = Non Taken
        */
        int gsharePrediction = GS.checkTable(address, behavior);
        int bimodalPrediction = BM.checkTable(address, behavior);
        int change = 0;
        if(gsharePrediction!=bimodalPrediction){
            //std::cout<<"not same"<<std::endl;
            change = 1; //if 1, change the state; else dont change state of table
        }
        //else std::cout<<"same"<<std::endl;
        int retval = -1;
        //0/1->prefer gshare; 2/3 -> prefer bimodal
        if(table[index] < 2){
            retval = gsharePrediction;
        }
        else if(table[index] >= 2){
            retval = bimodalPrediction;
        }
        else std::cout<<"err"<<std::endl;
        if(retval == -1 || bimodalPrediction == -1 || gsharePrediction == -1) return -1;
        if(change){
            //gshare predicts correctly, bimodal incorrect
            if((gsharePrediction == 1 && behavior == "T") || (gsharePrediction == 0 && behavior == "NT")){
                table[index] -= 1;//Move towards favoring gshare
                if(table[index] < 0) table[index] = 0;
            }
            //bimodal predicts correctly, gshare incorrect
            else if ((bimodalPrediction == 1 && behavior == "T") || (bimodalPrediction == 0 && behavior == "NT")){
                table[index] += 1;
                if(table[index] > 3) table[index] = 3;
            }
        }
        //update correct
        if((retval == 1 && behavior == "T") || (retval == 0 && behavior == "NT")) {this->correct++;}
        else std::cout<<"wrong"<<std::endl;
        return retval;
        /*
            For some reason, every prediction is correct but there are changes occuring (i.e. one is wrong)
            check comparison logic for change
            check values in index


        */
    }
    


};
#endif
