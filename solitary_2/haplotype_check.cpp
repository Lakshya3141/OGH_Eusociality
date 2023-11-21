#include <iostream>
#include "Parameters.hpp"
#include "Random.hpp"
#include "Haplotype.hpp"

// Function to print details about each Haplotype instance
void printHaplotypeDetails(const Haplotype& haplotype) {
    std::cout << "Genes Dispersal: " << haplotype.genes_dispersal << std::endl;
    std::cout << "Genes Choice Intercept: " << haplotype.genes_choice[0] << std::endl;
    std::cout << "Genes Choice Slope: " << haplotype.genes_choice[1] << std::endl;
}

int main() {
    // Create an instance of Haplotype
    Haplotype haplotype;

    // Print initial details
    std::cout << "Initial Haplotype Details:" << std::endl;
    printHaplotypeDetails(haplotype);

    // Mutate and print details
    haplotype.mutate();
    std::cout << "\nAfter Mutation:" << std::endl;
    printHaplotypeDetails(haplotype);

    // Add more steps or operations as needed for testing

    return 0;
}
