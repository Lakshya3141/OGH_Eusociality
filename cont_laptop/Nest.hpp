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
    template <int Ploidy>
    Individual<Ploidy>* feed(Individual<2>& female);
    bool reproduce(Individual<2>& female);
    
    Individual<1>* check_maturity(Individual<1>& male_larva);
    Individual<2>* check_maturity(Individual<2>& female_larva);

    // temp vector to hold male adults and then move them to population level, delete the one at nest level

    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<1> > tmp_males;              // temporary vector for adult males
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males

    int num_breeders;
private:
    // template <int Ploidy>
    // bool feedRandomLarvae(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female);
    Individual<2>* feedRandomFemale(Individual<2>& female);
    Individual<1>* feedRandomMale(Individual<2>& female);
};


template <int Ploidy>
Individual<Ploidy>* Nest::feed(Individual<2>& female){
    if (larval_males.size() + larval_females.size() != 0){
        // weighted probabilties
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        
        // tmp ? feedRandomLarva(larval_males, female) : feedRandomLarva(larval_females, female);
        if(bernoulli(maleProbability)) {
            return feedRandomMale(female);
        }
        else return feedRandomFemale(female);
    }
    return NULL;
}

Nest::Nest(Individual<2>& f) : adult_females{f} {} // initialiser list for nests (initialised with one female)

Individual<2>* Nest::feedRandomFemale(Individual<2>& female) {

    size_t selectedLarvaIndex = uni_int(0, larval_females.size() - 1);
    female.t_next = female.t_next + dForagingTime;
    // Transfer resources from the female to the selected larva
    larval_females[selectedLarvaIndex].feed(female.resources);
    female.resources = 0.0;
    larval_females[selectedLarvaIndex].t_next = female.t_next;
    return &larval_females[selectedLarvaIndex];

    // // Check maturity of larvae
    // if (larval_females[selectedLarvaIndex].check_mature(female.t_next)) {
    //     adult_females.push_back(larval_females[selectedLarvaIndex]);
    //     larval_females.erase(larval_females.begin() + selectedLarvaIndex);
    // }
}

Individual<1>* Nest::feedRandomMale(Individual<2>& female) {

    size_t selectedLarvaIndex = uni_int(0, larval_males.size() - 1);

    female.t_next = female.t_next + dForagingTime;
    // Transfer resources from the female to the selected larva
    larval_males[selectedLarvaIndex].feed(female.resources);
    female.resources = 0.0;
    larval_males[selectedLarvaIndex].t_next = female.t_next;

    return &larval_males[selectedLarvaIndex];
    // // Check maturity of larvae // shift to another function/
    // if (larval_males[selectedLarvaIndex].check_mature(female.t_next)) {
    //     tmp_males.push_back(larval_males[selectedLarvaIndex]);
    //     larval_males.erase(larval_males.begin() + selectedLarvaIndex);
    // }
}

bool Nest::reproduce(Individual<2>& female){
    if (bernoulli(const_sex_ratio)){
        Individual<1> son(female);
        larval_males.push_back(son);
    }
    else {
        Individual<2> daughter(female);
        larval_females.push_back(daughter);
    }
}



#endif /* Nest_hpp */