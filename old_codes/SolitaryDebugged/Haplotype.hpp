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
#include <array>
#include <string>
//#include <Eigen/Dense>

// Change name of genes_choice
// initialiser lists are computtionally efficient compared to declaring body
class Haplotype {
    public:
    // Haplotype(char s) {}
    // Haplotype() : genes_dispersal{initialise_genes_dispersal()}, genes_choice{initialise_genes_choice()} {}
    Haplotype() : genes_dispersal{dInitDispersal}, genes_choice{initialise_genes_choice()} {}
    
    double genes_dispersal;
    std::array<double, 2> genes_choice;
    
    double initialise_genes_dispersal();
    std::array<double, 2> initialise_genes_choice();
    void mutate();
};

// remove function if need be
double Haplotype::initialise_genes_dispersal() {
    double temp_disp;
    temp_disp = dInitDispersal;
    return temp_disp;
}

std::array<double, 2> Haplotype::initialise_genes_choice() {
    std::array<double, 2> temp_genes = {dInitChoiceIntercept, dInitChoiceSlope};
    return temp_genes;
}


void Haplotype::mutate() {
    // For now, the mutation rate is 1 and hence mutation occurs at every step
    // mutation for dispersal gene
    if (bernoulli(dMutRate)) genes_dispersal += normal(dMutBias, dMutEffect);
    // mutation for larval growth logistic coefficients
    // write a for loop to make it elegab
    for(int i = 0; i < 2; ++i){
        if (bernoulli(dMutRate)) genes_choice[i] += normal(dMutBias, dMutEffect);
    }
}

#endif /* Haplotype_hpp */
