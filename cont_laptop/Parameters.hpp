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
double dInitDispersal = 1.0;        // Initial value of the dispersal gene
// Logistic expression looks like 1/(1+exp(a+b*x))
double dLarvaIntercept = 0.1;       // value of parameter a in larval hatching logistic expression
double dLarvaSlope = 0.2;           // value of parameter a in larval hatching logistic expression
double dInitChoiceIntercept = 0.0;  // Initial value of parameter a in task choice logistic expression
double dInitChoiceSlope = 0.0;      // Initial value of parameter b in task choice logistic expression

double dForagingTime = 3.0;         // Time it takes for foraging
double dBroodingTime = 4.0;         // Time it takes for brooding
double dLambdaForaging = 1.0;       // Poisson's constant for foraging
double dLambdaBrooding = 1.0;       // Poisson's constant for brooding 

double dMutRate = 1.0;              // Mutation probability
double dMutBias = 0.0;              // Mutation Bias
double dMutEffect = 0.02;           // Mutation effect
double dSurvProb = 0.9;             // Survival probability at each time step for all bees
double dForagingMean = 1.0;         // Mean of normal foraging curve
double dForagingSD = 0.3;           // SD of normal foraging curve
double dMaturingSize = 1.0;         // Maturing size of larvae

unsigned int max_nests = 1000;      // Maximum number of nests
double const_sex_ratio = 0.1;       // male sex ratio

#endif /* Parameters_hpp */