//
//  main.cpp
//  MSc_project_eusociality
//
//  Created by Ella Rees-Baylis on 10/01/2022.
//  Copyright © 2022 Ella Rees-Baylis. All rights reserved.
//

#include "Population.hpp"

int main(int argc, char* argv[]) {
    try {
        std::string file_name = (argc > 2) ? argv[1] : "config.ini";
        
        std::cout<< "reading from config file: " << file_name << "\n";
        std::ifstream test_file(file_name.c_str());
        if(!test_file.is_open()) throw std::runtime_error("can't find config file");
        test_file.close();
        
        params sim_par_in(file_name);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        Population pop;
        pop.initialise_pop(sim_par_in); // initialises population of nests with mated founding females
        pop.write_headers(pop.p.param_names_to_record, pop.p);
        pop.write_basis_matrix_dispersal();
        pop.write_basis_matrix_offspring();
        
        for(int i=1; i<pop.p.years+1; ++i) {
            // brood 1, spring
            if(i % pop.p.output_interval_summary == 0) pop.data_analysis_summary(i, true, pop.p.params_to_record);
            if(i % pop.p.output_interval_genes == 0) pop.data_analysis_genes(i, true, pop.p.params_to_record);
            pop.first_brood(sim_par_in, i);
            
            // brood 2, summer
            if(i % pop.p.output_interval_summary == 0) pop.data_analysis_summary(i, false, pop.p.params_to_record);
            if(i % pop.p.output_interval_genes == 0) pop.data_analysis_genes(i, false, pop.p.params_to_record);
            pop.write_stationarity_data();
            pop.second_brood(sim_par_in, i);
            
            //std::cout << "Year:  " << i << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        std::cout << std::endl;
        std::cout << "That took " << std::chrono::duration<double>(diff).count() << " seconds" << std::endl;
    
        return 0;
        
    }
    
    catch (const std::exception& err) {std::cerr << err.what() << '\n';}
    catch (const char* err) {std::cerr << err << '\n';}
    catch (...) {std::cerr << "unknown exception\n";}
    std::cerr << "bailing out.\n";
    
    return -1;
}
