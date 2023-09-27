//
//  main.cpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#include "Haplotype.hpp"
#include <iostream>

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

int main() {
    //haplo_check();
    std::array<double, 2> a {2, 3}, b {4, 5};
    double c[5];
    for (int i = 0; i < 5; i++) {  
        c[i] = a[i] + b[i];  
        std::cout << c[i] << " "; 
    }  

    return 0;
}