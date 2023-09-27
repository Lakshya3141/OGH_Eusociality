//
//  Individual.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//  Large code snippets borrowed from Ella Rees-Baylis

#ifndef Individual_hpp
#define Individual_hpp

#include "Haplotype.hpp"
#include "Random.hpp"

using diploid_genome = std::array<Haplotype, 2>;
using haploid_genome = std::array<Haplotype, 1>;
using sperm_genomes = std::vector<haploid_genome>;

template <int Ploidy>
class Individual {
public:
    // Individual constructors
    Individual() = default;
    Individual(const diploid_genome& genome_mum); // male
    Individual(const diploid_genome& genome_mum, const haploid_genome& genome_dad); // female
    
    std::array<Haplotype, Ploidy> genome; // genome of individual with 1 or 2 haplotypes
    sperm_genomes sperm; // sperm vector to allow for multiple matings
    
    void mate(const sperm_genomes& sg);
    void mutate();
    void calculate_phenotype();
    
    double phenotype_dispersal;
    std::array<double, 2> phenotype_growth;
    
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

// constructor for males (haploid)
template <>
Individual<1>::Individual (const diploid_genome& genome_mum) {
    // son inherits genes from mum (with 50:50 chance of each haplotype)
    genome[0].genes_dispersal = genome_mum[bernoulli()].genes_dispersal;
    genome[0].genes_growth = genome_mum[bernoulli()].genes_growth;
    mutate(); // individual is created then mutates
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const diploid_genome& genome_mum, const haploid_genome& genome_dad) {

    // daughter inherits one haplotype from dad
    genome[0].genes_dispersal = genome_dad[0].genes_dispersal;
    genome[0].genes_growth = genome_dad[0].genes_growth;
    
    // daughter inherits one haplotype from mum at random
    genome[1].genes_dispersal = genome_mum[bernoulli()].genes_dispersal;
    genome[1].genes_growth = genome_mum[bernoulli()].genes_growth;
    mutate();
    calculate_phenotype();
}

// mate function
template <>
void Individual<2>::mate(const sperm_genomes& sg) {
    sperm = sg; // females store sperm from mate
}

// mutate function
template <int Ploidy>
void Individual<Ploidy>::mutate() {
    for (int i=0; i<Ploidy; ++i) {
        genome[i].mutate();
    }
}

#endif /* Individual_hpp */