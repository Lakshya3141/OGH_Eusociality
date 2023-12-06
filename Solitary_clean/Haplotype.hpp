//
//  Haplotype.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#ifndef Haplotype_hpp
#define Haplotype_hpp

#include "Parameters.hpp"
#include "Random.hpp"

// Change name of genes_choice
// initialiser lists are computtionally efficient compared to declaring body
class Haplotype {
    public:
    // Haplotype() : genes_dispersal{dInitDispersal}, genes_choice{initialise_genes_choice()} {}
    Haplotype() : genes_dispersal{dInitDispersal}, genes_choice{{dInitChoiceIntercept, dInitChoiceSlope}} {}
    
    double genes_dispersal;
    std::array<double, 2> genes_choice; // IP: why 2? choice of task?
    // I see now: one is intercept, other slope. But wrt what?
    // LCIP: It defines a logistic function
    
    std::array<double, 2> initialise_genes_choice();
    void mutate(const params& p);
};

// function to initialise choice genes in haplotype declaration
std::array<double, 2> Haplotype::initialise_genes_choice() {
    std::array<double, 2> temp_genes = {dInitChoiceIntercept, dInitChoiceSlope};
    return temp_genes;
}

// mutate function
// For now, the mutation rate is 1 and hence mutation occurs at every step
void Haplotype::mutate(const params& p) {
    // mutation for dispersal gene
    if (bernoulli(p.dMutRate)) genes_dispersal += normal(p.dMutBias, p.dMutEffect);
    // mutation for tsak choice logistic coefficients
    for(int i = 0; i < 2; ++i){
        if (bernoulli(p.dMutRate)) genes_choice[i] += normal(p.dMutBias, p.dMutEffect);
    }
}

#endif /* Haplotype_hpp */
