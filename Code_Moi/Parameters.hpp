//
//  Parameters.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#ifndef Parameters_hpp
#define Parameters_hpp

// Below we will have some basic parameters to build the code
double dInitDispersal = 1.0; //Initial value of the dispersal gene
// Logistic expression looks like 1/(1+exp(a+b*x))
double dInitGrowthIntercept = 0.1; //Initial value of parameter a in logistic expression
double dInitGrowthSlope = 0.2; //Initial value of parameter b in logistic expression
double dMutRate = 1.0; //Mutation probability
double dMutBias = 0.0; //Mutation Bias
double dMutEffect = 0.02; //Mutation effect

// add survival probability and limit to foraging

#endif /* Parameters_hpp */