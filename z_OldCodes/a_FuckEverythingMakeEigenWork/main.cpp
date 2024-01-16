#include <iostream>
#include <vector>
#include "Random.hpp"
#include "Splines.hpp"

int main() {
    // Test splines
    std::cout << "Spline Tests:" << std::endl;
    
    int knots = 5;
    int resolution = 100;
    double lowerLimit = 0.0;
    double upperLimit = 1.0;

    // 2D Spline
    Eigen::MatrixXd spline2d = make_spline2d(knots, resolution, lowerLimit, upperLimit);
    
    // Print the first few rows of the spline matrix
    std::cout << "2D Spline Matrix:" << std::endl;
    std::cout << spline2d.block(0, 0, 5, spline2d.cols()) << std::endl;

    // Test evaluate_spline2d
    double x2d = 0.5;
    double y2d = 0.3;
    double splineValue2d = evaluate_spline2d(spline2d, Eigen::MatrixXd::Random(knots, 1), resolution, x2d);
    std::cout << "2D Spline Value at (" << x2d << ", " << y2d << "): " << splineValue2d << std::endl;

    // 3D Spline
    double lowerLimit2 = 0.0;
    double upperLimit2 = 1.0;
    Eigen::MatrixXd spline3d = make_spline3d(knots, resolution, lowerLimit, upperLimit, lowerLimit2, upperLimit2);

    // Print the first few rows of the 3D spline matrix
    std::cout << "3D Spline Matrix:" << std::endl;
    std::cout << spline3d.block(0, 0, 5, spline3d.cols()) << std::endl;

    // Test evaluate_spline3d
    double x3d = 0.7;
    double y3d = 0.4;
    double splineValue3d = evaluate_spline3d(spline3d, Eigen::MatrixXd::Random(knots * knots, 1), knots, resolution, x3d, y3d);
    std::cout << "3D Spline Value at (" << x3d << ", " << y3d << "): " << splineValue3d << std::endl;

    return 0;
}
