#include <iostream>
#include "Population.hpp"

template<int Ploidy>
void printIndividualDetails(const Individual<Ploidy>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << " | Nest ID: " << individual.nest_id << std::endl;
    std::cout << "Genome Values: ";
    for (int i = 0; i < Ploidy; ++i) {
        std::cout << "(" << individual.genome[i].genes_dispersal << ", " << individual.genome[i].genes_choice[0] << ", " << individual.genome[i].genes_choice[1] << ") ";
    }
    std::cout << std::endl;
    std::cout << "Phenotype Dispersal: " << individual.phenotype_dispersal << std::endl;
    std::cout << "Phenotype Choice Intercept: " << individual.phenotype_choice[0] << std::endl;
    std::cout << "Phenotype Choice Slope: " << individual.phenotype_choice[1] << std::endl;
    std::cout << "Is Alive: " << (individual.is_alive ? "true" : "false") << std::endl;
    std::cout << "Is Larvae: " << (individual.is_larvae ? "true" : "false") << std::endl;
    std::cout << "Is Foraging: " << (individual.is_foraging ? "true" : "false") << std::endl;
    std::cout << "Is Disperser: " << (individual.is_disperser ? "true" : "false") << std::endl;
    std::cout << "Is Mated: " << (individual.is_mated ? "true" : "false") << std::endl;
    std::cout << "Body Size: " << individual.body_size << std::endl;
    std::cout << "t_death: " << individual.t_death << std::endl;
    // std::cout << "Number of Offspring: " << individual.num_offspring << std::endl;
    // std::cout << "Number of Female Offspring: " << individual.num_fem_offspring << std::endl;
    // std::cout << "Number of Larvae: " << individual.num_larva << std::endl;
    // std::cout << "Number of Female Larvae: " << individual.num_female_larva << std::endl;
    std::cout << std::endl;
}

#include <iostream>
#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream

// Function to print individual information concisely
void printIndividualInfo(const Individual<2>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << "\n";
    std::cout << "Nest ID: " << individual.nest_id << "\n";
    std::cout << "Time of Death: " << std::fixed << std::setprecision(2) << individual.t_death << "\n";

    // Print genome values
    for (int i = 0; i < 2; ++i) {
        const auto& haplotype = individual.genome[i];
        std::cout << "Haplotype " << i + 1 << " Genome: (" << haplotype.genes_dispersal << ", "
                  << haplotype.genes_choice[0] << ", " << haplotype.genes_choice[1] << ")\n";
    }

    // Print sperm gene values if mated
    if (individual.is_mated) {
        const auto& sperm = individual.sperm;
        std::cout << "Sperm Genome: (" << sperm.genes_dispersal << ", " << sperm.genes_choice[0] << ", "
                  << sperm.genes_choice[1] << ")\n";
    }
    std::cout << "\n" << std::endl;
}

int main() {
    Population myPop;

    std::cout << "1) INITIALISING POPULATION:  \n" << std::endl;
    // 1) Checking intialise pop
    myPop.initialise_pop();
    // printIndividualDetails(myPop.adult_males[0]);
    for (auto nest: myPop.nests) {
        // printIndividualDetails(nest.adult_females[0]);
    }

    // 2) Checking remove dead males
    std::cout << "2) REMOVING DEAD MALES  \n" << std::endl;
    double cnt = 6.0;
    for(int i = 0; i < 5; i++) {
        Individual<1> dumMale(++IndID);
        dumMale.t_death = cnt;
        dumMale.genome[0].genes_dispersal = cnt;
        cnt++;
        myPop.adult_males.push_back(dumMale);
    }

    myPop.adult_males[1].t_death = 10;
    gtime = 8.0;

    for (auto male: myPop.adult_males) {
        // printIndividualDetails(male);
    }

    myPop.removeDeadMales();
    std::cout << "After removal  \n" << std::endl;

    for (auto male: myPop.adult_males) {
        printIndividualDetails(male);
    }

    // 3) Checking random male and mates
    std::cout << "3) MATING AND RANDOM MALE  \n" << std::endl;
    for(auto& nest: myPop.nests) { 
        myPop.mate(nest.adult_females[0]);
        printIndividualInfo(nest.adult_females[0]);
    }
    
    std::vector<Individual<1> > dumMalesVector;
    dumMalesVector = myPop.adult_males;
    myPop.adult_males.clear();
    Individual<2> dumFemale(++IndID);
    std::cout << "When no males, mate returns: " << myPop.mate(dumFemale) << std::endl;
    printIndividualInfo(dumFemale);
    myPop.adult_males = dumMalesVector;
    std::cout << "When males, mate returns: " << myPop.mate(dumFemale) << std::endl;
    printIndividualInfo(dumFemale);

    // 4) Checking update_emptynests
    std::cout << "4) EMTPY NEST  \n" << std::endl;

    return 0;
}
