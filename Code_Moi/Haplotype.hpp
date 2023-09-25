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
#include <Eigen/Dense>

class Haplotype {
    public:
    Haplotype() : genes_dispersal{initialise_genes_dispersal()}, genes_growth{initialise_genes_growth()} {}
    double genes_dispersal;
    std::array<double, 2> genes_growth;
    
    double initialise_genes_dispersal();
    std::array<double, 2> initialise_genes_growth();
    void mutate();
}

double Haplotype::initialise_genes_dispersal() {
    double temp_disp;
    disp = dInitDispersal;
    return disp;
}

std::array<double, 2> Haplotype::initialise_genes_growth() {
    std::array<double, 2> temp_genes {dInitGrowthIntercept, dInitGrowthSlope};
    return temp_genes;
}

void Haplotype::mutate() {
    
    // dispersal genes only mutate after no dispersal evolution phase has ended
    if (year >= p.years_no_disp_evol+1) {
        for (int i = 0; i<num_knots_disp; ++i) {
            if (bernoulli(p.mut_rate)) genes_dispersal(i, 0) += normal(p.mut_bias, p.mut_effect);
        }
    }
    
    // manipulation genes only mutate after no manipulation phase has ended
    if (year >= p.years_no_manip+1) {
        for (int i = 0; i < num_knots_off; ++i) {
            if (bernoulli(p.mut_rate)) genes_offspring_b1(i, 0) += normal(p.mut_bias, p.mut_effect);
        }
    }
    
    // sex allocation genes only mutate after no sex allocation evol phase has ended
    if (year >= p.years_no_sa_evol+1) {
       for (auto& g : genes_sex_allocations) {
            if (bernoulli(p.mut_rate)) g += normal(p.mut_bias, p.mut_effect);
        }
    }
}

#endif /* Haplotype_hpp */