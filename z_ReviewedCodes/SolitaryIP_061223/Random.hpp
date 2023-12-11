//
//  Parameters.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#ifndef Random_hpp
#define Random_hpp

#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>

unsigned int simulationID = static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()); // sample a seed
std::mt19937 rn(simulationID); // seed the random number generator

// bernoulli distribution (default p=0.5)
bool bernoulli(double p=0.5) { return std::bernoulli_distribution(p)(rn); }

// normal distribution
double normal(double mean, double sd) { return std::normal_distribution<double>(mean, sd)(rn); }

// uniform integer distribution
template<typename T1, typename T2>
T2 uni_int(T1 lower, T2 upper) { return std::uniform_int_distribution<T2>(lower, upper - 1)(rn); }

// uniform real distribution (default limits 0-1)
double uni_real(double lower = 0.0, double upper = 1.0) { return std::uniform_real_distribution<double>(lower, upper)(rn); }

// binomial distribution
int binom(int n, double p) { return std::binomial_distribution<int>(n,p)(rn); }

// logistic function
template <typename T>
double logistic(T x, double intercept = 0.0, double slope = 1.0) {
    return 1.0 / (1.0 + exp(intercept + static_cast<double>(x) * slope));
}
// IP: this could be confusing because a *positive* slope implies that
// the function *decreases* with x!


// function that removes objects from a vector
template <typename T = double>
void remove_from_vec(std::vector<T>& v, const size_t idx) {
    auto back_idx = v.size()-1;
    if (idx != back_idx) v[idx] = std::move(v[back_idx]);
    v.pop_back();
}

// dummy print function for debugging
void print(){
    std::cout << "DUMMY" << std::endl;
}

// dummy print function for debugging
void print_spec(const std::string& str){
    std::cout << str << std::endl;
}

// dummy print function for debugging
void print_spec(const char* str) {
    std::cout << str << std::endl;
}

// dummy print vector function for debugging
template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[ ";
    for (const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << "]\n";
}

#endif /* Random_hpp */
