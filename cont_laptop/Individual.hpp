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
using sperm_genomes = std::vector<haploid_genome>;

template <int Ploidy>
class Individual {
public:
    bool is_alive = true;                           //
    bool is_larvae = false;                         // needs to be before constructor definition
    // Individual constructors
    Individual() = default;
    Individual(const Individual<2>& mum);   // male and female?
    // Individual(const Individual<2>& mum);   // female

    Haplotype sperm;
    std::array<Haplotype, Ploidy> genome;           // genome of individual with 1 or 2 haplotypes
    void mate(const Individual<1>& male); // revisit this too
    void mutate();
    void calculate_phenotype();

    double t_prev = 0.0;
    double t_next = uni_real();                     // Impart an asynchronous start if need be
    double t_birth;
    unsigned long int ind_id;   // Inidividual identifier
    unsigned int nest_id;       // Nest identifier
    
    double phenotype_dispersal;
    std::array<double, 2> phenotype_choice;
    
    void survival(const double& survival_prob = dSurvProb);
    void forage(const double& mean = dForagingMean, const double& SD = dForagingSD);
    void feed(const double& food);
    bool check_mature(double& birth_time);
    bool task_check();      // function to check and change task choice
    bool is_disperser = false;
    bool is_foraging = false;      // If not foraging, then brooding
    
    double body_size = 0.0;     // body size, primarily used for larvae
    double resources = 0.0;     // resources brought back in one foraging trip
    int num_offspring = 0;      // number of offspring a foundress will produce
    int num_fem_offspring = 0;  // number of female offspring a foundress will produce
    int num_larva = 0;
    int num_femle_larva = 0;
    
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
Individual<1>::Individual (const Individual<2>& mum) {
    // son inherits genes from mum (with 50:50 chance of each haplotype)
    genome[0].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;
    genome[0].genes_choice = mum.genome[bernoulli()].genes_choice;
    is_larvae = true;
    mutate(); // individual is created then mutates
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const Individual<2>& mum) {

    // daughter inherits one haplotype from sperm stored with mum
    genome[0] = mum.sperm;
    //genome[0].genes_dispersal = genome_dad[0].genes_dispersal;
    //genome[0].genes_choice = genome_dad[0].genes_choice;
    
    genome[1].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;
    genome[1].genes_choice[0] = mum.genome[bernoulli()].genes_choice[0];
    genome[1].genes_choice[1] = mum.genome[bernoulli()].genes_choice[1];

    nest_id = mum.nest_id;
    is_larvae = true;
    mutate();
    calculate_phenotype();
    if (bernoulli(logistic(phenotype_dispersal))){
        is_disperser = true;
    }
}

// mate function // works
template <>
void Individual<2>::mate(const Individual<1>& male) {
    sperm = male.genome[0]; // females store sperm from mate
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
}

// task check function // works // revisited
template <>
bool Individual<2>::task_check() {
    bool tmp1 = bernoulli(logistic(num_offspring, phenotype_choice[0], phenotype_choice[1]));
    bool tmp2 = is_foraging;
    is_foraging = tmp1;
    t_prev = t_next;
    t_next = t_next + is_foraging ? dForagingTime : dBroodingTime;
    return tmp2 ^ is_foraging;      // returns true if a switch happened
}

// function to check larval maturity // works
// chooses task the moment born
// template <int Ploidy>
// bool Individual<Ploidy>::check_mature(double& birth_time){
//     if (body_size >= dMaturingSize) {
//         is_larvae = false;
//         task_check();
//         return true;
//     }
// }

bool Individual<1>::check_mature(double& birth_time){
    if (body_size >= dMaturingSize) {
        is_larvae = false;
        t_birth = birth_time;
        return true;
    }
}

bool Individual<2>::check_mature(double& birth_time){
    if (body_size >= dMaturingSize) {
        is_larvae = false;
        task_check();
        t_birth = birth_time;
        return true;
    }
}



#endif /* Individual_hpp */