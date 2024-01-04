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
    std::cout << "max_nests_evolution = " << max_nests_evolution << std::endl;
    std::cout << "max_nests_LastOfUs = " << max_nests_LastOfUs << std::endl;
    std::cout << "dInitDispersal = " << dInitDispersal << std::endl;
    std::cout << "dInitChoiceIntercept = " << dInitChoiceIntercept << std::endl;
    std::cout << "dInitChoiceSlope = " << dInitChoiceSlope << std::endl;
    std::cout << "Simulation ID = " << simulationID << std::endl;
    std::cout << "Is Additive = " << bIsAdditive << std::endl;
    std::cout << "Locuswise recombination = " << bRecombLocus << std::endl;
    std::cout << "\nReading from config file: " << file_name << "\n";
    std::ifstream test_file(file_name.c_str());
    if (!test_file.is_open()) {
      throw std::runtime_error("can't find config file");
    }
    test_file.close();

    params sim_par_in(file_name);

    // Create CSV output folder
    std::string csv_folder = "OutputCSV";
    if (!fs::exists(csv_folder)) {
        try {
            fs::create_directory(csv_folder);
            std::cout << "Folder created successfully." << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating folder: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "Folder already exists." << std::endl;
    }

    // Create Images output folder
    std::string images_folder = "OutputImages";
    if (!fs::exists(images_folder)) {
        try {
            fs::create_directory(images_folder);
            std::cout << "Folder created successfully." << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating folder: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "Folder already exists." << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    Population myPop(sim_par_in);
    myPop.initialise_pop();

    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    myPop.simulate_tst(myPop.p.param_names_to_record, csv_folder);

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    std::cout << std::endl;
    std::cout << "Initial population simulation took " << std::chrono::duration<double>(diff).count() << " seconds" << std::endl;

    std::cout << "\nSTARTING last of us" << std::endl;

    auto start2 = std::chrono::high_resolution_clock::now();
    
    Population lastGen = myPop.initialise_LastOfUs();
    
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    lastGen.simulate_LastOfUs(myPop.p.param_names_to_record, csv_folder);

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

