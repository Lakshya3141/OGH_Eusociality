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
private:
    // template <int Ploidy>
    // bool feedRandomLarvae(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female);
    //int feedRandomFemale(Individual<2>& female); //JK: no longer needed
    //int feedRandomMale(Individual<2>& female); //JK: no longer needed
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
//JK: lots of changes in this function
int Nest::feed(const double& mean, const double& SD) {
    if (larval_males.size() + larval_females.size() > 0){ // weighted probabilties
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        double resources = normal(mean, SD);
        if (resources < 0.0) resources = 0.0;
        if (bernoulli(maleProbability)) {
            size_t ind = uni_int(0, larval_males.size());
            larval_males[ind].body_size += resources;
            return -ind;
        } else {
            size_t ind = uni_int(0, larval_females.size());
            larval_females[ind].body_size += resources;
            return ind;
        }
    }
    return 0;
}

Nest::Nest(Individual<2>& f) : adult_females{f} {} // initialiser list for nests (initialised with one female)


//JK: the next two functions are both no longer needed. can be deleted
//int Nest::feedRandomFemale(Individual<2>& female) {

    //size_t selectedLarvaIndex = uni_int(0, larval_females.size());
    //female.t_next += dForagingTime;
    // Transfer resources from the female to the selected larva
    //larval_females[selectedLarvaIndex].feed(female.resources);
    //female.resources = 0.0;
    //larval_females[selectedLarvaIndex].t_next = female.t_next;
    //return -selectedLarvaIndex - 1;

    // // Check maturity of larvae
    // if (larval_females[selectedLarvaIndex].check_mature(female.t_next)) {
    //     adult_females.push_back(larval_females[selectedLarvaIndex]);
    //     larval_females.erase(larval_females.begin() + selectedLarvaIndex);
    // }
//}

//int Nest::feedRandomMale(Individual<2>& female) {

    //size_t selectedLarvaIndex = uni_int(0, larval_males.size());

    //female.t_next += dForagingTime;
    // Transfer resources from the female to the selected larva
    //larval_males[selectedLarvaIndex].feed(female.resources);
    //female.resources = 0.0;
    //larval_males[selectedLarvaIndex].t_next = female.t_next;

    //return selectedLarvaIndex + 1;
//}

void Nest::reproduce(Individual<2>& female){ //JK: made this a void function
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
