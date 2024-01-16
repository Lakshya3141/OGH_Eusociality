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
#include "Splines.hpp"

// using haplotype_choice_type = Eigen::Matrix<double, n_knots * n_knots, 1>;
using haplotype_choice_type = Eigen::Matrix<double, n_knots, 1>; // we use the signal one

// Change name of genes_choice
// initialiser lists are computtionally efficient compared to declaring body
class Haplotype {
    public:
    // Haplotype() : genes_dispersal{dInitDispersal}, genes_choice{initialise_genes_choice()} {}
    Haplotype() : genes_dispersal{dInitDispersal}, haplotype_choice{init_haplo_choice()} {}

    double genes_dispersal;
    haplotype_choice_type haplotype_choice;
    
    haplotype_choice_type init_haplo_choice();
    void mutate(const params& p);
};

haplotype_choice_type Haplotype::init_haplo_choice() {
    haplotype_choice_type init_matrix;
    for (int i = 0; i < n_knots * n_knots; ++i) {
        init_matrix(i, 0) = 0.0;
    }
    return init_matrix;
}

// mutate function
void Haplotype::mutate(const params& p) {
    // mutation for dispersal gene
    if (bernoulli(p.dMutRate)) genes_dispersal += normal(p.dMutBias, p.dMutEffect);
    // mutation for task choice knot values
    for (int i = 0; i < n_knots * n_knots; ++i) {
        if (bernoulli(p.dMutRate)) haplotype_choice(i, 0) += normal(p.dMutBias, p.dMutEffect);
    }
}

#endif /* Haplotype_hpp */
