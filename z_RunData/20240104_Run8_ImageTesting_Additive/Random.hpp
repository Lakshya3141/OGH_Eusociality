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
#include <algorithm>
#include <tuple>

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

// exponential distribution
double exponential(double lambda) {
    return std::exponential_distribution<double>(lambda)(rn);
}

// logistic function
template <typename T>
double logistic(T x, double intercept = 0.0, double slope = 1.0) {
    return 1 / (1 + exp(intercept + static_cast<double>(x) * slope));
}

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

// Function to randomly select alpha number of individuals
template <typename T>
std::vector<T> randomSubset(const std::vector<T>& individuals, int alpha) {
    // Copy the vector to avoid modifying the original vector
    std::vector<T> result = individuals;

    if (alpha >= individuals.size()) {
        // Shuffle the entire vector if alpha is greater or equal to the vector size
        std::shuffle(result.begin(), result.end(), rn);
        return result;
    }

    // Shuffle the vector to randomize the selection
    std::shuffle(result.begin(), result.end(), rn);

    // Resize the vector to contain only alpha elements
    result.resize(alpha);

    return result;
}

// Function template to calculate the average of a vector of any numeric type
template <typename T>
T calculateAverage(const std::vector<T>& values) {
    if (values.empty()) {
        // Handle the case where the vector is empty to avoid division by zero
        std::cerr << "Error: Cannot calculate average of an empty vector." << std::endl;
        return T();
    }
    // Calculate the sum of all elements in the vector
    T sum = T();
    for (const auto& value : values) {
        sum += static_cast<double>(value); // Typecast value to double
    }
    // Calculate the average
    return sum / static_cast<double>(values.size());
}

// Function template to calculate the mean and standard deviation of a vector of any numeric type
template <typename T>
std::tuple<double, double> mean_std(const std::vector<T>& data) {
    double mean = calculateAverage(data);
    double std_dev = 0.0;
    for (const auto& value : data) {
        std_dev += std::pow(static_cast<double>(value) - mean, 2);
    }
    std_dev = std::sqrt(std_dev / static_cast<double>(data.size()));
    return std::make_tuple(mean, std_dev);
}


#endif /* Random_hpp */
