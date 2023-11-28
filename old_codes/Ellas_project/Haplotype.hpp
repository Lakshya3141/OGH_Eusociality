//
//  Haplotype.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 10/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Haplotype_hpp
#define Haplotype_hpp

#include "splines.hpp"

class Haplotype {
public:
    Haplotype() : genes_dispersal{initialise_genes_dispersal()}, genes_offspring_b1{initialise_genes_offspring()}, genes_sex_allocations(initialise_genes_sex_allocations()) {}

    Eigen::Matrix<double, num_knots_disp, 1> genes_dispersal;
    Eigen::Matrix<double, num_knots_off, 1> genes_offspring_b1;
    std::array<double, 2> genes_sex_allocations;
    
    Eigen::Matrix<double, num_knots_disp, 1> initialise_genes_dispersal();
    Eigen::Matrix<double, num_knots_off, 1> initialise_genes_offspring();
    std::array<double, 2> initialise_genes_sex_allocations();

    void mutate(const params& p, const int year);
};

Eigen::Matrix<double, num_knots_disp, 1> Haplotype::initialise_genes_dispersal() {
    Eigen::Matrix<double, num_knots_disp, 1> temp_matrix;
    temp_matrix(0, 0) = init_disp_int; // intercept = 3.5 means that initial prob. dispersal = 0.97
    for (int i = 1; i < num_knots_disp; ++i) {
        temp_matrix(i, 0) = 0.0; // all slopes initially = 0
    }
    return temp_matrix;
}

Eigen::Matrix<double, num_knots_off, 1> Haplotype::initialise_genes_offspring() {
    Eigen::Matrix<double, num_knots_off, 1> temp_matrix;
    temp_matrix(0, 0) = no_manip_av_bodysize; // initial intercept
    for (int i = 1; i < num_knots_off; ++i) {
        temp_matrix(i, 0) = 0.0; // all initial b1 slopes = 0.0
    }
    return temp_matrix;
}

std::array<double, 2> Haplotype::initialise_genes_sex_allocations() {
    std::array<double, 2> temp_genes {sa_spring, sa_summer};
    return temp_genes;
}

// mutate function (per-locus mutation rate not dependent on the number of loci)
void Haplotype::mutate(const params& p, const int year) {
    
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
