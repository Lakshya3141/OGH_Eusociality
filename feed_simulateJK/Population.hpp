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
     std::vector<size_t> empty_nests; //JK: I guess this will contain vector indices for the empty nests? Then I would make it a size_t, not int

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
    std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> event_queue(cmptime); //JK: changed to event_queue here and every else

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
            std::cout << "ERROR: event_queue empty" << std::endl; //JK: added error message
            break;
        }

        // Get the individual with the earliest next action time
        track_time next_event = event_queue.top();
        event_queue.pop();
        Individual<2> current = *next_event.ind;
        
        // Survival check 
        if (current.survival()){ //JK: is this a check whether it survived the previous task or whether it will survive the next one? because that might be task-dependent and we do not know the next task yet
            //JK: first we need to see whether the individual WAS foraging because then a larvae gets food.
            if (current.is_foraging) {
                int index = nests[current.nest_id].feed(dForagingMean, dForagingSD);
                if (index < 0) {
                    if (nests[current.nest_id].larval_males[-index].check_mature(time)) { //JK: I added "time" here as a place holder. we need to somwhere keep track of global time and update it whenever an individual is updated. so this needs to happen in this function
                        adult_males.push_back(std::move(nests[current.nest_id].larval_males[-index]));
                        nests[current.nest_id].larval_males.erase(nests[current.nest_id].larval_males.begin() + (-index));
                    }
                } else {
                    if (nests[current.nest_id].larval_females[index].check_mature(time)) {
                        if (nests[current.nest_id].larval_females[index].check_disperser()) {
                            bool mated = mate(nests[current.nest_id].larval_females[index]);
                            if (!mated) {
                                continue; //JK: hard to decide what to do if no mates are available. we need to think about this / discuss this
                            }
                            disperser_females.push_back(std::move(nests[current.nest_id].larval_females[index]));
                            nests[current.nest_id].larval_females.erase(nests[current.nest_id].larval_females.begin() + index);
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
            (void)current.task_check(); //JK: I would not use these conversions (void). It seems that you can just make task_check a void function in the first place. It does not have to return a bool

            // Do foraging if so
            // If forage, then get resources, transfer to larvae, note larvae index and pass back
            // and then check for maturation and gender
            // if male mature, pass it to adult males vector
            // if female mature, check dispersal. If dispersal add to disperser, otherwiser to adult
            // int cur_nest = current.nest_id;

            if (current.is_foraging) {
                current.t_next += dForagingTime; //JK: update time
                //current.forage();
                // cur_nest.feed(current);
                // int index = nests[current.nest_id].feed();
                // nests[current.nest_id].feed()
                //int index = 5; // CORRECT ERROR
                // feed() returns integer. If 0, no larvae fed. If negative, |index| - 1 is position of female larva
                // If positive, |index| - 1 is position of male larva
                //JK: can move all of this up. all of this is independent of the current individual's current task. it just depends on whether it was foraging previously, thus came back with resources and fed a larva
                //if (index < 0) {
                    //int flarv_pos =  -index - 1;
                    //bool birth = nests[cur_nest].larval_females[flarv_pos].check_mature(current.t_next);
                    //bool disperser = nests[cur_nest].larval_females[flarv_pos].check_disperser();
                    // FEMALE LARVA NEEDS TO BREED TOO
                    //if (birth && disperser) {
                        // mates, then disperses, look for empty nests
                        // if can't mate, dies
                        //bool mated = mate(nests[cur_nest].larval_females[flarv_pos]);
                        //if (!mated) {
                           // continue;
                        //}
                        //disperser_females.push_back(std::move(nests[cur_nest].larval_females[flarv_pos]));
                        //nests[cur_nest].larval_females.erase(nests[cur_nest].larval_females.begin() + flarv_pos);
                        // NEED TO CHECK FOR EMPTY NESTS NOW
                    //}
                    //else if (birth) {
                        // mates, and if can't, dies
                        //bool mated = mate(nests[cur_nest].larval_females[flarv_pos]);
                        //if (!mated) {
                           // continue;
                      //  }
                        //nests[cur_nest].adult_females.push_back(std::move(nests[cur_nest].larval_females[flarv_pos]));
                       // nests[cur_nest].larval_females.erase(nests[cur_nest].larval_females.begin() + flarv_pos);
                   // }
             //   }
                //if (index > 0) {
                   // int mlarv_pos = index - 1;
                   // bool birth = nests[cur_nest].larval_males[mlarv_pos].check_mature(current.t_next);
                   // if (birth) {
                       // adult_males.push_back(std::move(nests[cur_nest].larval_males[mlarv_pos]));
                       // nests[cur_nest].larval_males.erase(nests[cur_nest].larval_males.begin() + mlarv_pos);
                    //}
                //}
            }
            // if reproduce call nest level function
            // that's all?
            else {
                nests[current.nest_id].reproduce(current);
                current.t_next += dBroodingTime; //JK: update time for next event
            }
        }
        
        // Reinsert the individual into the event queue with the new next action time
        event_queue.push(track_time(&current));
    }
}

// returns index + 1 of selected male
int Population::choose_RndmMale(){
    if (adult_males.size() != 0) {
        size_t index = uni_int(0, adult_males.size()) + 1; //JK: changed this to a size_t
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
