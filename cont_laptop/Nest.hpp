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
    bool feed(Individual<2>& female);

    // temp vector to hold male adults and then move them to population level, delete the one at nest level

    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<1> > tmp_males;              // temporary vector for adult males
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males

    int num_breeders;
private:
    // template <int Ploidy>
    // bool feedRandomLarvae(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female);
    bool feedRandomFemale(Individual<2>& female);
    bool feedRandomMale(Individual<2>& female);
};

bool Nest::feed(Individual<2>& female){
    if (larval_males.size() + larval_females.size() != 0){
        // weighted probabilties
        double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
        bool tmp = bernoulli(maleProbability);
        // tmp ? feedRandomLarva(larval_males, female) : feedRandomLarva(larval_females, female);
        tmp ? feedRandomMale(female) : feedRandomFemale(female);
    }
}

Nest::Nest(Individual<2>& f) : adult_females{f} {} // initialiser list for nests (initialised with one female)

bool Nest::feedRandomFemale(Individual<2>& female) {

    size_t selectedLarvaIndex = uni_int(0, larval_females.size() - 1);

    // Transfer resources from the female to the selected larva
    larval_females[selectedLarvaIndex].feed(female.resources);
    female.resources = 0.0;

    // Check maturity of larvae
    if (larval_females[selectedLarvaIndex].check_mature(female.t_next)) {
        adult_females.push_back(larval_females[selectedLarvaIndex]);
        larval_females.erase(larval_females.begin() + selectedLarvaIndex);
    }

    return true;
}

bool Nest::feedRandomMale(Individual<2>& female) {

    size_t selectedLarvaIndex = uni_int(0, larval_males.size() - 1);

    // Transfer resources from the female to the selected larva
    larval_males[selectedLarvaIndex].feed(female.resources);
    female.resources = 0.0;

    // Check maturity of larvae
    if (larval_males[selectedLarvaIndex].check_mature(female.t_next)) {
        tmp_males.push_back(larval_males[selectedLarvaIndex]);
        larval_males.erase(larval_males.begin() + selectedLarvaIndex);
    }

    return true;
}

// template <int Ploidy>
// bool Nest::feedRandomLarva(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female) {
//     // if (larvae.empty()) {
//     //     return false; // No larvae available to feed
//     // } Removed cause unneccessary

//     // Randomly select a larva
//     // std::uniform_int_distribution<size_t> dist(0, larvae.size() - 1);
//     size_t selectedLarvaIndex = uni_int(0, larvae.size() - 1);

//     // Transfer resources from the female to the selected larva
//     larvae[selectedLarvaIndex].feed(female.resources);
//     female.resources = 0;

//     // Check maturity of larvae
//     // Maybe we can declare two functions so there is no if checking here? Optimisation to be checked later
//     (void)larvae[selectedLarvaIndex].check_mature();
//     if (Ploidy == 2) {
//         dum_females.push_back(larvae[selectedLarvaIndex]);
//     }
//     else if (Ploidy == 1) {
//         dum_males.push_back(larvae[selectedLarvaIndex]);
//     }
//     larvae.erase(larvae.begin() + selectedLarvaIndex);

//     return true;
// }


#endif /* Nest_hpp */