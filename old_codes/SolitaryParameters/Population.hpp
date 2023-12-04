//
//  Population.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Population_hpp
#define Population_hpp    

#include "Nest.hpp"
#include <queue>
#include <iostream>
#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream

template<int Ploidy>
void printIndividualDetails(const Individual<Ploidy>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << " | Nest ID: " << individual.nest_id << std::endl;
    std::cout << "Genome Values: ";
    for (int i = 0; i < Ploidy; ++i) {
        std::cout << "(" << individual.genome[i].genes_dispersal << ", " << individual.genome[i].genes_choice[0] << ", " << individual.genome[i].genes_choice[1] << ") ";
    }
    std::cout << std::endl;
    std::cout << "Phenotype Dispersal: " << individual.phenotype_dispersal << std::endl;
    std::cout << "Phenotype Choice Intercept: " << individual.phenotype_choice[0] << std::endl;
    std::cout << "Phenotype Choice Slope: " << individual.phenotype_choice[1] << std::endl;
    std::cout << "Is Alive: " << (individual.is_alive ? "true" : "false") << std::endl;
    std::cout << "Is Larvae: " << (individual.is_larvae ? "true" : "false") << std::endl;
    std::cout << "Is Foraging: " << (individual.is_foraging ? "true" : "false") << std::endl;
    std::cout << "Is Disperser: " << (individual.is_disperser ? "true" : "false") << std::endl;
    std::cout << "Is Mated: " << (individual.is_mated ? "true" : "false") << std::endl;
    std::cout << "Body Size: " << individual.body_size << std::endl;
    std::cout << "t_death: " << individual.t_death << std::endl;
    // std::cout << "Number of Offspring: " << individual.num_offspring << std::endl;
    // std::cout << "Number of Female Offspring: " << individual.num_fem_offspring << std::endl;
    // std::cout << "Number of Larvae: " << individual.num_larva << std::endl;
    // std::cout << "Number of Female Larvae: " << individual.num_female_larva << std::endl;
    std::cout << std::endl;
}

// Function to print individual information concisely
void printIndividualInfo(const Individual<2>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << "\n";
    std::cout << "Nest ID: " << individual.nest_id << "\n";
    std::cout << "Time of Death: " << std::fixed << std::setprecision(2) << individual.t_death << "\n";

    // Print genome values
    for (int i = 0; i < 2; ++i) {
        const auto& haplotype = individual.genome[i];
        std::cout << "Haplotype " << i + 1 << " Genome: (" << haplotype.genes_dispersal << ", "
                  << haplotype.genes_choice[0] << ", " << haplotype.genes_choice[1] << ")\n";
    }

    // Print sperm gene values if mated
    if (individual.is_mated) {
        const auto& sperm = individual.sperm;
        std::cout << "Sperm Genome: (" << sperm.genes_dispersal << ", " << sperm.genes_choice[0] << ", "
                  << sperm.genes_choice[1] << ")\n";
    }
    std::cout << "\n" << std::endl;
}

// track_time struct
struct track_time {
    float time;
    int nest_id;
    int ind_id;

    track_time(const Individual<2>& input) {
      ind_id = input.ind_id;
      nest_id = input.nest_id;
      time = input.t_next;
    }
};

class Population {
public:
    Population(const params& par) : p(par) { };
    std::vector<Individual<1> > adult_males;       // vector for males to fertilize female
    std::vector<Nest> nests;
    std::vector<size_t> empty_nests;
    std::vector<unsigned int> storer_nest_id;

    void initialise_pop();
    int choose_RndmMale();
    std::tuple<bool, unsigned int> update_emptyNests();
    void mate(Individual<2>& female);
    void removeDeadMales();

    void simulate_tst(); 
    int calculateTotalAdultFemales();
    void printIndividualInfo(const Individual<2>& individual);
    int findIndexByNestId(unsigned int nestId); // maybe size_t

    
    unsigned int nest_id_counter = 0;
    unsigned int individual_id_counter = 0;
    
