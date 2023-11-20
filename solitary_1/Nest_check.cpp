#include <iostream>
#include "Parameters.hpp"
#include "Random.hpp"
#include "Haplotype.hpp"
#include "Individual.hpp"
#include "Nest.hpp"

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
    // std::cout << "t_death: " << individual.t_next << std::endl;
    // std::cout << "Number of Offspring: " << individual.num_offspring << std::endl;
    // std::cout << "Number of Female Offspring: " << individual.num_fem_offspring << std::endl;
    // std::cout << "Number of Larvae: " << individual.num_larva << std::endl;
    // std::cout << "Number of Female Larvae: " << individual.num_female_larva << std::endl;
    std::cout << std::endl;
}

int main() {
    // 1) Constructor: Create a Nest with one female and add 9 more females
    Individual<2> firstFemale;
    std::cout << firstFemale.ind_id << "\n\n" << std::endl;
    Nest myNest(firstFemale);
    // std::cout << IndID << "\n\n" << std::endl;
    // Create 9 more females, mutate them, and add to the adult_females vector
    for (int i = 0; i < 4; ++i) {
        Individual<2> female;
        female.mutate();
        myNest.adult_females.push_back(female);
    }

    // Print details of all females in the nest
    std::cout << "Initial Nest Details:" << std::endl;
    for (const auto& female : myNest.adult_females) {
        printIndividualDetails(female);
        // std::cout << female.ind_id << "\n";
    }
    std::cout << "\n" << firstFemale.ind_id << "\n";
    std::cout << "\n" << myNest.adult_females[0].ind_id << "\n";
    // printIndividualDetails(firstFemale);
    // printIndividualDetails(myNest.adult_females[0]);
    // // 2) Check_maturity: Test the check_maturity function
    // std::cout << "\nTesting check_maturity function:" << std::endl;

    // // Case 1: Female with body size 0 (should not mature)
    // Individual<2> immatureFemale;
    // immatureFemale.body_size = 0.0;
    // int result1 = myNest.check_maturity(0, 0.0);
    // std::cout << "Case 1 Result: " << result1 << std::endl;  // Expected: -1

    // // Case 2: Female with body size 10 (should mature)
    // Individual<2> matureFemale;
    // matureFemale.body_size = 10.0;
    // int result2 = myNest.check_maturity(0, 0.0);
    // std::cout << "Case 2 Result: " << result2 << std::endl;  // Expected: 1

    // // Case 3: Female with body size 10 and is_disperser set to true (should disperse)
    // Individual<2> disperserFemale;
    // disperserFemale.body_size = 10.0;
    // disperserFemale.is_disperser = true;
    // int result3 = myNest.check_maturity(0, 0.0);
    // std::cout << "Case 3 Result: " << result3 << std::endl;  // Expected: 1

    // // 3) feed: Test the feed function
    // std::cout << "\nTesting feed function:" << std::endl;

    // // Create instances of male and female larva vectors
    // myNest.larval_males.resize(5);
    // myNest.larval_females.resize(5);

    // // Call feed function and print the result
    // int feedResult = myNest.feed(5.0, 1.0);
    // std::cout << "Feed Result: " << feedResult << std::endl;

    // // Print the resources value of the chosen larva
    // if (feedResult < 0) {
    //     std::cout << "Fed Larva Resources: " << myNest.larval_males[-feedResult - 1].body_size << std::endl;
    // } else if (feedResult > 0) {
    //     std::cout << "Fed Larva Resources: " << myNest.larval_females[feedResult - 1].body_size << std::endl;
    // } else {
    //     std::cout << "No Larvae Fed." << std::endl;
    // }

    // // 4) Reproduce: Test the reproduce function
    // std::cout << "\nTesting reproduce function:" << std::endl;

    // // Mated Female
    // Individual<2> matedFemale;
    // matedFemale.is_mated = true;
    // myNest.reproduce(matedFemale);
    // std::cout << "Number of Larval Females: " << myNest.larval_females.size() << std::endl;  // Expected: 1

    // // Unmated Female
    // Individual<2> unmatedFemale;
    // unmatedFemale.is_mated = false;
    // myNest.reproduce(unmatedFemale);
    // std::cout << "Number of Larval Males: " << myNest.larval_males.size() << std::endl;  // Expected: 1

    // // 5) findFemaleIndexById: Test the findFemaleIndexById function
    // std::cout << "\nTesting findFemaleIndexById function:" << std::endl;

    // // Search for the index of the first female (should be present)
    // size_t index1 = myNest.findFemaleIndexById(firstFemale.ind_id);
    // std::cout << "Index of First Female: " << index1 << std::endl;  // Expected: 1

    // // Search for the index of an invalid female ID (should not be present)
    // size_t index2 = myNest.findFemaleIndexById(1000);  // Assuming 1000 is an invalid ID
    // std::cout << "Index of Invalid Female ID: " << index2 << std::endl;  // Expected: 0

    return 0;
}
