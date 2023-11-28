//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Nest_hpp
#define Nest_hpp    

#include "Individual.hpp"
#include <algorithm>
#include <cmath>
#include <tuple>

class Nest {
public:
    Nest(const Individual<2>& f, const int nid) ;
    std::tuple<bool, bool, size_t> feed(const double& mean, const double& SD); // first bool refers to whether there are any larvae; second bool to whether a female = true or male = false was fed; size_t is larval index
    void reproduce(Individual<2>& female, const int id);
    void task_check(Individual<2>& current);
    size_t findFemaleIndexById(unsigned long int id);

    int nest_id;
    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males
};


std::tuple<bool, bool, size_t> Nest::feed(const double& mean, const double& SD) {
    std::cout << "f# larvae: " << larval_males.size() + larval_females.size()
    << " | f# male larvae: " << larval_males.size()
    << " | f# female larvae: " << larval_females.size() << std::endl; // TST DELETE
    if ((larval_males.size() + larval_females.size()) > 0){ // weighted probabilities
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        double resources = normal(mean, SD);
        // std::cout << "      resource: " << resources << std::endl;
        if (resources < 0.0) resources = 0.0;
        if (bernoulli(maleProbability)) {
            size_t ind = uni_int(0, larval_males.size());
            larval_males[ind].body_size += resources;
            return std::make_tuple(true, false, ind);
        } else {
            size_t ind = uni_int(0, larval_females.size());
            std::cout << "ind " << ind << std::endl;
            std::cout << "larval_females.size() " << larval_females.size() << std::endl;
            larval_females[ind].body_size += resources;
            return std::make_tuple(true, true, ind);
        }
    }
    return std::make_tuple(false, true, 0);
}

Nest::Nest(const Individual<2>& f, const int nid) : nest_id(nid) {
    adult_females.push_back(f);
} // Constructor for nest

// produces males if not mated or with bernoulli(male sex ratio)
void Nest::reproduce(Individual<2>& female, const int id) {
    if (bernoulli(const_sex_ratio) || !female.is_mated) {
        larval_males.emplace_back(id, female);
    } else {
        larval_females.emplace_back(id, female);
    }
}


void Nest::task_check(Individual<2>& current){
    current.is_foraging = bernoulli(logistic(larval_females.size() + larval_males.size(), current.phenotype_choice[0], current.phenotype_choice[1]));
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