    params p;
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

int Population::findIndexByNestId(unsigned int nestId) {
        auto it = std::find(storer_nest_id.begin(), storer_nest_id.end(), nestId);
        if (it != storer_nest_id.end()) {
            return static_cast<int>(std::distance(storer_nest_id.begin(), it));
        } else {
            return -1;  // Return -1 if nest_id is not found
        }
    }

void Population::simulate_tst() {
    // Create a priority queue to track individuals by their next action time
    std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> event_queue(cmptime);

    // Initialize the event queue with individuals and their initial next action times
    for (auto& nest : nests) {
        for (auto& individual : nest.adult_females) {
            event_queue.push(track_time(individual));
        }
    }

    // Simulate for a fixed number of events/individual updates
    for (int event = 0; event < max_events; ++event) {
        if (event_queue.empty()) {
            // No more events to process
            std::cout << "ERROR: event_queue empty" << std::endl;
            break;
        }
        if (event % check_dead_males == 0) {
            removeDeadMales();
        }
        // Get the individual with the earliest next action time
        track_time next_event = event_queue.top();
        event_queue.pop();
        size_t cnestid = findIndexByNestId(next_event.nest_id);
        auto cindid = next_event.ind_id;
        auto current_index = nests[cnestid].findFemaleIndexById(cindid);
        auto current = nests[cnestid].adult_females[current_index];
        // TST DELETE//
        std::cout << "\nE: " << event << " | Ind: " << cindid << " | Nid: " << next_event.nest_id << " | GT: " << gtime 
        << " | Nindex: " << cnestid <<  " | Alive_before: " << (current.is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current.is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;

        gtime = current.t_next;

        // Last task survival check
        if (current.is_alive) {
            if (current.is_foraging) {
                std::tuple<bool, bool, size_t> index = nests[cnestid].feed(p.dForagingMean, p.dForagingSD);
                if (std::get<0>(index) == true) {
                    if (std::get<1>(index) == false) { // if male
                        if (nests[cnestid].larval_males[std::get<2>(index)].check_mature(gtime, p)) {
                            size_t old_lmales = nests[cnestid].larval_males.size();
                            size_t old_amales = adult_males.size();
                            
                            adult_males.push_back(nests[cnestid].larval_males[std::get<2>(index)]);
                            remove_from_vec(nests[cnestid].larval_males, std::get<2>(index));
                            
                            std::cout << "FOR Male index: " << std::get<2>(index) // << " BSize: "<< nests[cnestid].larval_males[std::get<2>(index)].body_size
                            << " MATURED Adult from " << old_amales << " to " << adult_males.size()
                            << " Larvae From " << old_lmales << " to " << nests[cnestid].larval_males.size() << std::endl;
                        } else { // TST
                            std::cout << "FOR Male index: " << std::get<2>(index) // << " BSize: " << nests[cnestid].larval_males[std::get<2>(index)].body_size 
                            << " No Mature " << std::endl;
                        }
                    } else { // if female
                        if (nests[cnestid].larval_females[std::get<2>(index)].check_mature(gtime, p)) {
                            if (nests[cnestid].larval_females[std::get<2>(index)].check_disperser()) {
                                mate(nests[cnestid].larval_females[std::get<2>(index)]);
                                auto empty = update_emptyNests();
                                size_t empty_index = findIndexByNestId(std::get<1>(empty));
                                
                                if (std::get<0>(empty) == true) {

                                    int confirm_empty = nests[empty_index].adult_females.size(); // TST DELETE

                                    nests[empty_index].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                    nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));
                                    
                                    nests[empty_index].larval_females.clear();
                                    nests[empty_index].larval_males.clear();
                                    
                                    nests[empty_index].adult_females[0].t_next = gtime;
                                    nests[empty_index].task_check(nests[empty_index].adult_females[0], p);
                                    nests[empty_index].adult_females[0].nest_id = nest_id_counter; //JK: I would not use the same nest_id again but have a nest id that continues whenever there is a new nest founded, i.e., the nest_id_counter in the population class and then increase it whenever a new nest is founded
                                    storer_nest_id[empty_index] = nest_id_counter;
                                    nests[empty_index].nest_id = nest_id_counter;
                                    nest_id_counter++;
                                    event_queue.push(track_time(nests[empty_index].adult_females[0])); // add matured to queue

                                    std::cout << "1FOR Female id: " << nests[empty_index].adult_females[0].ind_id << " BSize: " << nests[empty_index].adult_females[0].body_size
                                    << " MATURED DISPERSER Nest From " << current.nest_id << " to " << storer_nest_id[empty_index]
                                    << " NewAFem size from " << confirm_empty << " to " << nests[empty_index].adult_females.size() <<std::endl;
                                } else {
                                    size_t old_lfemales = nests[cnestid].larval_females.size();
                                    double old_lbodyzize = nests[cnestid].larval_females[std::get<2>(index)].body_size;
                                    nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));
                                    
                                    std::cout << "2FOR Female index: " << std::get<2>(index) << " BSize: " << old_lbodyzize
                                    << " MATURED DISPERSER Died From " << current.nest_id << " LFemales From "
                                    << old_lfemales << " to " << nests[cnestid].larval_females.size() << std::endl;
                                }
                            } else {
                                size_t old_lfemales = nests[cnestid].larval_females.size();
                                size_t old_afemales = nests[cnestid].adult_females.size();

                                double old_bodysize = nests[cnestid].larval_females[std::get<2>(index)].body_size;
                                mate(nests[cnestid].larval_females[std::get<2>(index)]);

                                nests[cnestid].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));

                                nests[cnestid].adult_females[old_afemales].t_next = gtime;
                                nests[cnestid].task_check(nests[cnestid].adult_females[old_afemales], p);
                                
                                event_queue.push(track_time(nests[cnestid].adult_females[old_afemales]));
                                std::cout << "3FOR Female index: " << std::get<2>(index) << " BSize: " << old_bodysize
                                << " MATURED Afems from " << old_afemales << " to " << nests[cnestid].adult_females.size()
                                << " Larval fems from " << old_lfemales << " to " << nests[cnestid].larval_females.size() <<std::endl;  
                            }
                        }
                        // TST DELETE
                        else std::cout << "4FOR Female index: " << std::get<2>(index) << " BSize: " << nests[cnestid].larval_females[std::get<2>(index)].body_size << " NO Mature" << std::endl;
                    }
                }
            } else {
                size_t num_lmale = nests[cnestid].larval_males.size(); // TST
                size_t num_lfemale = nests[cnestid].larval_females.size(); // TST
                nests[cnestid].reproduce(nests[cnestid].adult_females[current_index], individual_id_counter, p);
                ++individual_id_counter;
                std::cout << "REP LFem From " << num_lfemale <<" to " << nests[cnestid].larval_females.size() 
                << " LMale From " << num_lmale <<" to " << nests[cnestid].larval_males.size() << std::endl; // TST
            }

            // Task check
            nests[cnestid].task_check(nests[cnestid].adult_females[current_index], p);
            nests[cnestid].adult_females[current_index].survival(p);
            event_queue.push(track_time(nests[cnestid].adult_females[current_index]));

        
        } // Death condition 
        else {
            std::cout << "DEAD" << std::endl;
            nests[cnestid].adult_females.erase(nests[cnestid].adult_females.begin() + current_index);
        }
    }
}

