#include <iostream>
#include "Population.hpp"

#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream


int main() {
    Population myPop;

    std::cout << "\n\n 1) INITIALISING POPULATION:  \n" << std::endl;
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
        Individual<1> dumMale(myPop.individual_id_counter);
        ++myPop.individual_id_counter;
        dumMale.t_death = cnt;
        dumMale.genome[0].genes_choice[0] = cnt;
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
        printIndividualDetails(nest.adult_females[0]);
    }
    
    std::vector<Individual<1> > dumMalesVector;
    dumMalesVector = myPop.adult_males;
    myPop.adult_males.clear();
    Individual<2> dumFemale(myPop.individual_id_counter);
    ++myPop.individual_id_counter;
    // printIndividualInfo(dumFemale);
    myPop.adult_males = dumMalesVector;
    // printIndividualInfo(dumFemale);

    // 4) Checking update_emptynests
    std::cout << "\n4) EMPTY NEST  \n" << std::endl;
    Population dumPop = myPop;
    myPop.nests[1].adult_females.clear();
    myPop.nests[3].adult_females.clear();

    for(int i = 0; i < 10; i++) {
        // std::cout << "Empty nest given: " << myPop.update_emptyNests() - 1<< std::endl;
    }

    // 5) Checking population simluate
    std::cout << "4) SIMULATE  \n" << std::endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    myPop = dumPop;
    gtime = 0.010001;
    // myPop.simulate_tst();

    // 6) Checking new population initalise and simulate
    
    std::cout << "5) INITIALISE AND SIMULATE  \n" << std::endl;

    Population newPop;
    newPop.initialise_pop();
    newPop.simulate_tst();

    return 0;
}
