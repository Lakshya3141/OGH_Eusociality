//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

// Do remember that the reproduce function gotta be at the population stage
// and the mate function has to happen the moment a female is declared!
// Need to change feed function cause now that is individual level thing

#ifndef Nest_hpp
#define Nest_hpp    

#include "Individual.hpp"
#include "Random.hpp"

class Nest {
public:
    Nest(Individual<2>& f);
    int feed();
    //  vector for larvae as well

    // temp vector to hold male adults and then move them to population level, delete the one at nest level
    // feed function loops through female, choose random larvae to give food to
    std::vector<Individual<2> > adult_females;          // vector of adult females, [0] = breeder
    std::vector<Individual<1> > tmp_males;              // temporary vector for adult males
    std::vector<Individual<2> > larval_females;         // vector to store larval females
    std::vector<Individual<1> > larval_males;           // vector to store larval males

    int num_breeders;
private:
    template <int Ploidy>
    bool feedRandomLarva(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female);
};

int Nest::feed(){

}

Nest::Nest(Individual<2>& f) : adult_females{f} {} // initialiser list for nests (initialised with one female)

template <int Ploidy>
bool Nest::feedRandomLarva(std::vector<Individual<Ploidy> >& larvae, Individual<2>& female) {
    if (larvae.empty()) {
        return false; // No larvae available to feed
    }

    // Randomly select a larva
    // std::uniform_int_distribution<size_t> dist(0, larvae.size() - 1);
    size_t selectedLarvaIndex = uni_int(0, larvae.size() - 1);

    // Transfer resources from the female to the selected larva
    larvae[selectedLarvaIndex].feed(female.resources);
    female.resources = 0;

    // Check maturity of larvae
    (void)larvae[selectedLarvaIndex].check_mature()
    if (Ploidy == 2) {
        dum_females.push_back(larvae[selectedLarvaIndex]);
    }
    else if (Ploidy == 1) {
        dum_males.push_back(larvae[selectedLarvaIndex]);
    }
    larvae.erase(larvae.begin() + selectedLarvaIndex);

    return true;
}

int Nest::feed() {
    int numFed = 0;
    std::vector<Individual<2> > dum_females;            // vector to store larval females DUMMY
    std::vector<Individual<1> > dum_males;              // vector to store larval males DUMMY
    for (Individual<2>& female : adult_females) {
        if (female.is_foraging) {
            // Decide whether to feed male or female larvae
            double maleProbability = static_cast<double>(larval_males.size()) / (larval_males.size() + larval_females.size());
            bool tmp = bernoulli(maleProbability);

            if (tmp) {
                // Feed a male larva
                if (feedRandomLarva(larval_males, female)) {
                    numFed++;
                }
            } else {
                // Feed a female larva
                if (feedRandomLarva(larval_females, female)) {
                    numFed++;
                }
            }
        }
    }

    // remove all the females from dum_females that have been fed and now matured
    auto partitionIterF = std::partition(dum_females.begin(), dum_females.end(), [](Individual<2> indi) {
        return !indi.is_larvae; 
    });

    // add those to adult females
    adult_females.assign(partitionIterF, dum_females.end());
    // remove them from dum_females
    dum_females.erase(partitionIterF, dum_females.end());

    // Same process repeated for males
    auto partitionIterM = std::partition(dum_males.begin(), dum_males.end(), [](Individual<1> indi) {
        return !indi.is_larvae;
    });
    tmp_males.assign(partitionIterM, tmp_males.end());
    dum_males.erase(partitionIterM, tmp_males.end());

    // concatenate dum and larval vectors
    larval_males.insert(larval_males.end(), dum_males.begin(), dum_males.end());
    larval_females.insert(larval_females.end(), dum_females.begin(), dum_females.end());

    return numFed;
}

#endif /* Nest_hpp */