//
//  Population.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 02/10/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.

#ifndef Population_hpp
#define Population_hpp    

#include "Nest.hpp"
#include "Random.hpp"
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
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// initialise population function
void Population::initialise_pop() {
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    
    Individual<1> adam(IndID++);
    adult_males.push_back(adam);
    double cnt = 2.0; //TST
    
    for(int i=0; i < max_nests; ++i) {
        Individual<2> eve(IndID++);
        // eve.mate(adam); // TST UNCOMMENT
        eve.nest_id = i;
        eve.genome[0].genes_dispersal = cnt; //TST
        cnt += 0.5; // TST
        Nest dumNest(i+1, eve);
        nests.push_back(dumNest);
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
        
        if (event % 10000 == 0) {
            removeDeadMales();
        }

        // Get the individual with the earliest next action time
        track_time next_event = event_queue.top();
        // std::cout << "Prepop Qlen: " << event_queue.size() << std::endl; // TST
        event_queue.pop();
        // std::cout << "Pospop Qlen: " << event_queue.size() << std::endl; // TST
        auto current = next_event.ind;
        // TST // 
        std::cout << "E: " << event << " | Ind: " << current->ind_id << " | Nid: " << current->nest_id << " | GT: " << gtime 
        << " | tn_bef: " << current->t_next <<  " | Alive_before: " << (current->is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current->is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
        // current->is_alive = false; // TST

        gtime = current->t_next;

        // Last task survival check
        if (current->is_alive) { 
            
            // TST Larval part shifted to the end
            if (current->is_foraging) {
                int index = nests[current->nest_id].feed(dForagingMean, dForagingSD);
                if (index < 0) {
                    index = -index - 1;
                    // TST
                    // edit index setting here
                    if (nests[current->nest_id].larval_males[index].check_mature(gtime)) {
                        // TST
                        int old_lmales = nests[current->nest_id].larval_males.size();
                        int old_amales = adult_males.size();
                        Individual<1> temp = std::move(nests[current->nest_id].larval_males[index]);
                        adult_males.push_back(std::move(temp));
                        // adult_males.emplace_back(std::move(nests[current->nest_id].larval_males[index]));
                        // nests[current->nest_id].larval_males.erase(nests[current->nest_id].larval_males.begin() + (-index));
                        // auto it = nests[current->nest_id].larval_males.begin() + index;
                        // remove_from_vec2(nests[current->nest_id].larval_males, it);    
                        // remove_from_vec(nests[current->nest_id].larval_males, index);
                        nests[current->nest_id].larval_males.erase(nests[current->nest_id].larval_males.begin() + index);
                        std::cout << "FOR Male index: " << index << " BSize: "<< nests[current->nest_id].larval_males[index].body_size 
                        << " MATURED Adult from " << old_amales << " to " << adult_males.size() 
                        << " Larvae From " << old_lmales << " to " << nests[current->nest_id].larval_males.size() << std::endl;
                    } 
                    // TST
                    else {
                        std::cout << "FOR Male index: " << index << " BSize: " << nests[current->nest_id].larval_males[index].body_size << " No Mature " << std::endl;
                    }
                } else if (index > 0) {
                    --index;
                    // TST
                    // edit index setting here
                    if (nests[current->nest_id].larval_females[index].check_mature(gtime)) {
                        // Need to check for task since check_task moved to nest level
                        // VIMP NEED TO CHANGE PLACEMENT OF THIS SO AS TO MAKE SURE TASK CHECK
                        // FOR DISPERSER DOESNT WORK ON MATURING NEST
                        nests[current->nest_id].task_check(nests[current->nest_id].larval_females[index]);
                        // TST MAJOR
                        // nests[current->nest_id].larval_females[index].phenotype_dispersal = 0.0;
                        // TST 
                        // std::cout << nests[current->nest_id].larval_females[index].phenotype_dispersal << std::endl;
                        if (nests[current->nest_id].larval_females[index].check_disperser()) {
                            bool mated = mate(nests[current->nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //LC3 :: remove this                            
                            }
                            int empty = update_emptyNests();
                            if (empty > 0) {
                                --empty;
                                // TST
                                int confirm_empty = nests[empty].adult_females.size();
                                nests[empty].adult_females = {std::move(nests[current->nest_id].larval_females[index])};
                                // auto it = nests[current->nest_id].larval_females.begin() + index;
                                // remove_from_vec2(nests[current->nest_id].larval_females, it);
                                nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                                //remove_from_vec(nests[current->nest_id].larval_females, index);
                                current->nest_id = empty; // Change nest ID too
                                // TST
                                std::cout << "1FOR Female index: " << index << " BSize: " << nests[current->nest_id].larval_females[index].body_size 
                                << " MATURED DISPERSER Nest From " << current->nest_id << " to " << empty 
                                << " NewAFem size from " << confirm_empty << " to " << nests[empty].adult_females.size() <<std::endl;
                            }
                            // TST
                            else {
                                int old_lfemales = nests[current->nest_id].larval_females.size();
                                std::cout << "2FOR Female index: " << index << " BSize: " << nests[current->nest_id].larval_females[index].body_size 
                                << " MATURED DISPERSER Died From " << current->nest_id << " LFemales From " 
                                << old_lfemales << " to " << nests[current->nest_id].larval_females.size() << std::endl;
                                // auto it = nests[current->nest_id].larval_females.begin() + index;
                                // remove_from_vec2(nests[current->nest_id].larval_females, it);
                                //remove_from_vec(nests[current->nest_id].larval_females, index);
                                nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                            }
                        } else {
                            std::cout << "MAT NO DISP" << std::endl;
                            // TST
                            int old_lfemales = nests[current->nest_id].larval_females.size();
                            int old_afemales = nests[current->nest_id].adult_females.size();
                            // TST
                            int old_bodysize = nests[current->nest_id].larval_females[index].body_size;
                            bool mated = mate(nests[current->nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //LC3 :: remove this  
                            }
                            // TST
                            std::cout << nests[current->nest_id].adult_females.size() << std::endl;
                            print();
                            // Individual<2> temp = std::move(nests[current->nest_id].larval_females[index]);
                            // nests[current->nest_id].adult_females.push_back(std::move(temp));
                            // nests[current->nest_id].adult_females.emplace_back(std::move(nests[current->nest_id].larval_females[index]));
                            Individual<2> tempCopy = nests[current->nest_id].larval_females[index];
                            print();
                            nests[current->nest_id].adult_females.push_back(tempCopy);
                            print();
                            std::cout << nests[current->nest_id].adult_females.size() << std::endl;
                            // ERROR BELOW!
                            // auto it = nests[current->nest_id].larval_females.begin() + index;
                            // remove_from_vec2(nests[current->nest_id].larval_females, it);
                            remove_from_vec(nests[current->nest_id].larval_females, index);
                            // nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                            print();
                            std::cout << nests[current->nest_id].adult_females.size() << std::endl;
                            // TST
                            std::cout << "3FOR Female index: " << index << " BSize: " << old_bodysize 
                                << " MATURED Afems from " << old_afemales << " to " << nests[current->nest_id].adult_females.size() 
                                << " Larval fems from " << old_lfemales << " to " << nests[current->nest_id].larval_females.size() <<std::endl;
                            
                        }
                        // nests[current->nest_id].larval_females.erase(nests[current->nest_id].larval_females.begin() + index);
                        // remove_from_vec(nests[current->nest_id].larval_females, index);
                    }
                    // TST
                    else std::cout << "4FOR Female index: " << index << " BSize: " << nests[current->nest_id].larval_females[index].body_size << " NO Mature" << std::endl;
                }
            } else {
                int num_lmale = nests[current->nest_id].larval_males.size(); // TST
                int num_lfemale = nests[current->nest_id].larval_females.size(); // TST
                int sex = nests[current->nest_id].reproduce(*current);
                if (sex) {
                    std::cout << "REP LarvFemale From " << num_lfemale <<" to " << nests[current->nest_id].larval_females.size() << std::endl; // TST
                } else {
                    std::cout << "REP LarvMale From " << num_lmale <<" to " << nests[current->nest_id].larval_males.size() << std::endl; // TST
                }
                if(!current->is_mated) {
                    bool dummy = mate(*current);
                } 
            }

            // Task check
            nests[current->nest_id].task_check(*current);
            current->survival();

            // HERE TST //
            // std::cout << "Queue: [ ";
            // std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> temp_queue(event_queue);
            // while (!temp_queue.empty()) {
            //     track_time temp_event = temp_queue.top();
            //     temp_queue.pop();
            //     std::cout << "(" << temp_event.ind->ind_id << ", " << temp_event.time << ")";
            //     if (!temp_queue.empty()) {
            //         std::cout << ", ";
            //     }
            // }
            // std::cout << " ]\n\n";

            // Reinsert the individual into the event queue with the new next action time
            event_queue.push(track_time(current));

            // printIndividualInfo(*current); // TST

            // HERE TST //
            // std::cout << "Queue: [ ";
            // std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> temp_queue1(event_queue);
            // while (!temp_queue1.empty()) {
            //     track_time temp_event = temp_queue1.top();
            //     temp_queue1.pop();
            //     std::cout << "(" << temp_event.ind->ind_id << ", " << temp_event.time << ")";
            //     if (!temp_queue1.empty()) {
            //         std::cout << ", ";
            //     }
            // }
            // std::cout << " ]\n\n";
        
        } // Death condition 
        else {
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
        if (nests[i].is_empty()) {
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

