#include "Population.hpp"  // Assuming your Population.hpp file includes the necessary headers

int main() {
    // Initialize a Population
    Population population;
    population.initialise_pop();  // Assuming this sets up nests and adds females to adult_females vector

    // Assuming you have some logic to add females to the nests, let's add a few females
    // For demonstration purposes, let's assume there are two nests, and we add one female to each
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            Individual<2> female(++IndID);  // Assuming IndID is a global variable or declared somewhere
            population.nests[i].adult_females.push_back(female);
        }
    }

    // Display the state of the population before removal
    std::cout << "Before removal:\n";
    for (size_t i = 0; i < population.nests.size(); ++i) {
        std::cout << "Nest " << i << " females: ";
        for (const auto& female : population.nests[i].adult_females) {
            std::cout << female.ind_id << " ";
        }
        std::cout << "\n";
    }

    // Let's say we want to remove the first female from the first nest (assuming it exists)
    size_t nestIndexToRemoveFrom = 0;
    size_t femaleIndexToRemove = 0;

    // Check if the nest and female exist before removal
    if (nestIndexToRemoveFrom < population.nests.size() &&
        femaleIndexToRemove < population.nests[nestIndexToRemoveFrom].adult_females.size()) {
        
        // Remove the female
        remove_from_vec(population.nests[nestIndexToRemoveFrom].adult_females, femaleIndexToRemove);
        
        // Display the state of the population after removal
        std::cout << "\nAfter removal:\n";
        for (size_t i = 0; i < population.nests.size(); ++i) {
            std::cout << "Nest " << i << " females: ";
            for (const auto& female : population.nests[i].adult_females) {
                std::cout << female.ind_id << " ";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "Invalid nest index or female index.\n";
    }

    return 0;
}
