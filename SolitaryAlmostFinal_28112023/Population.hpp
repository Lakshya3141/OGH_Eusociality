//
//  Population.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Population_hpp
#define Population_hpp    

#include "Nest.hpp"
//#include "Random.hpp" //JK: here too
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
      std::cout << "IndID: "<< ind->ind_id << std::endl;
    }
};

class Population {
public:
    Population() = default;
    std::vector<Individual<1> > adult_males;       // vector for males to fertilize female
    // std::vector<Individual<2> > disperser_females;
    std::vector<Nest> nests;
    std::vector<size_t> empty_nests;

    void initialise_pop();
    void simulate();
    int choose_RndmMale();
    int update_emptyNests();
    bool mate(Individual<2>& female);
    void removeDeadMales();

    // TST
    void simulate_tst();
    int calculateTotalAdultFemales();
    void printIndividualInfo(const Individual<2>& individual);
    
    unsigned int nest_id_counter = 0;
    unsigned int individual_id_counter = 0;
    
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// initialise population function
void Population::initialise_pop() {
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    
    Individual<1> adam(individual_id_counter);
    ++individual_id_counter;
    adult_males.push_back(adam);
    double cnt = 2.0; //TST
    
    for(int i=0; i < max_nests; ++i) {
        Individual<2> eve(individual_id_counter);
        ++individual_id_counter;
        // eve.mate(adam); // TST UNCOMMENT
        eve.nest_id = nest_id_counter;
        ++nest_id_counter;
        eve.genome[0].genes_dispersal = cnt; //TST
        cnt += 0.5; // TST
        //Nest dumNest(i+1, eve); //JK: commented this out. call constructor within emplace back. line below
        nests.emplace_back(eve, eve.nest_id);
    }
}

// TST
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
        
        if (event % 10000 == 0) { //JK: maybe make those 10000 a parameter that can be varied. i.e. define it in the parameters header
            removeDeadMales();
        }
        // Get the individual with the earliest next action time
        track_time next_event = event_queue.top();
        // std::cout << "Prepop Qlen: " << event_queue.size() << std::endl; // TST
        event_queue.pop();
        // std::cout << "Pospop Qlen: " << event_queue.size() << std::endl; // TST
        auto current = next_event.ind;
        // TST // 
        std::cout << "\nE: " << event << " | Ind: " << current->ind_id << " | Nid: " << current->nest_id << " | GT: " << gtime 
        << " | tn_bef: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
        // current->is_alive = false; // TST

        gtime = current->t_next;

