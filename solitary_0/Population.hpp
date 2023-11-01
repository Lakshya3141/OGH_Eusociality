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
private:
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// initialise population function
void Population::initialise_pop() {
    
    // population founders (using default constructor, which calls default constructor of Haplotype)
    Individual<2> eve;
    eve.calculate_phenotype();
    Individual<1> adam;
    
    eve.mate(adam); // female mated with adam
    eve.calculate_phenotype();
    for(int i=0; i < max_nests; ++i) {
        eve.nest_id = i;
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
                    if (nests[current.nest_id].larval_males[-index].check_mature(gtime)) { //LC1: update gtime to be implemented
                        adult_males.push_back(std::move(nests[current.nest_id].larval_males[-index]));
                        nests[current.nest_id].larval_males.erase(nests[current.nest_id].larval_males.begin() + (-index));
                        // LC2: Copy the last individual and then paste it in place of middle one to be erased
                        // Jan may have a util, check it out
                    }
                } else if (index > 0) {
                    index -= 1;
                    // edit index setting here
                    if (nests[current.nest_id].larval_females[index].check_mature(gtime)) { //LC1: update gtime to be implemented
                        if (nests[current.nest_id].larval_females[index].check_disperser()) {
                            bool mated = mate(nests[current.nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //LC2: hard to decide what to do if no mates are available                            
                            }
                            int empty = update_emptyNests();
                            if (empty > 0) {
                                --empty;
                                nests[empty].adult_females = {std::move(nests[current.nest_id].larval_females[index])};
                                nests[current.nest_id].larval_females.erase(nests[current.nest_id].larval_females.begin() + index);
                            }
                            else {
                                nests[current.nest_id].larval_females.erase(nests[current.nest_id].larval_females.begin() + index);
                            }

                        } else {
                            bool mated = mate(nests[current.nest_id].larval_females[index]);
                            if (!mated) {
                                continue;
                            }
                            nests[current.nest_id].adult_females.push_back(std::move(nests[current.nest_id].larval_females[index]));
                            nests[current.nest_id].larval_females.erase(nests[current.nest_id].larval_females.begin() + index);
                        }
                    }
                }
            }
            
            // Task check
            current.task_check();
            
            if (current.is_foraging) {
                current.t_next += dForagingTime; 
            } else {
                nests[current.nest_id].reproduce(current); //LC2: Change the order, this needs to happen at the end of task too, not initially
                // LC2: And here, at the same time check for mated
                current.t_next += dBroodingTime; 
            }
            current.survival();
            // Reinsert the individual into the event queue with the new next action time
            event_queue.push(track_time(&current));
        } else {
            // Find the position of the current individual in its nest's adult_females vector
            auto it = std::find_if(nests[current.nest_id].adult_females.begin(), nests[current.nest_id].adult_females.end(), [&current](const Individual<2>& ind) {
                return &ind == &current;
            });
            if (it != nests[current.nest_id].adult_females.end()) {                
                nests[current.nest_id].adult_females.erase(it);
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
#endif /* Population_hpp */
