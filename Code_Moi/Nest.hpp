//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 28/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//  Large code snippets borrowed from Ella Rees-Baylis

#ifndef Nest_hpp
#define Nest_hpp

#include "Individual.hpp"
#include "Random.hpp"

template <int Ploidy>
class Individual {
public:
    // Individual constructors
    Individual() = default;
    Individual(const diploid_genome& genome_mum); // male
    Individual(const diploid_genome& genome_mum, const haploid_genome& genome_dad); // female

    Haplotype sperm;
    std::array<Haplotype, Ploidy> genome; // genome of individual with 1 or 2 haplotypes
     // sperm vector to allow for multiple matings
    // Revisit this in case we might not need multiple matings, for now keep it simple
    // and carry only 1 haploid genome and not a vector
    // sperm_genomes sperm;
    void mate(const Haplotype& sg); // revisit this too
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


#endif /* Nest_hpp */