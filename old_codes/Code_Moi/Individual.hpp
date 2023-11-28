//
//  Individual.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 26/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//  Large code snippets borrowed from Ella Rees-Baylis

#ifndef Individual_hpp
#define Individual_hpp

#include "Haplotype.hpp"
#include "Random.hpp"
#include <iostream>

using diploid_genome = std::array<Haplotype, 2>;
using haploid_genome = std::array<Haplotype, 1>;
//using sperm_genomes = std::vector<haploid_genome>;

template <int Ploidy>
class Individual {
public:
    bool is_alive = true;                           //
    bool is_larvae = false;                         // needs to be before constructor definition
    bool is_fed = false;
    // Individual constructors
    Individual() = default;
    Individual(const diploid_genome& genome_mum);   // male
    Individual(const diploid_genome& genome_mum, const haploid_genome& genome_dad); // female

    Haplotype sperm;
    std::array<Haplotype, Ploidy> genome;           // genome of individual with 1 or 2 haplotypes
    // Revisit this in case we might not need multiple matings, for now keep it simple
    // and carry only 1 haploid genome and not a vector
    void mate(const Haplotype& sg); // revisit this too
    void mutate();
    void calculate_phenotype();
    
    double phenotype_dispersal;
    std::array<double, 2> phenotype_choice;
    
    void survival(const double& survival_prob = dSurvProb);
    void forage(const double& mean = dForagingMean, const double& SD = dForagingSD);
    void feed(const double& food);
    bool check_mature();
    bool task_check();      // function to check and change task choice
    bool disperser = false;
    bool is_foraging = false;      // If not foraging, then brooding
    
    double body_size = 0.0;     // body size, primarily used for larvae
    double resources = 0.0;     // resources brought back in one foraging trip
    int num_offspring = 0;      // number of offspring a foundress will produce
    int num_fem_offspring = 0;  // number of female offspring a foundress will produce
    unsigned long int ind_id;   // Inidividual identifier
    unsigned int nest_id;       // Nest identifier
};

// calculate phenotype // works
template<>
void Individual<2>::calculate_phenotype() {
    phenotype_dispersal = (genome[0].genes_dispersal + genome[1].genes_dispersal) * 0.5;
    phenotype_choice[0] = (genome[0].genes_choice[0] + genome[1].genes_choice[0]) * 0.5;
    phenotype_choice[1] = (genome[0].genes_choice[1] + genome[1].genes_choice[1]) * 0.5;
}

// constructor for males (haploid)
template <>
Individual<1>::Individual (const diploid_genome& genome_mum) {
    // son inherits genes from mum (with 50:50 chance of each haplotype)
    genome[0].genes_dispersal = genome_mum[bernoulli()].genes_dispersal;
    genome[0].genes_choice = genome_mum[bernoulli()].genes_choice;
    is_larvae = true;
    mutate(); // individual is created then mutates
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const diploid_genome& genome_mum, const haploid_genome& genome_dad) {

    // daughter inherits one haplotype from dad
    genome[0] = genome_dad[0];
    //genome[0].genes_dispersal = genome_dad[0].genes_dispersal;
    //genome[0].genes_choice = genome_dad[0].genes_choice;
    
    genome[1].genes_dispersal = genome_mum[bernoulli()].genes_dispersal;
    genome[1].genes_choice[0] = genome_mum[bernoulli()].genes_choice[0];
    genome[1].genes_choice[1] = genome_mum[bernoulli()].genes_choice[1];

    is_larvae = true;
    mutate();
    calculate_phenotype();
}

// mate function // works
template <>
void Individual<2>::mate(const Haplotype& sg) {
    sperm = sg; // females store sperm from mate
}

// mutate function // works
template <int Ploidy>
void Individual<Ploidy>::mutate() {
    for (int i=0; i<Ploidy; ++i) {
        genome[i].mutate();
    }
}

// survival check // works
// MIGHT NEED TO REVISIT WHEN YOU TALK ABOUT WETHER THIS APPLIES TO LARVAE OR NOT
template <int Ploidy>
void Individual<Ploidy>::survival(const double& survival_prob){
    //if (!is_larvae && !bernoulli(survival_prob)) {is_alive = false;}
    if (!bernoulli(survival_prob)) {is_alive = false;}

    // if(body_size <= 0.0) is_alive = false; // assuming they stay alive
}

// foraging function // works
template <int Ploidy>
void Individual<Ploidy>::forage(const double& mean, const double& SD){
    resources += normal(mean, SD); //trial for now, doesn't matter
}

// feeding function //works
template <int Ploidy>
void Individual<Ploidy>::feed(const double& food){
    body_size += food; // since food can be different from resources, needed 2 parameters
    is_fed = true;
}

// task check function // works
template <>
bool Individual<2>::task_check() {
    bool tmp1 = bernoulli(logistic(num_offspring, phenotype_choice[0], phenotype_choice[1]));
    bool tmp2 = is_foraging;
    is_foraging = tmp1;
    return tmp2 ^ is_foraging;      // returns true if a switch happened
}

// function to check larval maturity // works
// chooses task the moment born
template <int Ploidy>
bool Individual<Ploidy>::check_mature(){
    if (body_size >= dMaturingSize) {
        is_larvae = false;
        task_check();
        return true;
    }
}

#endif /* Individual_hpp */