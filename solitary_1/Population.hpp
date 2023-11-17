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
private:
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// initialise population function
void Population::initialise_pop() {
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    Individual<2> eve;
    eve.calculate_phenotype();
    Individual<1> adam;
    adam.ind_id = IndID;
    
    eve.mate(adam); // female mated with adam
    eve.calculate_phenotype();
    for(int i=0; i < max_nests; ++i) {
        eve.nest_id = i;
        eve.ind_id = ++IndID;
        nests.emplace_back(eve);  // loops through each nest and adds mated founding female
    }
}

void Population::simulate() {
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
        event_queue.pop();
        Individual<2> current = *next_event.ind;
        
        gtime = current.t_next;

        // Last task survival check
        if (current.is_alive) { 
            if (current.is_foraging) {
                int index = nests[current.nest_id].feed(dForagingMean, dForagingSD);
                if (index < 0) {
                    index = -index - 1;
                    // edit index setting here
                    if (nests[current.nest_id].larval_males[-index].check_mature(gtime)) {
                        adult_males.push_back(std::move(nests[current.nest_id].larval_males[-index]));
                        // nests[current.nest_id].larval_males.erase(nests[current.nest_id].larval_males.begin() + (-index));
                        remove_from_vec(nests[current.nest_id].larval_males, -index);
                    }
                } else if (index > 0) {
                    --index;
                    // edit index setting here
                    if (nests[current.nest_id].larval_females[index].check_mature(gtime)) {
                        // Need to check for task since check_task moved to nest level
                        nests[current.nest_id].task_check(nests[current.nest_id].larval_females[index]);
                        if (nests[current.nest_id].larval_females[index].check_disperser()) {
                            bool mated = mate(nests[current.nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //LC3 :: remove this                            
                            }
                            int empty = update_emptyNests();
                            if (empty > 0) {
                                --empty;
                                nests[empty].adult_females = {std::move(nests[current.nest_id].larval_females[index])};
                                current.nest_id = empty; // Change nest ID too
                            }
                        } else {
                            bool mated = mate(nests[current.nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //LC3 :: remove this  
                            }
                            nests[current.nest_id].adult_females.push_back(std::move(nests[current.nest_id].larval_females[index]));
                        }
                        // nests[current.nest_id].larval_females.erase(nests[current.nest_id].larval_females.begin() + index);
                        remove_from_vec(nests[current.nest_id].larval_females, index);
                    }
                }
            } else {
                nests[current.nest_id].reproduce(current);
                if(!current.is_mated) {
                    bool dummy = mate(current);
                } 
            }
            
            // Task check
            nests[current.nest_id].task_check(current);
            
            if (current.is_foraging) {
                current.t_next += dForagingTime;
            } else {
                current.t_next += dBroodingTime; 
            }
            current.survival();
            // Reinsert the individual into the event queue with the new next action time
            event_queue.push(track_time(&current));
        } else {
            // Find the position of the current individual in its nest's adult_females vector
            size_t ind = nests[current.nest_id].findFemaleIndexById(current.ind_id);
            if (ind - 1) {
                remove_from_vec(nests[current.nest_id].adult_females, ind - 1);
            } else {
                // LC3 :: Throw error, need to learn error handling
            }
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
bool Population::mate(Individual<2>& female){
    int index = choose_RndmMale();
    if (index){
        female.mate(adult_males[index - 1]);
        female.is_mated = true;
        return true;
    }
    else {
        return false;
    }
}

// updates empty nest list and returns empty one from that
int Population::update_emptyNests() {
    empty_nests.clear();
    for (size_t i = 0; i < nests.size(); ++i) {
        if (nests[i].is_empty()) {
            empty_nests.push_back(i);
        }
    }
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
