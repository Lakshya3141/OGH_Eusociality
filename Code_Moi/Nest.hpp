//
//  Nest.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Nest_hpp
#define Nest_hpp    

#include "Individual.hpp"
#include "Random.hpp"

class Nest {
public:
    Nest(Individual<2>& f);

    // temp vector for larvae as well

    // temp vector to hold male adults and then move them to population level, delete the one at nest level

    // feed function loops through female, choose random larvae to give food to
    std::vector<Individual<2> > females; // vector of females, [0] = breeder
    
};

Nest::Nest(Individual<2>& f) : females{f} {} // initialiser list for nests (initialised with one female)

#endif /* Nest_hpp */