// returns index + 1 of selected male
int Population::choose_RndmMale(){
    if (adult_males.size() != 0) {
        size_t index = uni_int(0, adult_males.size()) + 1;
        return static_cast<int>(index);
    }
    else return 0;
}

// returns true if female finds mate
void Population::mate(Individual<2>& female){
    int index = choose_RndmMale();
    if (index){
        female.mate(adult_males[index - 1]);
    }
}

// updates empty nest list and returns tuple
// if_empty, index, id
std::tuple<bool, unsigned int> Population::update_emptyNests() { //JK: I am not a fan of this +1 indexing here and in other functions. can we solve this differently?
    empty_nests.clear();
    for (size_t i = 0; i < nests.size(); ++i) {
        if (!nests[i].adult_females.size()) {
            empty_nests.push_back(nests[i].nest_id);
        }
    }
    if (empty_nests.empty()) {
        return std::make_tuple(false, 0); // No empty nests found
    } else {
        size_t randomIndex = uni_int(0, empty_nests.size()); //LCO: can move to next line
        return std::make_tuple(true, empty_nests[randomIndex]); //JK: I don't quite understand this line to be honest
    }
}

void Population::removeDeadMales() {
    auto Condition = [](const Individual<1>& male) {
        return male.t_death <= gtime;
    };
    adult_males.erase(std::remove_if(adult_males.begin(), adult_males.end(), Condition), adult_males.end());
    // How about incorporating erase_if here itself // ??
}

// initialise population function
void Population::initialise_pop() {
    // population founders (using default constructor, which calls default constructor of Haplotype)
    adult_males.clear();
    nests.clear();
    empty_nests.clear();
    Individual<1> adam(individual_id_counter);
    ++individual_id_counter;
    adult_males.push_back(adam);
    
    for(int i=0; i < max_nests; ++i) {
        Individual<2> eve(individual_id_counter);
        ++individual_id_counter;
        eve.mate(adam);
        eve.nest_id = nest_id_counter;
        storer_nest_id.push_back(i);
        eve.t_next = uni_real();
        eve.is_foraging = false;
        ++nest_id_counter;
        nests.emplace_back(eve, eve.nest_id);
    }
}

// TST REDN
int Population::calculateTotalAdultFemales() {
    int totalAdultFemales = 0;

    for (const auto& nest : nests) {
        totalAdultFemales += static_cast<int>(nest.adult_females.size());
    }

    return totalAdultFemales;
}

// TST Function to print individual information concisely
void Population::printIndividualInfo(const Individual<2>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << "\n";
    std::cout << "Nest ID: " << individual.nest_id << "\n";
    std::cout << "TNext: " << std::fixed << std::setprecision(2) << individual.t_next << "\n";

    // Print genome values
    for (int i = 0; i < 2; ++i) {
        const auto& haplotype = individual.genome[i];
        std::cout << "Haplotype " << i + 1 << " Genome: (" << haplotype.genes_dispersal << ", "
                  << haplotype.genes_choice[0] << ", " << haplotype.genes_choice[1] << ")\n";
    }

    // Print sperm gene values if mated
    if (individual.is_mated) {
        const auto& sperm = individual.sperm;
        std::cout << "Sperm Genome: (" << sperm.genes_dispersal << ", " << sperm.genes_choice[0] << ", "
                  << sperm.genes_choice[1] << ")\n";
    }
    std::cout << "\n" << std::endl;
}

#endif /* Population_hpp */

