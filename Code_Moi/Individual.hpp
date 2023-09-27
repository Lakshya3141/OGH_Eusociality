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
    void survival(const double surv_prob);
    void forage(const double limit);
    bool disperser = false;
    bool helper = false;
    
    double body_size;      // for now, body size reflected by foraging
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
    bool chosen_hpl = bernoulli(); 
    genome[1].genes_dispersal = genome_mum[chosen_hpl].genes_dispersal;
    genome[1].genes_growth = genome_mum[chosen_hpl].genes_growth;
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

// determine female phenotype from taking average of the haplotypes
template<>
void Individual<2>::calculate_phenotype() {
    
    phenotype_dispersal = (genome[0].genes_dispersal + genome[1].genes_dispersal) / 2;
    phenotype_growth[0] = (genome[0].genes_growth[0] + genome[1].genes_growth[0]) / 2;
    phenotype_growth[1] = (genome[0].genes_growth[1] + genome[1].genes_growth[1]) / 2;
}

// survival check
template <int Ploidy>
void Individual<Ploidy>::survival(const double surv_prob){
    if bernoulli(surv_prob) is_alive = true;
    else is_alive = false;
    if(body_size <= 0.0) is_alive = false;
}

// foraging function
template <int Ploidy>
void Individual<Ploidy>::forage(const double limit = 1.0){
    food = uni_real(0.0, limit); //trial for now, doesn't matter
    body_size += food*0.1;
}

#endif /* Individual_hpp */