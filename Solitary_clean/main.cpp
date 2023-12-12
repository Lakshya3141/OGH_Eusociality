#include <iostream>
#include "Population.hpp"

#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream


int main(int argc, char* argv[]) {
  try {
    std::string file_name = (argc > 2) ? argv[1] : "config.ini";
    std::cout << "Global Parameters:" << std::endl;
    std::cout << "max_gtime_evolution = " << max_gtime_evolution << std::endl;
    std::cout << "max_gtime_LastOfUs = " << max_gtime_LastOfUs << std::endl;
    std::cout << "dMaleRemovalTime = " << dMaleRemovalTime << std::endl;
    std::cout << "dOutputTimeInterval = " << dOutputTimeInterval << std::endl;
    std::cout << "max_nests = " << max_nests << std::endl;
    std::cout << "dInitDispersal = " << dInitDispersal << std::endl;
    std::cout << "dInitChoiceIntercept = " << dInitChoiceIntercept << std::endl;
    std::cout << "dInitChoiceSlope = " << dInitChoiceSlope << std::endl;

    std::cout << "\nReading from config file: " << file_name << "\n";
    std::ifstream test_file(file_name.c_str());
    if (!test_file.is_open()) {
      throw std::runtime_error("can't find config file");
    }
    test_file.close();

    params sim_par_in(file_name);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    Population myPop(sim_par_in);
    myPop.initialise_pop();

    std::string output_pop = "output_evolution.csv";
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    myPop.simulate_tst(output_pop);

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    std::cout << std::endl;
    std::cout << "Initial population simulation took " << std::chrono::duration<double>(diff).count() << " seconds" << std::endl;
    
    auto start2 = std::chrono::high_resolution_clock::now();
    
    Population lastGen = myPop.initialise_LastOfUs();
    
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    std::string output_LastOfUs = "output_LastOfUs.csv";
    lastGen.simulate_LastOfUs(output_LastOfUs);

    auto end2 = std::chrono::high_resolution_clock::now();
    auto diff2 = end2 - start2;
    std::cout << std::endl;
    std::cout << "Last of Us simulation took " << std::chrono::duration<double>(diff2).count() << " seconds\n" << std::endl;
    

    return 0; 
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << '\n';
  }
  catch (const char* err) {
    std::cerr << err << '\n';
  }
  catch (...) {
    std::cerr << "unknown exception\n";
  }
  std::cerr << "bailing out.\n";

  return -1; 
}

