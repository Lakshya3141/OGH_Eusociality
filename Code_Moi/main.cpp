//
//  main.cpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#include "Haplotype.hpp"
#include <iostream>
#include "Random.hpp"

void haplo_check();
void individual_check();
template<typename T, std::size_t N>
std::array<T, N> array_sum(const std::array<T, N>& a, const std::array<T, N>& b);


int main() {
    std::array<double, 2> a {10, 11}, b {4, 5};
    std::array<double, 2> c;
    c = array_sum(a, b);
    for (int i = 0; i < 2; i++) {  
        std::cout << c[i] << " "; 
    }
    return 0;
}

void individual_check(){}

void haplo_check(){
    Haplotype hpl;
    std::cout << "Check print 1\n";
    std::cout << "g\ta\tb\n";
    std::cout << hpl.genes_dispersal << "\t";
    
    // Print the elements of the genes_growth array individually
    for (const double& gene : hpl.genes_growth) {
        std::cout << gene << "\t";
    }
    std::cout << "\n";
    for (int i = 0; i < 50; ++i){
        hpl.mutate();
        std::cout << hpl.genes_dispersal << "\t";
        for (const double& gene : hpl.genes_growth) {
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


//end