#include <iostream>
#include "Individual.hpp"
#include "Parameters.hpp"

// Function to print details about an individual
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
    std::cout << "t_death: " << individual.t_next << std::endl;
    // std::cout << "Number of Offspring: " << individual.num_offspring << std::endl;
    // std::cout << "Number of Female Offspring: " << individual.num_fem_offspring << std::endl;
    // std::cout << "Number of Larvae: " << individual.num_larva << std::endl;
    // std::cout << "Number of Female Larvae: " << individual.num_female_larva << std::endl;
    std::cout << std::endl;
}

int main() {
    // Create a female Individual
    Individual<2> female;
    std::cout << "Female Individual Details:" << std::endl;
    // printIndividualDetails(female);

    // Mutate and print details
    female.mutate();
    female.calculate_phenotype();
    std::cout << "\nAfter Mutation and Phenotype calculation:" << std::endl;
    printIndividualDetails(female);

    // Create a male Individual
    Individual<1> male;
    std::cout << "\nMale Individual Details:" << std::endl;
    // printIndividualDetails(male);

    // Mutate the male sperm before mating
    male.mutate();
    std::cout << "\nAfter Male Mutation:" << std::endl;
    printIndividualDetails(male);

    // Mate the female with the male
    female.mate(male);
    std::cout << "\nAfter Mating:" << std::endl;
    // printIndividualDetails(female);


    // Check for disperser status
    if (female.check_disperser()) {
        std::cout << "\nThe female is a disperser!" << std::endl;
    } else {
        std::cout << "\nThe female is not a disperser." << std::endl;
    }

    // Test survival
    int count = 0;
    while(female.is_alive){ 
        female.survival();
        count++;
    }
    
    std::cout << "\nAfter " << count << " Survival Checks:" << std::endl;
    printIndividualDetails(female);

    std::cout << "\n\n AIndID " << IndID << "\n" << std::endl;
    
    // Check for maturity
    double birth_time = gtime;
    Individual<2> daughter(female);
    std::cout << "\n\n CIndID " << IndID << "\n" << std::endl;
    std::cout << "\nDaughter produced:" << std::endl;
    printIndividualDetails(daughter);
    std::cout << "\n\n DIndID " << IndID << "\n" << std::endl;
    Individual<1> son(female);
    std::cout << "\n\n EIndID " << IndID << "\n" << std::endl;
    Individual<2> dummy_female;
    std::cout << "\n\n DumFemaleIndID " << IndID << "\n" << std::endl;
    Individual<1> dummy_male;
    std::cout << "\n\n DumMaleEIndID " << IndID << "\n" << std::endl;
    
    std::cout << "\nSon produced:" << std::endl;
    printIndividualDetails(son);
    daughter.body_size = 4.0;
    son.body_size = 2.0;
    daughter.check_mature(birth_time);
    son.check_mature(birth_time);
    std::cout << "\nAfter Checking Maturity Daughter:" << std::endl;
    printIndividualDetails(daughter);
    std::cout << "\nAfter Checking Maturity Son:" << std::endl;
    printIndividualDetails(son);



    return 0;
}