        // Last task survival check
        if (current->is_alive) { 
            // std::cout << "ALIVE" << std::endl;
            // TST Larval part shifted to the end
            if (current->is_foraging) {
                // std::cout << "FORAGING" << std::endl;
                std::cout << "nLarvae " << nests[current->nest_id].larval_females.size() << std::endl;
                std::tuple<bool, bool, size_t> index = nests[current->nest_id].feed(dForagingMean, dForagingSD);
                std::cout << "feed complete" << std::endl;
                
                if (get<0>(index) == true) {
                    if (get<1>(index) == false) { // if male
                        // TST
                        // edit index setting here
                        if (nests[current->nest_id].larval_males[get<2>(index)].check_mature(gtime)) {
                            // TST
                            // std::cout << "MALE Mat" << std::endl;
                            std::cout << "check 1" << std::endl;
                            size_t old_lmales = nests[current->nest_id].larval_males.size(); //JK: removed some implicit conversion warnings here
                            size_t old_amales = adult_males.size();
                            //Individual<1> temp = nests[current->nest_id].larval_males[index];
                            adult_males.push_back(nests[current->nest_id].larval_males[get<2>(index)]);
                            remove_from_vec(nests[current->nest_id].larval_males, get<2>(index));
                            // nests[current->nest_id].larval_males.erase(nests[current->nest_id].larval_males.begin() + index);
                            std::cout << "FOR Male index: " << get<2>(index) << " BSize: "<< nests[current->nest_id].larval_males[get<2>(index)].body_size
                            << " MATURED Adult from " << old_amales << " to " << adult_males.size()
                            << " Larvae From " << old_lmales << " to " << nests[current->nest_id].larval_males.size() << std::endl;
                        } else { //TST
                            std::cout << "FOR Male index: " << get<2>(index) << " BSize: " << nests[current->nest_id].larval_males[get<2>(index)].body_size << " No Mature " << std::endl;
                        }
                    } else { // if female
                        // TST
                        // edit index setting here
                        if (nests[current->nest_id].larval_females[get<2>(index)].check_mature(gtime)) {
                            
                            std::cout << "check 2" << std::endl;                            // Need to check for task since check_task moved to nest level
                            // VIMP NEED TO CHANGE PLACEMENT OF THIS SO AS TO MAKE SURE TASK CHECK
                            // FOR DISPERSER DOESNT WORK ON MATURING NEST
                            nests[current->nest_id].task_check(nests[current->nest_id].larval_females[get<2>(index)]); // LCIMP gotta change position
                            // TST MAJOR
                            nests[current->nest_id].larval_females[get<2>(index)].phenotype_dispersal = 0.0;
                            // TST
                            // std::cout << nests[current->nest_id].larval_females[index].phenotype_dispersal << std::endl;
                            
                            std::cout << "check 3" << std::endl;
                            if (nests[current->nest_id].larval_females[get<2>(index)].check_disperser()) {
                                
                                std::cout << "check 4" << std::endl;
                                bool mated = mate(nests[current->nest_id].larval_females[get<2>(index)]);

                                int empty = update_emptyNests();
                                if (empty > 0) {
                                    std::cout << "check empty" << std::endl;
                                    // std::cout << "FEMALE Disp Mat" << std::endl;
                                    --empty;
                                    // TST
                                    size_t confirm_empty = nests[empty].adult_females.size(); //JK: fixed conversion warning

                                    //Individual<2> tempCopy = nests[current->nest_id].larval_females[index];
                                    nests[current->nest_id].adult_females[0].t_next = gtime;
                                    nests[current->nest_id].task_check(nests[current->nest_id].adult_females[0]);
                                    nests[empty].adult_females.push_back(nests[current->nest_id].larval_females[get<2>(index)]);

                                    //remove_from_vec(nests[current->nest_id].larval_females, index);
                                    nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + get<2>(index));
                                    //current->nest_id = empty; // Change nest ID too //JK: why is the nest id of the current individual changing? that seems wrong. this individual does not disperse. only the emerged larvae
                                    // event_queue.push(track_time(&nests[empty].adult_females[0])); // add matured to queue
                                    // TST
                                    
                                    std::cout << "1FOR Female index: " << get<2>(index) << " BSize: " << nests[current->nest_id].larval_females[get<2>(index)].body_size
                                    << " MATURED DISPERSER Nest From " << current->nest_id << " to " << empty
                                    << " NewAFem size from " << confirm_empty << " to " << nests[empty].adult_females.size() <<std::endl;
                                } else { // TST
                                    // std::cout << "FEMALE Disp Die" << std::endl;
                                    size_t old_lfemales = nests[current->nest_id].larval_females.size(); //JK: fixed conversion warning
                                    //remove_from_vec(nests[current->nest_id].larval_females, index);
                                    nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + get<2>(index));
                                    
                                    std::cout << "2FOR Female index: " << get<2>(index) << " BSize: " << nests[current->nest_id].larval_females[get<2>(index)].body_size
                                    << " MATURED DISPERSER Died From " << current->nest_id << " LFemales From "
                                    << old_lfemales << " to " << nests[current->nest_id].larval_females.size() << std::endl;
                                }
                            } else {
                                // std::cout << "FEMALE Mat" << std::endl;
                                // TST
                                std::cout << "check 5" << std::endl;
                                size_t old_lfemales = nests[current->nest_id].larval_females.size();
                                size_t old_afemales = nests[current->nest_id].adult_females.size();
                                // TST
                                double old_bodysize = nests[current->nest_id].larval_females[get<2>(index)].body_size;
                                bool mated = mate(nests[current->nest_id].larval_females[get<2>(index)]);
                                if (!mated) {
                                    continue; //LC3 :: remove this
                                }
                                
                                size_t nind = current->nest_id;
                                
                                std::cout << "nest id " << current->nest_id << std::endl;
                                std::cout << "ind id " << current->ind_id << std::endl;
                                std::cout << "check 6" << std::endl;
                                // Individual<2> tempCopy = nests[current->nest_id].larval_females[index];
                                std::cout << "size before push_back" << nests[nind].larval_females.size() << std::endl;
                                nests[current->nest_id].adult_females.push_back(nests[nind].larval_females[get<2>(index)]);
                                std::cout << "size after push_back" << nests[nind].larval_females.size() << std::endl;
                                
                                std::cout << "check 7" << std::endl;
                                std::cout << "nest id " << current->nest_id << std::endl;
                                std::cout << "ind id " << current->ind_id << std::endl;
                                std::cout << "larval_female size " << nests[nind].larval_females.size() << std::endl;
                                std::cout << "larval position " << get<2>(index) << std::endl;
                                nests[nind].larval_females.erase(nests[nind].larval_females.begin() + get<2>(index));
                                
                                nests[nind].adult_females[old_afemales].t_next = gtime;
                                nests[nind].task_check(nests[nind].adult_females[old_afemales]);
                                // printIndividualInfo(nests[current->nest_id].adult_females[old_afemales]);
                                //remove_from_vec(nests[current->nest_id].larval_females, index);
                                
                                
                                std::cout << "check 8" << std::endl;
                                event_queue.push(track_time(&nests[nind].adult_females[old_afemales])); // add matured to queue
                                std::cout << "3FOR Female index: " << get<2>(index) << " BSize: " << old_bodysize
                                    << " MATURED Afems from " << old_afemales << " to " << nests[nind].adult_females.size()
                                    << " Larval fems from " << old_lfemales << " to " << nests[nind].larval_females.size() <<std::endl;
                                
                                
                                
                                
                            }
                            // nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                            // remove_from_vec(nests[current->nest_id].larval_females, index);
                        }
                        // TST
                        else std::cout << "4FOR Female index: " << get<2>(index) << " BSize: " << nests[current->nest_id].larval_females[get<2>(index)].body_size << " NO Mature" << std::endl;
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


// void Population::simulate() {
//     // Create a priority queue to track individuals by their next action time
//     std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> event_queue(cmptime);

//     // Initialize the event queue with individuals and their initial next action times
//     for (auto& nest : nests) {
//         for (auto& individual : nest.adult_females) {
//             event_queue.push(track_time(&individual));
//         }
//     }

//     // Simulate for a fixed number of events/individual updates
//     for (int event = 0; event < max_events; ++event) {
        
//         if (event_queue.empty()) {
//             // No more events to process
//             std::cout << "ERROR: event_queue empty" << std::endl;
//             break;
//         }
        
//         if (event % 10000 == 0) {
//             removeDeadMales();
//         }

//         // Get the individual with the earliest next action time
//         track_time next_event = event_queue.top();
//         event_queue.pop();
//         auto current = next_event.ind;
//         // TST // 
//         std::cout << "E: " << event << " | Ind: " << current->ind_id << " | GT: " << gtime << " | tn_bef: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << std::endl;
//         gtime = current->t_next;

//         // Last task survival check
//         if (current->is_alive) { 
//             if (current->is_foraging) {
//                 int index = nests[current->nest_id].feed(dForagingMean, dForagingSD);
//                 if (index < 0) {
//                     index = -index - 1;
//                     // edit index setting here
//                     if (nests[current->nest_id].larval_males[-index].check_mature(gtime)) {
//                         adult_males.push_back(std::move(nests[current->nest_id].larval_males[index]));
//                         // nests[current->nest_id].larval_males.erase(nests[current->nest_id].larval_males.begin() + (-index));
//                         // auto it = nests[current->nest_id].larval_males.begin() + index;
//                         // remove_from_vec(nests[current->nest_id].larval_males, it);
//                         // remove_from_vec(nests[current->nest_id].larval_males, index);
//                         nests[current->nest_id].larval_males.erase(nests[current->nest_id].larval_males.begin() + index);
//                     }
//                 } else if (index > 0) {
//                     --index;
//                     // edit index setting here
//                     if (nests[current->nest_id].larval_females[index].check_mature(gtime)) {
//                         // Need to check for task since check_task moved to nest level
//                         nests[current->nest_id].task_check(nests[current->nest_id].larval_females[index]);
//                         if (nests[current->nest_id].larval_females[index].check_disperser()) {
//                             bool mated = mate(nests[current->nest_id].larval_females[index]);
//                             if (!mated) {
//                                 continue; //LC3 :: remove this                            
//                             }
//                             int empty = update_emptyNests();
//                             if (empty > 0) {
//                                 --empty;
//                                 nests[empty].adult_females = {std::move(nests[current->nest_id].larval_females[index])};
//                                 current->nest_id = empty; // Change nest ID too
//                             }
//                         } else {
//                             bool mated = mate(nests[current->nest_id].larval_females[index]);
//                             if (!mated) {
//                                 continue; //LC3 :: remove this  
//                             }
//                             nests[current->nest_id].adult_females.push_back(std::move(nests[current->nest_id].larval_females[index]));
//                         }
//                         // nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
//                         // remove_from_vec(nests[current->nest_id].larval_females, index);
//                         // auto it = nests[current->nest_id].larval_females.begin() + index;
//                         // remove_from_vec2(nests[current->nest_id].larval_females, it);
//                         nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                            
//                     }
//                 }
//             } else {
//                 nests[current->nest_id].reproduce(*current);
//                 if(!current->is_mated) {
//                     bool dummy = mate(*current);
//                 } 
//             }
            
//             // Task check
//             nests[current->nest_id].task_check(*current);
//             current->survival();
//             // Reinsert the individual into the event queue with the new next action time
//             event_queue.push(track_time(current));
//         } else {
//             // Find the position of the current individual in its nest's adult_females vector
//             size_t ind = nests[current->nest_id].findFemaleIndexById(current->ind_id);
//             if (ind - 1) {
//                 // auto it = nests[current->nest_id].adult_females.begin() + ind - 1;
//                 nests[current->nest_id].adult_females.erase(nests[current->nest_id].larval_females.begin() + ind - 1);
//                 // remove_from_vec(nests[current->nest_id].adult_females, it);
//                 // remove_from_vec(nests[current->nest_id].adult_females, ind - 1);
//             } else {
//                 // LC3 :: Throw error, need to learn error handling
//             }
//         }
//         // TST //
//         std::cout << "     | Ind: " << current->ind_id << " | GT: " << gtime << " | tn_now: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << std::endl;
//     }
// }

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
    // std::cout << "Male index chosen: " << index - 1 << std::endl;
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
    adult_males.erase(std::remove_if(adult_males.begin(), adult_males.end(), Condition), adult_males.end()); //LC4
    // How about incorporating erase_if here itself
}

#endif /* Population_hpp */

