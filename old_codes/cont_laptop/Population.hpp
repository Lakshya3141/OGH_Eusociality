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

class Population {
public:
    Population() = default;

     std::vector<Individual<1> > adult_males;       // vector for males to fertilize female
     std::vector<Individual<2> > disperser_females; 
     std::vector<Nest> nests;   
     std::vector<int> empty_nests;

     void initialise_pop();  
     void simulate();  
     int choose_RndmMale();   
     bool mate(Individual<2>& female);          
private:
};

// track_time struct
struct track_time {
    float time;
    Individual<2>* ind;

    track_time(Individual<2>* input) {
      ind = input;
      time = ind->t_next;
    }
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
    std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> event_qeue(cmptime);

    // Initialize the event queue with individuals and their initial next action times
    for (auto& nest : nests) {
        for (auto& individual : nest.adult_females) {
            event_qeue.push(track_time(&individual));
        }
    }

    // Simulate for a fixed number of events/individual updates
    for (int event = 0; event < max_events; ++event) {
        if (event_qeue.empty()) {
            // No more events to process
            break;
        }

        // Get the individual with the earliest next action time
        track_time next_event = event_qeue.top();
        event_qeue.pop();
        Individual<2> current = *next_event.ind;
        
        // Survival check 
        if (current.survival()){
            // Task check
            (void)current.task_check();
            // Do foraging if so
            // If forage, then get resources, transfer to larvae, note larvae index and pass back
            // and then check for maturation and gender
            // if male mature, pass it to adult males vector
            // if female mature, check dispersal. If dispersal add to disperser, otherwiser to adult
            int cur_nest = current.nest_id;

            if (current.is_foraging) {
                current.forage();
                // cur_nest.feed(current);
                // int index = nests[current.nest_id].feed();
                // nests[current.nest_id].feed()
                int index = 5; // CORRECT ERROR
                // feed() returns integer. If 0, no larvae fed. If negative, |index| - 1 is position of female larva
                // If positive, |index| - 1 is position of male larva
                if (index < 0) {
                    int flarv_pos =  -index - 1;
                    bool birth = nests[cur_nest].larval_females[flarv_pos].check_mature(current.t_next);
                    bool disperser = nests[cur_nest].larval_females[flarv_pos].check_disperser();
                    // FEMALE LARVA NEEDS TO BREED TOO
                    if (birth && disperser) {
                        // mates, then disperses, look for empty nests
                        // if can't mate, dies
                        bool mated = mate(nests[cur_nest].larval_females[flarv_pos]);
                        if (!mated) {
                            continue;
                        }
                        disperser_females.push_back(std::move(nests[cur_nest].larval_females[flarv_pos]));
                        nests[cur_nest].larval_females.erase(nests[cur_nest].larval_females.begin() + flarv_pos);
                        // NEED TO CHECK FOR EMPTY NESTS NOW
                    }
                    else if (birth) {
                        // mates, and if can't, dies
                        bool mated = mate(nests[cur_nest].larval_females[flarv_pos]);
                        if (!mated) {
                            continue;
                        }
                        nests[cur_nest].adult_females.push_back(std::move(nests[cur_nest].larval_females[flarv_pos]));
                        nests[cur_nest].larval_females.erase(nests[cur_nest].larval_females.begin() + flarv_pos);
                    }
                }
                if (index > 0) {
                    int mlarv_pos = index - 1;
                    bool birth = nests[cur_nest].larval_males[mlarv_pos].check_mature(current.t_next);
                    if (birth) {
                        adult_males.push_back(std::move(nests[cur_nest].larval_males[mlarv_pos]));
                        nests[cur_nest].larval_males.erase(nests[cur_nest].larval_males.begin() + mlarv_pos);
                    }
                }
            }
            else {
                nests[cur_nest].reproduce(current);
            }
        }
        
        // Reinsert the individual into the event queue with the new next action time
        event_qeue.push(track_time(&current));
    }
}

// returns index + 1 of selected male
int Population::choose_RndmMale(){
    if (adult_males.size() != 0) {
        int index = uni_int(0, adult_males.size()) + 1;
        return index;
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
#endif /* Population_hpp */