//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Nest_hpp
#define Nest_hpp    

#include "Individual.hpp"
#include "Random.hpp"

class Nest {
public:
    Nest(Individual<2>& f);
    int feed(const double& mean, const double& SD);
    void reproduce(Individual<2>& female);
    int check_maturity(int& index, double c_time); //only for females
    bool is_empty();

    // temp vector to hold male adults and then move them to population level, delete the one at nest level

    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    // std::vector<Individual<1> > tmp_males;              // temporary vector for adult males
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males

    int num_breeders;
};

// check maturity for female larva
// returns -1 if no mature
// returns 0 if no disperser
// returns 1 if matures and disperses 
int Nest::check_maturity(int& index, double c_time){
    // Check maturity of larvae // shift to another function/
    if(!larval_females[index].check_mature(c_time)){
        return -1;
    }
    else if (larval_females[index].is_disperser){
        return 1;
    }
    else {
        adult_females.emplace_back(larval_females[index]);
        larval_females.erase(larval_females.begin() + index);
        return 0;
    }
}


// returns integer. If 0, no larvae fed. If negative, |index| - 1 is position of female larva
// If positive, |index| - 1 is position of male larva
//LC1: Need to decide what to do in the alternate case of no larvae
int Nest::feed(const double& mean, const double& SD) {
    if (larval_males.size() + larval_females.size() > 0){ // weighted probabilties
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        double resources = normal(mean, SD);
        if (resources < 0.0) resources = 0.0;
        if (bernoulli(maleProbability)) {
            size_t ind = uni_int(0, larval_males.size());
            larval_males[ind].body_size += resources;
            return static_cast<int>(-ind -1);
        } else {
            size_t ind = uni_int(0, larval_females.size());
            larval_females[ind].body_size += resources;
            return static_cast<int>(ind + 1);
        }
    }
    return 0;
}

Nest::Nest(Individual<2>& f) : adult_females{f} {} // initialiser list for nests (initialised with one female)

void Nest::reproduce(Individual<2>& female){
    if (bernoulli(const_sex_ratio)){
        Individual<1> son(female);
        larval_males.emplace_back(son);
    }
    else {
        Individual<2> daughter(female);
        larval_females.emplace_back(daughter);
    }
}

bool Nest::is_empty(){
    if (adult_females.size() == 0){
        return true;
    }
    else return false;
}


#endif /* Nest_hpp */
