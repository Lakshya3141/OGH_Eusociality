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

using diploid_genome = std::array<Haplotype, 2>;
using haploid_genome = std::array<Haplotype, 1>;

template <int Ploidy>
class Individual {
public:

    // Individual constructors
    // male and female default + initialised constructors overloaded
    Individual(const int id);
    Individual(const int id, const Individual<2>& mum, const params& p);   
    
    Haplotype sperm;
    std::array<Haplotype, Ploidy> genome;           // genome of individual with 1 or 2 haplotypes
    
    bool is_alive = true;
    bool is_larva = false;
    double t_death = -1.0;
    double t_next;                     
    double t_birth = 0.0;
    int ind_id;
    int mom_id = -1;            // Mother ID
    int dad_id = -1;            // Father ID
    unsigned int nest_id;       // Nest identifier
    int mate_id = -1;           // Mate ID
    
    double phenotype_dispersal;
    std::array<double, 2> phenotype_choice;

    bool is_disperser = false;
    bool is_foraging = false;   // If not foraging, then brooding
    bool is_mated = false;
    
    double body_size = 0.0;     // body size, used for larvae
    int num_larva = 0; 
    int num_female_larva = 0;   // LCIP This is to have the total number of males and females by each individual

    // Individual funcions
    void mate(const Individual<1>& male); 
    void mutate(const params& p);
    void calculate_phenotype();
    void survival(const params& p);
    bool check_mature(double& birth_time, const params& p);
    bool check_disperser();
};


// calculate phenotype by taking average across haplotypes
template<>
void Individual<2>::calculate_phenotype() {
    phenotype_dispersal = (genome[0].genes_dispersal + genome[1].genes_dispersal) * 0.5;
    for(int i = 0; i < 2; ++i) {
        phenotype_choice[i] = (genome[0].genes_choice[i] + genome[1].genes_choice[i]) * 0.5;
    }
}

// default constructor male
template <>
Individual<1>::Individual (const int id) : ind_id(id), t_death(0.0) { }

// default constructor female
template <>
Individual<2>::Individual (const int id) : ind_id(id) {
    calculate_phenotype();
}

// constructor for males (haploid)
template <>
Individual<1>::Individual (const int id, const Individual<2>& mum, const params& p) : is_larva(true), ind_id(id), mom_id(mum.ind_id) {
    // son inherits genes from mum (with 50:50 chance of each haplotype)
    genome[0].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;
    genome[0].genes_choice = mum.genome[bernoulli()].genes_choice;
    mutate(p);          // individual is created then mutates
}

// constructor for females (diploid)
template <>
Individual<2>::Individual (const int id, const Individual<2>& mum, const params& p) : nest_id(mum.nest_id), is_larva(true), ind_id(id), mom_id(mum.ind_id), dad_id(mum.mate_id) {
    // daughter inherits one haplotype from sperm stored with mum
    // LCIP: first check if mom actally has sperm? We check if the female is mated when
    // they reproduce, and if not they only produce males
    genome[0] = mum.sperm;
    genome[1].genes_dispersal = mum.genome[bernoulli()].genes_dispersal;

    // LCIP: Yes, we have full recombination for slope and choice
    for(int i = 0; i < 2; ++i) {
        genome[1].genes_choice[i] = mum.genome[bernoulli()].genes_choice[i];
    }
    mutate(p);          // individual is created then mutates
    calculate_phenotype();
}


// mate function, stores the male haploid genome as sperm
template <>
void Individual<2>::mate(const Individual<1>& male) {
    sperm = male.genome[0]; // females store sperm from mate
    // LCIP: What abour recombination? Oh yeah I just forgot to delete this comment
    mate_id = male.ind_id;
    is_mated = true;
}

// mutate function // works
template <int Ploidy>
void Individual<Ploidy>::mutate(const params& p) {
    for (int i=0; i<Ploidy; ++i) {
        genome[i].mutate(p);
    }
}

// survival check for females based on their current task
template <>
void Individual<2>::survival(const params& p) {
    // If foraging and survives XOR reproducing and survives
    // LCIP: why XOR? OR is faster because it doesn't evaluate the second expression if the first evaluates as TRUE
    // Valid point, XOR is correct technically but since both expressions evaluating to true isnt an
    // harmful condition, OR works better :)
    if ((is_foraging && !bernoulli(p.dSurvForage)) ^ (!is_foraging && !bernoulli(p.dSurvBrood))) {
        is_alive = false;
        t_death = gtime + p.dDeathTime;
        t_next = t_death;
    }
}

// function to check maturity of male larvae after being fed
// Inverse transform sampling for an exponential distribution
// to sample time of death
// https://en.wikipedia.org/wiki/Inverse_transform_sampling
// IP: there's a built-in std::exponential_distribution
template <> 
bool Individual<1>::check_mature(double& birth_time, const params& p) {
    if (bernoulli(logistic(body_size, p.dLarvaIntercept, p.dLarvaSlope))) {
        // LCIP: so the bigger the more likely to mature (seeing as slope=-5): Yep
        is_larva = false;
        t_birth = birth_time;
        // inverse transform sampling
        t_death = birth_time + ( - log(1 - uni_real()) / p.dMaleLambda );
        return true;
    }
    return false;
}

// LCIP: why is maturation "stochastic"? LCIMP
// Would it not be easier/faster/less noisy with a deterministic threshold size?
// -> Yep I think you are right, but maybe this is closer to a more realistic scenario
// and accounts for developmental noise? Although I am not sure how much that is relevant
// to observing division of tasks, we could change this?

// function to check maturity of female larvae after being fed
template <> 
bool Individual<2>::check_mature(double& birth_time, const params& p) {
    if (bernoulli(logistic(body_size, p.dLarvaIntercept, p.dLarvaSlope))) {
        is_larva = false;
        t_birth = birth_time;
        return true;
    }
    return false;
}

// function to check disperser status of mature larvae
template <>
bool Individual<2>::check_disperser(){
    if (bernoulli(logistic(phenotype_dispersal))) { 
        is_disperser = true;
        return true;
    }
    return false;
}


#endif /* Individual_hpp */
