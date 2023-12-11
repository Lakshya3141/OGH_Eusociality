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
#include <algorithm>
#include <cmath>

class Nest {
public:
    Nest(const int nid, const Individual<2>& f);
    int feed(const double& mean, const double& SD);
    int reproduce(Individual<2>& female);
    bool is_empty();
    void task_check(Individual<2>& current);
    size_t findFemaleIndexById(unsigned long int id);
    // temp vector to hold male adults and then move them to population level, delete the one at nest level

    int nest_id;
    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    // std::vector<Individual<1> > tmp_males;              // temporary vector for adult males
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males

    int num_breeders;
};


// returns integer. If 0, no larvae fed. If positive, |index| - 1 is position of female larva
// If negative, |index| - 1 is position of male larva
//LC1: Need to decide what to do in the alternate case of no larvae
int Nest::feed(const double& mean, const double& SD) {
    if (larval_males.size() + larval_females.size() > 0){ // weighted probabilties
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        double resources = normal(mean, SD);
        // std::cout << "      resource: " << resources << std::endl;
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

Nest::Nest(const int nid, const Individual<2>& f) {
    adult_females.push_back(f);
    nest_id = nid;
} // Constructor for nest

// produces males if not mated or with bernoulli(male sex ratio)
// returns 0 if male born, 1 if female
int Nest::reproduce(Individual<2>& female){
    if (bernoulli(const_sex_ratio) || !female.is_mated) {
        Individual<1> son(IndID++, female);
        larval_males.emplace_back(son);
        return 0;
    }
    else {
        Individual<2> daughter(IndID++, female);
        larval_females.emplace_back(daughter);
        return 1;
    }
}

bool Nest::is_empty(){
    if (adult_females.size() == 0){
        return true;
    }
    else return false;
}


void Nest::task_check(Individual<2>& current){
    // LCI: Comfirm these
    current.is_foraging = bernoulli(logistic(larval_females.size() + larval_males.size(), current.phenotype_choice[0], current.phenotype_choice[1]));
    // current.t_prev = current.t_next;
    current.t_next = current.t_next + (current.is_foraging ? dForagingTime : dBroodingTime);
}

// Function to search by indidivual ID and return index + 1
size_t Nest::findFemaleIndexById(unsigned long int id) {
    auto it = std::find_if(adult_females.begin(), adult_females.end(), [id](const Individual<2>& ind) {
        return ind.ind_id == id;
    });

    if (it != adult_females.end()) {
        return std::distance(adult_females.begin(), it) + 1;
    } else {
        // Return some sentinel value (e.g., adult_females.size()) to indicate not found
        return 0; //adult_females.size();
    }
}


#endif /* Nest_hpp */
