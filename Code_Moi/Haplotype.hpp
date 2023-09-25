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
//#include <Eigen/Dense>

class Haplotype {
    public:
    Haplotype() : genes_dispersal{initialise_genes_dispersal()}, genes_growth{initialise_genes_growth()} {}
    double genes_dispersal;
    std::array<double, 2> genes_growth;
    
    double initialise_genes_dispersal();
    std::array<double, 2> initialise_genes_growth();
    void mutate();
};

double Haplotype::initialise_genes_dispersal() {
    double temp_disp;
    temp_disp = dInitDispersal;
    return temp_disp;
}

std::array<double, 2> Haplotype::initialise_genes_growth() {
    std::array<double, 2> temp_genes = {dInitGrowthIntercept, dInitGrowthSlope};
    return temp_genes;
}

void Haplotype::mutate() {
    // For now, the mutation bias is 1 and hence mutation occurs at every step

    // mutation for dispersal gene
    if (bernoulli(dMutRate)) genes_dispersal += normal(dMutBias, dMutEffect);
    
    // mutation for larval growth logistic coefficients
    if (bernoulli(dMutRate)) genes_growth[0] += normal(dMutBias, dMutEffect);
    if (bernoulli(dMutRate)) genes_growth[1] += normal(dMutBias, dMutEffect);

}

#endif /* Haplotype_hpp */