#ifndef PREDICTORS_HEADER
#define PREDICTORS_HEADER
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
void alwaysTaken(unsigned long long , std::string , unsigned long long , int &);
void alwaysNonTaken(unsigned long long , std::string , unsigned long long , int &);
void predictor();

class Bimodal{
public:    
    std::vector<int> table;
    int tablesize;
    int bits;
    Bimodal(){
        table.resize(0);
        bits = 0;
        tablesize = 0;
        std::cout << "empty"<<std::endl;
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
        table.resize(tablesize);
        if(bits == 1){
            //Initiallize all entries to Taken
            for(int i = 0; i < tablesize; i++){
                table[i] == 3;
            }
        }
        else if(bits == 2){
            //Initiallize all entries to Strongly Taken
            for(int i = 0; i < tablesize; i++){
                table[i] == 3;
            }
        }
        else{
            std::cout << "err with bit num" <<std::endl;
        }
    }
    bool checkTable(int entry_number, std::string behavior, int & correct){
        if(behavior != "T" && behavior != "NT") std::cout<< "incorrect behavior"<<std::endl;
        //updates if wrong//
        // 0 <= entry <= tablesize
        if(bits == 1){
            //TAKEN PREDICTION
            if(behavior == "T" && table[entry_number] == 3){
                correct ++;
                return true;
            }//NON TAKEN PREDICTION
            else if(behavior == "T" && table[entry_number] == 0){
                table[entry_number] = 3;
                return false;
            }//TAKEN PREDICTION
            else if(behavior == "NT" && table[entry_number] == 3){
                table[entry_number] = 0;
                return false;
            }//NON TAKEN PREDICTION
            else if(behavior == "NT" && table[entry_number] == 0){
                correct++;
                return true;
            }
            else return false;
        }
        else if(bits == 2){
            int prediction = table[entry_number];
            //TAKEN PREDICTION
            if(prediction >=2 && behavior == "T"){ 
                correct++;
                table[entry_number] = 3; //go to strongly taken if not already
                return true; 
            }//NON TAKEN PREDICTION
            if(prediction < 2 && behavior == "NT"){
                correct++;
                table[entry_number] = 0;
                return true;
            }
            //incorrect prediction
            if(behavior == "T"){
                if(table[entry_number] == 3) std::cout << "prediction err"<<std::endl;
                else table[entry_number] = table[entry_number] + 1;
            }
            else if (behavior == "NT"){
                if(table[entry_number] == 0) std::cout << "prediction err"<<std::endl;
                else table[entry_number] = table[entry_number] - 1;
            }
            return false;
        }
        else{
            std::cout <<  "err with bit num" <<std::endl;
        }
        return false;
    }
    

};


#endif
