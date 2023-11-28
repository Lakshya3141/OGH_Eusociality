//
//  Parameters.hpp
//  OGH_Eusociality
//
//  Created by Lakshya Chauhan on 25/09/2023.
//  Copyright © 2023 Lakshya Chauhan. All rights reserved.
//
#ifndef TimeStructDummy_hpp
#define TimeStructDummy_hpp

#include <random>
#include <chrono>
#include <array>
#include <iostream>
#include <queue>
#include "Individual.hpp"

struct track_time {
    float time;
    Individual<2>* ind;

    track_time(Individual<2>* input) {
      ind = input;
      time = ind->t_next;
    }
};

auto cmptime = [](const track_time& a,
                  const track_time& b) { return a.time > b.time; };
   
std::priority_queue< track_time, 
                     std::vector<track_time>, 
                     decltype(cmptime) > time_queue(cmptime);


auto focal_individual = time_queue.top().ind;

// update focal_individual state
// make focal_individual choose next task
// update focal_individual next time

time_queue.push(track_time(focal_individual));

#endif