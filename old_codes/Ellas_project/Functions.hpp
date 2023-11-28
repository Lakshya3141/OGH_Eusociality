//
//  Functions.hpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 25/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#ifndef Functions_hpp
#define Functions_hpp

#include "Random.hpp"
#include <array>
#include <cmath>


// function that removes objects from a vector
template <typename T = double>
void remove_from_vec(std::vector<T>& v, const size_t idx) {
    if (auto back_idx = v.size()-1; idx != back_idx) v[idx] = std::move(v[back_idx]);
    v.pop_back();
}

// logistic function (default values are for logistically transforming the sex ratio genes)
double logistic(double x, double intercept = 0.0, double slope = 1.0) {
    double y = 1 / (1 + exp(intercept + slope*x));
    return y;
}
    
// stochastically rounds a double to a nearest integer
int stochastic_rounding(const double x) {
    double intpart;
    double fractpart = modf(x,&intpart);
    if(bernoulli(fractpart)) return static_cast<int>(x+1.0);
    return static_cast<int>(intpart);
}

#endif /* Functions_hpp */
