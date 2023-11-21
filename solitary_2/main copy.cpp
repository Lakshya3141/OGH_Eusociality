//
//  main.cpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#include "Population.hpp" //JK: changed this (previously included individual.hpp)
#include <iostream>
//#include "Random.hpp" //JK: can be deleted. already included

void haplo_check();
void indi_check();

template<typename T, std::size_t N> //JK: can go into a functions header, e.g. utils.hpp
std::array<T, N> array_sum(const std::array<T, N>& a, const std::array<T, N>& b);

template<int Ploidy>
void print_individual(const Individual<Ploidy>& tmp);

int main() {
    indi_check();
    for(int j = 0; j<5; ++j){
        int cnt = 0;
        for(int i = 0; i < 1000; ++i) {
            bool tmp = bernoulli(logistic(j, 0.0, 0.0));
            if (tmp) cnt++;
            //std::cout << tmp << "\n";
        }
        //std::cout << cnt << "\n";
    }
    return 0;
}

void haplo_check(){
    Haplotype hpl;
    std::cout << "Check print 1\n";
    std::cout << "g\ta\tb\n";
    std::cout << hpl.genes_dispersal << "\t";
    
    // Print the elements of the genes_choice array individually
    for (const double& gene : hpl.genes_choice) {
        std::cout << gene << "\t";
    }
    std::cout << "\n";
    for (int i = 0; i < 50; ++i){
        hpl.mutate();
        std::cout << hpl.genes_dispersal << "\t";
        for (const double& gene : hpl.genes_choice) {
            std::cout << gene << "\t";
    }
    std::cout << "\n";
    
    }
    std::cout << "\nCheck print 2.2\n";
}

void indi_check() {
    Individual<2> mom;
    Individual<1> dad;
    Individual<2> tmp = mom;
    
    tmp.mutate();
    dad.mutate();
    print_individual(dad);
    tmp.forage();
    tmp.feed(1.1);
    
    tmp.calculate_phenotype();
    tmp.mate(dad); //JK: changed this to remove error message
    print_individual(tmp);

    Individual<2> daughter(tmp); //JK: changed this to remove error message
    print_individual(daughter);

    daughter.feed(0.5);
    double time = 0.0; //JK: added "time" for mature function 
    daughter.check_mature(time); //JK: added function argument
    print_individual(daughter);
    daughter.feed(1);
    daughter.check_mature(time); //JK: added function argument
    print_individual(daughter);

    for(int i = 0; i < 10; ++i) {
        dad.survival(0.7);
        std::cout << " " << dad.is_alive;
    }
}

template<typename T, std::size_t N>
std::array<T, N> array_sum(const std::array<T, N>& a, const std::array<T, N>& b){
    std::array<T, N> sum;
    for (std::size_t i = 0; i < N; ++i) {
        sum[i] = a[i] + b[i];
    }
    return sum;
}

template<int Ploidy>
void print_individual(const Individual<Ploidy>& tmp){
    std::cout << "body size " << tmp.body_size << " reso " << tmp.resources 
        << " alive " << tmp.is_alive << " larvae " << tmp.is_larvae << "\n";
    for (int i = 0; i < tmp.genome.size() ; ++i){
        std::cout << "GENOME " << i << " \ndisp "<< tmp.genome[i].genes_dispersal 
        << " inter " << tmp.genome[i].genes_choice[0] << " slope " << tmp.genome[i].genes_choice[1] << "\n";
    }
    std::cout << "PHENOTYPE " << " \nPdisp "<< tmp.phenotype_dispersal
        << " Pinter " << tmp.phenotype_choice[0] << " Pslope " << tmp.phenotype_choice[1] << "\n";
    if (Ploidy == 2) {
        std::cout << "SPERM " << " \ndisp " << tmp.sperm.genes_dispersal  
        << " inter " << tmp.sperm.genes_choice[0] << " slope " << tmp.sperm.genes_choice[1] << "\n";
    }
    std::cout << "\n\n";
}

//end
