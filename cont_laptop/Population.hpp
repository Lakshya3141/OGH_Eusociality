//
//  Population.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Population_hpp
#define Population_hpp    

#include "Nest.hpp"
#include "Random.hpp"

class Population {
public:
    Population() = default;

     std::vector<Individual<1> > adult_males;       // vector for males to fertilize female
     std::vector<Nest> nests;      

     void initialise_pop();                 
private:
};

// initialise population function
void Population::initialise_pop() {
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    Individual<2> eve;
    eve.calculate_phenotype();
    Individual<1> adam;
    
    eve.mate(adam); // female mated with adam
    eve.calculate_phenotype();
    for(int i=0; i < max_nests; ++i) {
        eve.nest_id = i;
        nests.emplace_back(eve);  // loops through each nest and adds mated founding female
    }
}

#endif /* Population_hpp */