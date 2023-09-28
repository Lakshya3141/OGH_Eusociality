//
//  main.cpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#include "individual.hpp"
#include <iostream>
#include "Random.hpp"

void haplo_check();

template<typename T, std::size_t N>
std::array<T, N> array_sum(const std::array<T, N>& a, const std::array<T, N>& b);

template<int Ploidy>
void print_individual(const Individual<Ploidy>& tmp);

int main() {
    Individual<2> mom;
    Individual<1> dad;
    Individual tmp = dad;
    print_individual(tmp);
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
    std::cout << tmp.body_size << tmp.disperser << tmp.is_alive << tmp.is_larvae << "\n";
    for (int i = 0; i < tmp.genome.size() ; ++i){
        std::cout << tmp.genome[i].genes_dispersal << tmp.genome[i].genes_choice[0] << tmp.genome[i].genes_choice[1] << "\n";
    }
}

//end