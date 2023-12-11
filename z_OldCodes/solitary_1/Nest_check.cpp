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
    Individual<2> firstFemale(++IndID);
    
    firstFemale.genome[0].genes_choice[0] = 2.0;
    firstFemale.genome[0].genes_choice[1] = 3.0;
    firstFemale.calculate_phenotype();
    Nest myNest(1, firstFemale);
    Individual<1> male(++IndID);
    // printIndividualDetails(firstFemale);
    male.genome[0].genes_choice[0] = 4.0; // Mutate the male sperm before mating
    printIndividualDetails(male);

    // Create 4 more females, mutate them, and add to the adult_females vector
    for (int i = 0; i < 4; ++i) {
        Individual<2> female(++IndID);
        female.nest_id = myNest.nest_id;
        female.mutate();
        female.calculate_phenotype();
        myNest.adult_females.push_back(female);
    }

    // Print details of all females in the nest
    std::cout << "Initial Nest Details:" << std::endl;
    for (auto& female : myNest.adult_females) {
        female.mate(male); // To confirm if individuals mated
        printIndividualDetails(female);
    }
    print();
    printIndividualDetails(myNest.adult_females[0]);

    std::cout << "\nTesting findFemale function:" << std::endl;

    std::cout << myNest.findFemaleIndexById(1) - 1 << std::endl;
    std::cout << myNest.findFemaleIndexById(3) - 1<< std::endl;
    std::cout << myNest.findFemaleIndexById(8) - 1 << std::endl;
    std::cout << myNest.findFemaleIndexById(6) - 1<< std::endl;

    // 4) Reproduce: Test the reproduce function
    std::cout << "\nTesting reproduce function:" << std::endl;

    // Mated Female
    Individual<2> unmatedFemale(++IndID);
    
    for(int i = 0; i < 5; i++){
        // myNest.reproduce(unmatedFemale);
        // std::cout << "#LMales: " << myNest.larval_males.size() << std::endl;  // Expected: 1
    }
    std::cout << "#LFemales: " << myNest.larval_females.size() << "  |  #LMales: " << myNest.larval_males.size()<< std::endl;  // Expected: 1

    for(int i = 0; i < 10; i++){
        myNest.reproduce(myNest.adult_females[0]);
        // std::cout << "#LMales: " << myNest.larval_males.size() << std::endl;  // Expected: 1
    }
    std::cout << "#LFemales: " << myNest.larval_females.size() << "  |  #LMales: " << myNest.larval_males.size()<< std::endl;  // Expected: 1

    //std::cout << "LMales after 5 unmated reproduction: \n" << std::endl;
    for (auto& male : myNest.larval_males) {
        // printIndividualDetails(male);
    }


    for (auto& male : myNest.larval_females) {
        // printIndividualDetails(male);
    }

    // 3) feed: Test the feed function
    std::cout << "\nTesting feed function:" << std::endl;

    // Call feed function and print the result

    // myNest.emptyAdultFemales();
    myNest.larval_females.clear();
    myNest.larval_males.clear();

    for (int i = 0; i < 10; i++) {
        int feedResult = myNest.feed(5.0, 1.0);
        int index = std::abs(feedResult) - 1;
        if (feedResult > 0) {
            std::cout << "FEMALE: " << index << " |Body Size: " << myNest.larval_females[index].body_size << "\n" << std::endl;
        }
        else if (feedResult < 0) {
            std::cout << "MALE: "<< index << " |Body Size: " << myNest.larval_males[index].body_size << "\n" << std::endl;
        } else std::cout << "No feed" << std::endl;
        
    }


    return 0;
}
