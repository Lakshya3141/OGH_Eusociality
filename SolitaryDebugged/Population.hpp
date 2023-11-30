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
    Population() = default;
    std::vector<Individual<1> > adult_males;       // vector for males to fertilize female
    std::vector<Nest> nests;
    std::vector<size_t> empty_nests;

    void initialise_pop();
    int choose_RndmMale();
    int update_emptyNests();
    bool mate(Individual<2>& female);
    void removeDeadMales();

    //  TST  //
    void simulate_tst(); 
    int calculateTotalAdultFemales();
    void printIndividualInfo(const Individual<2>& individual);
    size_t findNestIndexById(unsigned long int id);
    
    unsigned int nest_id_counter = 0;
    unsigned int individual_id_counter = 0;
    
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// TST
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
        auto cnestid = next_event.nest_id;
        auto cindid = next_event.ind_id;
        auto current_index = nests[cnestid].findFemaleIndexById(cindid) - 1;
        auto current = nests[cnestid].adult_females[current_index];
        // TST DELETE//
        std::cout << "\nE: " << event << " | Ind: " << cindid << " | Nid: " << cnestid << " | GT: " << gtime 
        << " | tn_bef: " << next_event.time <<  " | Alive_before: " << (current.is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current.is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;

        std::cout << "Dispersal: " << current.phenotype_dispersal
        << " Choice Int: " << current.phenotype_choice[0]
        << " Choice Slope: " << current.phenotype_choice[1] << std::endl;

        gtime = current.t_next;

        // Last task survival check
        if (current.is_alive) {
            if (current.is_foraging) {
                std::tuple<bool, bool, size_t> index = nests[cnestid].feed(dForagingMean, dForagingSD);
                if (std::get<0>(index) == true) {
                    if (std::get<1>(index) == false) { // if male
                        if (nests[cnestid].larval_males[std::get<2>(index)].check_mature(gtime)) {
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
                        if (nests[cnestid].larval_females[std::get<2>(index)].check_mature(gtime)) {
                            if (nests[cnestid].larval_females[std::get<2>(index)].check_disperser()) {
                                bool mated = mate(nests[cnestid].larval_females[std::get<2>(index)]); // TST MODIFY mated function maybe
                                int empty = update_emptyNests();
                                if (empty > 0) {
                                    --empty;
                                    size_t confirm_empty = nests[empty].adult_females.size(); // TST DELETE

                                    nests[empty].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                    nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));
                                    nests[empty].task_check(nests[empty].adult_females[0]);
                                    nests[empty].adult_females[0].nest_id = empty;

                                    event_queue.push(track_time(nests[empty].adult_females[0])); // add matured to queue

                                    std::cout << "1FOR Female index: " << std::get<2>(index) << " BSize: " << nests[empty].adult_females[0].body_size
                                    << " MATURED DISPERSER Nest From " << current.nest_id << " to " << empty
                                    << " NewAFem size from " << confirm_empty << " to " << nests[empty].adult_females.size() <<std::endl;
                                } else { // TST DELETE a bunch
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
                                // TST
                                double old_bodysize = nests[cnestid].larval_females[std::get<2>(index)].body_size;
                                bool mated = mate(nests[cnestid].larval_females[std::get<2>(index)]);

                                nests[cnestid].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));

                                nests[cnestid].adult_females[old_afemales].t_next = gtime;
                                nests[cnestid].task_check(nests[cnestid].adult_females[old_afemales]);
                                
                                event_queue.push(track_time(nests[cnestid].adult_females[old_afemales])); // TST UNCOMMENT add matured to queue
                                std::cout << "3FOR Female index: " << std::get<2>(index) << " BSize: " << old_bodysize
                                << " MATURED Afems from " << old_afemales << " to " << nests[cnestid].adult_females.size()
                                << " Larval fems from " << old_lfemales << " to " << nests[cnestid].larval_females.size() <<std::endl;  
                            }
                        }
                        // TST DELETE
                        else std::cout << "4FOR Female index: " << std::get<2>(index) << " BSize: " << nests[current.nest_id].larval_females[std::get<2>(index)].body_size << " NO Mature" << std::endl;
                    }
                }
            } else {
                size_t num_lmale = nests[cnestid].larval_males.size(); // TST
                size_t num_lfemale = nests[cnestid].larval_females.size(); // TST
                nests[cnestid].reproduce(nests[cnestid].adult_females[current_index], individual_id_counter);
                ++individual_id_counter;
                std::cout << "REP LFem From " << num_lfemale <<" to " << nests[current.nest_id].larval_females.size() 
                << " LMale From " << num_lmale <<" to " << nests[current.nest_id].larval_males.size() << std::endl; // TST
            }

            // Task check
            nests[cnestid].task_check(nests[cnestid].adult_females[current_index]);
            nests[cnestid].adult_females[current_index].survival();
            event_queue.push(track_time(nests[cnestid].adult_females[current_index]));

        
        } // Death condition 
        else {
            std::cout << "DEAD" << std::endl;
            nests[cnestid].adult_females.erase(nests[cnestid].adult_females.begin() + current_index);
            
        }

        // TST //
        // std::cout << "     | Ind: " << current.ind_id << " | Nid: " << current.nest_id << " | GT: " << gtime 
        // << " | tn_now: " << current.t_next <<  " | Alive_now: " << (current.is_alive ? "YES" : "NO ") 
        // << " | Task_now: " << (current.is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
        // std::cout << "Adult Len Females: " << calculateTotalAdultFemales() << std::endl; // TST
    }
}

// Function to search by Nest ID and return index + 1
size_t Population::findNestIndexById(unsigned long int id) {
    auto it = std::find_if(nests.begin(), nests.end(), [id](const Nest& nest) {
        return nest.nest_id == id;
    });
    if (it != nests.end()) {
        return std::distance(nests.begin(), it) + 1;
    } else {
        return 0;
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
bool Population::mate(Individual<2>& female){
    int index = choose_RndmMale();
    // std::cout << "Male index chosen: " << index - 1 << std::endl; // TST
    if (index){
        female.mate(adult_males[index - 1]);
        return true;
    }
    else {
        return false;
    }
}

// updates empty nest list and returns empty one from that LC5
int Population::update_emptyNests() {
    empty_nests.clear();
    for (size_t i = 0; i < nests.size(); ++i) {
        if (!nests[i].adult_females.size()) {
            empty_nests.push_back(i);
        }
    }
    // printVector(empty_nests); // TST
    if (empty_nests.empty()) {
        return 0; // No empty nests found
    } else {
        size_t randomIndex = uni_int(0, empty_nests.size()); //LCO: can move to next line
        return static_cast<int>(empty_nests[randomIndex] + 1);
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
    
    Individual<1> adam(individual_id_counter);
    ++individual_id_counter;
    adult_males.push_back(adam);
    double cnt = 2.0; // TST DELETE
    
    for(int i=0; i < max_nests; ++i) {
        Individual<2> eve(individual_id_counter);
        ++individual_id_counter;
        // eve.mate(adam); // TST UNCOMMENT
        eve.nest_id = nest_id_counter;
        eve.t_next = uni_real();
        eve.is_foraging = false;
        ++nest_id_counter;
        eve.genome[0].genes_dispersal = cnt; //TST DELETE
        cnt += 0.5; // TST DELETE
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

