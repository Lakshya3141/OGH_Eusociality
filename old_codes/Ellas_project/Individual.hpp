//
//  Individual.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 10/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Individual_hpp
#define Individual_hpp

#include "Haplotype.hpp"

using diploid_genome = std::array<Haplotype, 2>;
using haploid_genome = std::array<Haplotype, 1>;
using sperm_genomes = std::vector<haploid_genome>;

template <int Ploidy>
class Individual {
public:
    // Individual constructors
    Individual() = default;
    Individual(const diploid_genome& genome_mum, const params& p, const int year); // male
    Individual(const diploid_genome& genome_mum, const haploid_genome& genome_dad, const params& p, const int year); // female
    
    std::array<Haplotype, Ploidy> genome; // genome of individual with 1 or 2 haplotypes
    sperm_genomes sperm; // sperm vector to allow for multiple matings
    
    void mate(const sperm_genomes& sg);
    void mutate(const params& p, const int year);
    void calculate_phenotype();
    
    Eigen::Matrix<double, num_knots_disp, 1> phenotype_dispersal;
    std::array<double, 2> phenotype_sex_allocations;
    Eigen::Matrix<double, num_knots_off, 1> phenotype_offspring_b1;
    
    bool is_alive = true;
    void survival(const double prob);
    bool check_dispersal(const Eigen::MatrixXd& basis_matrix);
    bool disperser = false;
    bool helper = false;
    void range_bodysize();
    
    double body_size;
    int num_offspring;     // number of offspring a foundress will produce
    int num_fem_offspring; // number of female offspring a foundress will produce
    
};

// function to check female probability of dispersal
template<>
bool Individual<2>::check_dispersal(const Eigen::MatrixXd& basis_matrix) {
    // dispersal spline
  
    double y = evaluate_spline2d(basis_matrix, phenotype_dispersal, body_size);
    bool dispersal = bernoulli(logistic(y, 0.0, -1.0));

    if (dispersal) disperser = true; // record as disperser
    return dispersal;
}		

// function to ensure body size stays within the range (within matrix)
template <int Ploidy>
void Individual<Ploidy>::range_bodysize() {
    if(body_size > max_bodysize) body_size = max_bodysize;
    if(body_size < 0.0) body_size = 0.0;
}

// survival function
template <int Ploidy>
void Individual<Ploidy>::survival(const double prob) {
    if (bernoulli(prob)) is_alive = true;
    else is_alive = false;
    
    if(body_size <= 0.0) is_alive = false; // ensure body size cannot be 0 or negative
}

// mate function
template <>
void Individual<2>::mate(const sperm_genomes& sg) {
    sperm = sg; // females store sperm from mate
}

// constructor for males (haploid)
template <>
Individual<1>::Individual (const diploid_genome& genome_mum, const params& p, const int year) {
    // son inherits genes from mum (with recombination - 50:50 chance of each haplotype)
    for(int i=0; i<num_knots_disp; ++i) {
        genome[0].genes_dispersal(i, 0) = genome_mum[bernoulli()].genes_dispersal(i, 0);
    }
    for(int i=0; i<num_knots_off; ++i) {
        genome[0].genes_offspring_b1(i, 0) = genome_mum[bernoulli()].genes_offspring_b1(i, 0);
    }
    for(int i=0; i<2; ++i) {
        genome[0].genes_sex_allocations[i] = genome_mum[bernoulli()].genes_sex_allocations[i];
    }
    mutate(p, year); // individual is created then mutates
}

// determine female phenotype from picking one haplotype at random
template<>
void Individual<2>::calculate_phenotype() {
    
    for(int i=0; i<num_knots_disp; ++i) {
        phenotype_dispersal(i, 0) = genome[bernoulli()].genes_dispersal(i, 0);
    }
    for(int i=0; i<num_knots_off; ++i) {
        phenotype_offspring_b1(i, 0) = genome[bernoulli()].genes_offspring_b1(i, 0);
    }
    for(int i=0; i<2; ++i) {
        phenotype_sex_allocations[i] = genome[bernoulli()].genes_sex_allocations[i];
    }
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const diploid_genome& genome_mum, const haploid_genome& genome_dad, const params& p, const int year) {

    // daughter inherits one haplotype from dad
    genome[0].genes_dispersal = genome_dad[0].genes_dispersal;
    genome[0].genes_offspring_b1 = genome_dad[0].genes_offspring_b1;
    genome[0].genes_sex_allocations = genome_dad[0].genes_sex_allocations;
    
    // daughter inherits one haplotype from mum (with recombination)
    for(int i=0; i<num_knots_disp; ++i) {
        genome[1].genes_dispersal(i, 0) = genome_mum[bernoulli()].genes_dispersal(i, 0);
    }
    for(int i=0; i<num_knots_off; ++i) {
        genome[1].genes_offspring_b1(i, 0) = genome_mum[bernoulli()].genes_offspring_b1(i, 0);
    }
    for(int i=0; i<2; ++i) {
        genome[1].genes_sex_allocations[i] = genome_mum[bernoulli()].genes_sex_allocations[i];
    }
    mutate(p, year);
    calculate_phenotype();
}

// mutate function
template <int Ploidy>
void Individual<Ploidy>::mutate(const params& p, const int year) {
    for (int i=0; i<Ploidy; ++i) {
        genome[i].mutate(p, year);
    }
}

#endif /* Individual_hpp */
