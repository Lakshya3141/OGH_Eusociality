//
//  splines.hpp
//  splines
//
//  Created by Jan Kreider on 27/12/2021.
//

#ifndef splines_hpp
#define splines_hpp

#include "Functions.hpp"
#include "Parameters.hpp"
#include <iostream>
#include <vector>
#include <Eigen/Dense>

double max0(double x) {
    if (x < 0) return 0;
    else return x;
}

// knot locations equally spaced within the range of body size
std::vector<double> calculate_knot_locations(int knots, double lowerLimit, double upperLimit) {
    double distance = (upperLimit - lowerLimit) / ((double)knots + 1.0);
    std::vector<double> knot_locations;
    for (int i = 1; i < (knots + 1); ++i) {
        knot_locations.push_back(lowerLimit + (i * distance));
    }
    return knot_locations;
}

std::vector<double> calculate_resolution(int resolution, double lowerLimit, double upperLimit) {
    double distance = (upperLimit - lowerLimit) / ((double)resolution - 1.0);
    std::vector<double> resolution_locations;
    for (int i = 0; i < resolution; ++i) {
        resolution_locations.push_back(lowerLimit + (i * distance));
    }
    return resolution_locations;
}

Eigen::MatrixXd make_spline2d(int knots, int resolution, double lowerLimit, double upperLimit) {
    
    std::vector<double> knot_locations = calculate_knot_locations(knots, lowerLimit, upperLimit);
    std::vector<double> resolution_locations = calculate_resolution(resolution, lowerLimit, upperLimit);
    
    Eigen::MatrixXd basis_matrix;
    basis_matrix.resize(resolution, knots);
    for (int i = 0; i < resolution; ++i) {
        basis_matrix(i, 0) = 1;
        basis_matrix(i, 1) = resolution_locations[i];
        double t1 = max0(pow((resolution_locations[i] - knot_locations[knots - 2]), 3));
        double t2 = knot_locations[knots - 1] - knot_locations[knots - 2];
        double t3 = max0(pow((resolution_locations[i] - knot_locations[knots - 1]), 3));
        double t4 = pow((knot_locations[knots - 1] - knot_locations[0]), 2);
        for (int j = 0; j < (knots - 2); ++j) {
            basis_matrix(i, j + 2) = (max0(pow((resolution_locations[i] - knot_locations[j]), 3)) - (t1 * (knot_locations[knots - 1] - knot_locations[j]) / t2) + (t3 * (knot_locations[knots - 2] - knot_locations[j]) / t2)) / t4;
        }
    }
    return basis_matrix;
}

size_t find_row2d(Eigen::MatrixXd basis_matrix, double x) {
    size_t i = 0;
    while (basis_matrix(i, 1) < x) { ++i; }
    if (basis_matrix(i, 1) == x) return i;
    double probability_upper = (x - basis_matrix(i - 1, 1)) / ((x - basis_matrix(i - 1, 1)) + (basis_matrix(i, 1) - x));
    if (bernoulli(probability_upper)) return i;
    return i - 1;
}

double evaluate_spline2d(Eigen::MatrixXd basis_matrix, Eigen::MatrixXd betas, double x) {
    
    size_t row_index = find_row2d(basis_matrix, x);
    Eigen::MatrixXd row_values = basis_matrix.row(row_index);
    Eigen::MatrixXd phenotypic_value = row_values * betas;
    
    if (phenotypic_value.rows() > 1 or phenotypic_value.cols() > 1) {
        std::cout << "ERROR: row of basis_matrix * beta did not reduce to a 1 x 1 matrix" << std::endl;
    }
    assert(phenotypic_value.rows() == 1);
    assert(phenotypic_value.cols() == 1);
    
    double output = phenotypic_value(0, 0);
    return output;
}

/*
Eigen::MatrixXd tensor_product(Eigen::MatrixXd M1, Eigen::MatrixXd M2) {
    Eigen::MatrixXd T;
    T.resize(M1.rows() * M2.rows(), M1.cols() * M2.cols());
    for (int i = 0; i < M1.rows(); ++i) {
        for (int j = 0; j < M1.cols(); ++j) {
            for (int k = 0; k < M2.rows(); ++k) {
                for (int l = 0; l < M2.cols(); ++l) {
                    T(i * M2.rows() + k, j * M2.cols() + l) = M1(i, j) * M2(k, l);
                }
            }
        }
    }
    return T;
}

Eigen::MatrixXd make_spline3d(int knots, int resolution, double lowerLimit1, double upperLimit1, double lowerLimit2, double upperLimit2) {
    
    Eigen::MatrixXd basis_matrix1 = make_spline2d(knots, resolution, lowerLimit1, upperLimit1);
    Eigen::MatrixXd basis_matrix2 = make_spline2d(knots, resolution, lowerLimit2, upperLimit2);
    
    Eigen::MatrixXd basis_matrix;
    basis_matrix.resize(resolution * resolution,  knots * knots);
                                            
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            basis_matrix.row(i * resolution + j) << tensor_product(basis_matrix1.row(i), basis_matrix2.row(j));
        }
    }
    return basis_matrix;
}

size_t find_row3d(Eigen::MatrixXd basis_matrix, double x, double y) {
    size_t i = 0;
    int knots = sqrt(basis_matrix.cols());
    int resolution = sqrt(basis_matrix.rows());
    while (basis_matrix(i, knots) < x) { ++i; }
    while (basis_matrix(i, 1) < y) { ++i; }
    if (x == 0.0) i += resolution;
    if (y == 0.0) ++i;
    double probability_upperX = (x - basis_matrix(i - resolution, knots)) / ((x - basis_matrix(i - resolution, knots)) + (basis_matrix(i, knots) - x));
    double probability_upperY = (y - basis_matrix(i - 1, 1)) / ((y - basis_matrix(i - 1, 1)) + (basis_matrix(i, 1) - y));
    std::vector<double> probabilities = {
        (1 - probability_upperX) * (1 - probability_upperY),
        (1 - probability_upperX) * probability_upperY,
        probability_upperX * (1 - probability_upperY),
        probability_upperX * probability_upperY
    };
    int index = std::discrete_distribution<int>(probabilities.begin(), probabilities.end())(rn);
    if (index == 0) return i - 1 - resolution;
    else if (index == 1) return i - resolution;
    else if (index == 2) return i - 1;
    else return i;
}

Eigen::MatrixXd evaluate_spline3d(Eigen::MatrixXd basis_matrix, Eigen::MatrixXd betas, double x, double y) {
    size_t row_index = find_row3d(basis_matrix, x, y);
    Eigen::MatrixXd row_values = basis_matrix.row(row_index);
    Eigen::MatrixXd phenotypic_value = row_values * betas;
    
    if (phenotypic_value.rows() > 1 or phenotypic_value.cols() > 1) {
        std::cout << "ERROR: row of basis_matrix * beta did not reduce to a 1 x 1 matrix" << std::endl;
    }
    assert(phenotypic_value.rows() == 1);
    assert(phenotypic_value.cols() == 1);
    
    return phenotypic_value;
}
*/
 
#endif /* splines_hpp */
