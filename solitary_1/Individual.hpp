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
using sperm_genomes = std::vector<haploid_genome>; // YEP IS UNUSED YET

enum class Task {
    foraging,
    brooding,
    death
};

template <int Ploidy>
class Individual {
public:
    bool is_alive = true;
    bool is_larvae = false;                         // needs to be before constructor definition

    // Individual constructors
    Individual();
    Individual(const Individual<2>& mum);   // male and female?
    // Individual(const Individual<2>& mum);   // female

    Haplotype sperm;
    std::array<Haplotype, Ploidy> genome;           // genome of individual with 1 or 2 haplotypes
    // Individual<2>* mother;
    void mate(const Individual<1>& male); // revisit this too
    void mutate();
    void calculate_phenotype();

    double t_death;
    double t_next = uni_real();                     // Impart an asynchronous start if need be
    double t_birth;
    int ind_id;
    unsigned long int mom_id;   // Mother ID
    unsigned long int dad_id;   // Father IDW
    unsigned int nest_id;       // Nest identifier
    
    double phenotype_dispersal;
    std::array<double, 2> phenotype_choice;
    
    void survival();
    bool check_mature(double& birth_time);
    bool check_disperser();
    // static unsigned long int generateUniqueID();

    bool is_disperser = false;
    bool is_foraging = false;      // If not foraging, then brooding
    bool is_mated = false;
    
    double body_size = 0.0;     // body size, primarily used for larvae
    int num_offspring = 0;      // number of offspring a foundress will produce
    int num_fem_offspring = 0;  // number of female offspring a foundress will produce
    int num_larva = 0;
    int num_female_larva = 0;
    static int IndID;   // Declaring static member variable
};

// template <int Ploidy>
// static unsigned long int Individual<Ploidy>::generateUniqueID() {
//     static unsigned long int idCounter = 1;  // Starts from 1, adjust as needed
//     return idCounter++;
// }

// calculate phenotype // works
template<>
void Individual<2>::calculate_phenotype() {
    phenotype_dispersal = (genome[0].genes_dispersal + genome[1].genes_dispersal) * 0.5;
    for(int i = 0; i < 2; ++i) {
        phenotype_choice[i] = (genome[0].genes_choice[i] + genome[1].genes_choice[i]) * 0.5;
    }
}

template <int Ploidy>
int Individual<Ploidy>::IndID = 0;

// default constructor
template <int Ploidy>
Individual<Ploidy>::Individual() {
    ind_id = ++IndID;
}

// constructor for males (haploid)
template <>
Individual<1>::Individual (const Individual<2>& mum) {
    // son inherits genes from mum (with 50:50 chance of each haplotype)
    genome[0].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;
    genome[0].genes_choice = mum.genome[bernoulli()].genes_choice;
    is_larvae = true;
    mutate(); // individual is created then mutates
    ind_id = ++IndID;
    mom_id = mum.ind_id;
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const Individual<2>& mum) {
    // daughter inherits one haplotype from sperm stored with mum
    genome[0] = mum.sperm;
    //genome[0].genes_dispersal = genome_dad[0].genes_dispersal;
    //genome[0].genes_choice = genome_dad[0].genes_choice;
    
    genome[1].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;
    for(int i = 0; i < 2; ++i) {
        genome[1].genes_choice[i] = mum.genome[bernoulli()].genes_choice[i];
    }
    
    nest_id = mum.nest_id;
    is_larvae = true;
    mutate();
    calculate_phenotype();
    ind_id = ++IndID;
    mom_id = mum.ind_id;
}

// mate function // works
template <>
void Individual<2>::mate(const Individual<1>& male) {
    sperm = male.genome[0]; // females store sperm from mate
    genome[0] = sperm;
    is_mated = true;
}

// mutate function // works
template <int Ploidy>
void Individual<Ploidy>::mutate() {
    for (int i=0; i<Ploidy; ++i) {
        genome[i].mutate();
    }
}

// survival check // works
template <>
void Individual<2>::survival(){
    if ((is_foraging && !bernoulli(dSurvForage)) ^ (!is_foraging && !bernoulli(dSurvBrood))) {
        is_alive = false;
        // t_next = t_next - (is_foraging ? dForagingTime : dBroodingTime) + dDeathTime;
        t_death = gtime + dDeathTime;
        t_next = t_death;
    }
}

template <> 
bool Individual<1>::check_mature(double& birth_time){
    if (bernoulli(logistic(body_size, dLarvaIntercept, dLarvaSlope))) {
        is_larvae = false;
        t_birth = birth_time;
        t_death = birth_time + ( - log(1 - uni_real()) / dMaleLambda );
        return true;
    }
    else return false;
}

// LC: Inverse transform sampling
// https://en.wikipedia.org/wiki/Inverse_transform_sampling
template <> 
bool Individual<2>::check_mature(double& birth_time){
    if (bernoulli(logistic(body_size, dLarvaIntercept, dLarvaSlope))) {
        is_larvae = false;
        t_birth = birth_time;
        return true;
    }
    else return false;
}

template <>
bool Individual<2>::check_disperser(){
    if (bernoulli(phenotype_dispersal)) {
        is_disperser = true;
        return true;
    }
    else return false;
}

#endif /* Individual_hpp */
