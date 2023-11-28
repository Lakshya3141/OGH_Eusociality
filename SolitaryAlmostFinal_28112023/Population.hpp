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
    Individual<2>* ind;

    track_time(Individual<2>* input) {
      ind = input;
      time = ind->t_next;
      // std::cout << "IndID: "<< ind->ind_id << std::endl;
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
            event_queue.push(track_time(&individual));
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
        // std::cout << "Prepop Qlen: " << event_queue.size() << std::endl; // TST
        event_queue.pop();
        // std::cout << "Pospop Qlen: " << event_queue.size() << std::endl; // TST
        auto current = next_event.ind;
        // TST DELETE// 
        std::cout << "\nE: " << event << " | Ind: " << current->ind_id << " | Nid: " << current->nest_id << " | GT: " << gtime 
        << " | tn_bef: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
        // current->is_alive = false; // TST DELETE

        gtime = current->t_next;
        size_t cnind = current->nest_id; 
        // nind Might seriously backfire if we delete nests, then we need to replace it with
        // findNestByIndex function call at each place probably 
        // cnind stands for current nest index

        // Last task survival check
        if (current->is_alive) { 
            // std::cout << "ALIVE" << std::endl; // TST DELETE
            // TST Larval part shifted to the end
            if (current->is_foraging) {
                // std::cout << "FORAGING" << std::endl; // TST DELETE
                std::cout << "p# larvae:  " << nests[cnind].larval_females.size() + nests[cnind].larval_males.size()
                << " | p# male larvae: " << nests[cnind].larval_males.size() 
                << " | p# female larvae: " << nests[cnind].larval_females.size() << std::endl; // TST DELETE
                std::tuple<bool, bool, size_t> index = nests[cnind].feed(dForagingMean, dForagingSD);
                std::cout << "feed complete" << std::endl; // TST DELETE
                
                if (std::get<0>(index) == true) {
                    if (std::get<1>(index) == false) { // if male
                        if (nests[cnind].larval_males[std::get<2>(index)].check_mature(gtime)) {
                            std::cout << "check 1" << std::endl;
                            size_t old_lmales = nests[cnind].larval_males.size();
                            size_t old_amales = adult_males.size();
                            
                            adult_males.push_back(nests[cnind].larval_males[std::get<2>(index)]);
                            remove_from_vec(nests[cnind].larval_males, std::get<2>(index));
                            
                            std::cout << "FOR Male index: " << std::get<2>(index) << " BSize: "<< nests[cnind].larval_males[std::get<2>(index)].body_size
                            << " MATURED Adult from " << old_amales << " to " << adult_males.size()
                            << " Larvae From " << old_lmales << " to " << nests[cnind].larval_males.size() << std::endl;
                        } else { // TST
                            std::cout << "FOR Male index: " << std::get<2>(index) << " BSize: " << nests[cnind].larval_males[std::get<2>(index)].body_size << " No Mature " << std::endl;
                        }
                    } else { // if female
                        // TST
                        // edit index setting here
                        if (nests[cnind].larval_females[std::get<2>(index)].check_mature(gtime)) {
                            
                            std::cout << "check 2" << std::endl;
                            // nests[cnind].task_check(nests[cnind].larval_females[std::get<2>(index)]); // TST CHANGE POSITION!!
                            nests[cnind].larval_females[std::get<2>(index)].phenotype_dispersal = 0.0; // TST DELETE
                            std::cout << "check 3" << std::endl;
                            if (nests[cnind].larval_females[std::get<2>(index)].check_disperser()) {
                                std::cout << "check 4" << std::endl;
                                bool mated = mate(nests[cnind].larval_females[std::get<2>(index)]); // TST MODIFY mated function maybe
                                int empty = update_emptyNests();
                                if (empty > 0) {
                                    std::cout << "check empty" << std::endl;
                                    --empty;
                                    size_t confirm_empty = nests[empty].adult_females.size(); // TST DELETE

                                    // nests[current->nest_id].adult_females[0].t_next = gtime; // LC: Why do this? the first female in that vector is not being accessed right now
                                    
                                    // nests[current->nest_id].task_check(nests[current->nest_id].adult_females[0]); // LC: Why do this

                                    nests[empty].adult_females.push_back(nests[cnind].larval_females[std::get<2>(index)]);
                                    nests[cnind].larval_females.erase(nests[cnind].larval_females.begin() + std::get<2>(index));
                                    nests[empty].task_check(nests[empty].adult_females[0]);
                                    
                                    event_queue.push(track_time(&nests[empty].adult_females[0])); // add matured to queue
                                    std::cout << "1FOR Female index: " << std::get<2>(index) << " BSize: " << nests[empty].adult_females[0].body_size
                                    << " MATURED DISPERSER Nest From " << current->nest_id << " to " << empty
                                    << " NewAFem size from " << confirm_empty << " to " << nests[empty].adult_females.size() <<std::endl;
                                } else { // TST DELETE a bunch
                                    size_t old_lfemales = nests[cnind].larval_females.size();
                                    double old_lbodyzize = nests[cnind].larval_females[std::get<2>(index)].body_size;
                                    nests[cnind].larval_females.erase(nests[cnind].larval_females.begin() + std::get<2>(index));
                                    
                                    std::cout << "2FOR Female index: " << std::get<2>(index) << " BSize: " << nests[].larval_females[std::get<2>(index)].body_size
                                    << " MATURED DISPERSER Died From " << current->nest_id << " LFemales From "
                                    << old_lfemales << " to " << nests[cnind].larval_females.size() << std::endl;
                                }
                            } else {
                                // TST
                                std::cout << "check 5" << std::endl;
                                size_t old_lfemales = nests[cnind].larval_females.size();
                                size_t old_afemales = nests[cnind].adult_females.size();
                                // TST
                                double old_bodysize = nests[cnind].larval_females[std::get<2>(index)].body_size;
                                bool mated = mate(nests[cnind].larval_females[std::get<2>(index)]);
                                if (!mated) {
                                    continue; // TST :: remove this
                                }
                            
                                
                                std::cout << "nest id " << current->nest_id << std::endl;
                                std::cout << "ind id " << current->ind_id << std::endl;
                                std::cout << "check 6" << std::endl;
                                std::cout << "size before push_back" << nests[cnind].larval_females.size() << std::endl;
                                nests[cnind].adult_females.push_back(nests[cnind].larval_females[std::get<2>(index)]);
                                std::cout << "size after push_back" << nests[cnind].larval_females.size() << std::endl;
                                
                                std::cout << "check 7" << std::endl;
                                std::cout << "nest id " << current->nest_id << std::endl;
                                std::cout << "ind id " << current->ind_id << std::endl;
                                std::cout << "larval_female size " << nests[cnind].larval_females.size() << std::endl;
                                std::cout << "larval position " << std::get<2>(index) << std::endl;
                                nests[cnind].larval_females.erase(nests[cnind].larval_females.begin() + std::get<2>(index));
                                
                                nests[cnind].adult_females[old_afemales].t_next = gtime;
                                nests[cnind].task_check(nests[cnind].adult_females[old_afemales]);
                                // printIndividualInfo(nests[current->nest_id].adult_females[old_afemales]);
                                //remove_from_vec(nests[current->nest_id].larval_females, index);
                                
                                
                                std::cout << "check 8" << std::endl;
                                event_queue.push(track_time(&nests[nind].adult_females[old_afemales])); // add matured to queue
                                std::cout << "3FOR Female index: " << std::get<2>(index) << " BSize: " << old_bodysize
                                    << " MATURED Afems from " << old_afemales << " to " << nests[nind].adult_females.size()
                                    << " Larval fems from " << old_lfemales << " to " << nests[nind].larval_females.size() <<std::endl;
                                
                                
                                
                                
                            }
                            // nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                            // remove_from_vec(nests[current->nest_id].larval_females, index);
                        }
                        // TST
                        else std::cout << "4FOR Female index: " << std::get<2>(index) << " BSize: " << nests[current->nest_id].larval_females[std::get<2>(index)].body_size << " NO Mature" << std::endl;
                    }
                }
                
                
                
                
                
            } else {
                // std::cout << "REPRODUCING" << std::endl;
                size_t num_lmale = nests[current->nest_id].larval_males.size(); // TST
                size_t num_lfemale = nests[current->nest_id].larval_females.size(); // TST
                std::cout << "ind_id before reproduce " << current->ind_id << std::endl;
                nests[current->nest_id].reproduce(*current, individual_id_counter);
                ++individual_id_counter;
                std::cout << "REP LarvFemale From " << num_lfemale <<" to " << nests[current->nest_id].larval_females.size() << std::endl; // TST
                std::cout << "REP LarvMale From " << num_lmale <<" to " << nests[current->nest_id].larval_males.size() << std::endl; // TST
                if(!current->is_mated) {
                    bool dummy = mate(*current);
                } 
            }

            // Task check
            nests[current->nest_id].task_check(*current);
            current->survival();

            event_queue.push(track_time(current));

        
        } // Death condition 
        else {
            // std::cout << "DEAD" << std::endl;
            // Find the position of the current individual in its nest's adult_females vector
            size_t ind = nests[current->nest_id].findFemaleIndexById(current->ind_id);
            // std::cout << "INDEX: " << ind << std::endl; // TST
            if (ind) {
                // std::cout << "Len bef: " << nests[current->nest_id].adult_females.size() << std::endl; // TST
                // auto it = nests[current->nest_id].adult_females.begin() + ind - 1;
                // remove_from_vec2(nests[current->nest_id].adult_females, it);
                nests[current->nest_id].adult_females.erase(nests[current->nest_id].adult_females.begin() + ind - 1);
                // remove_from_vec(nests[current->nest_id].adult_females, ind - 1);
                // std::cout << "Len aft: " << nests[current->nest_id].adult_females.size() << std::endl; // TST
            } else {
                // LC3 :: Throw error, need to learn error handling
            }
        }

        // TST //
        // std::cout << "     | Ind: " << current->ind_id << " | GT: " << gtime 
        // << " | tn_now: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") 
        // << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
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

