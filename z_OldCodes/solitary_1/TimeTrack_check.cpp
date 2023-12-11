#include "Individual.hpp"
#include <iostream>
#include <queue>
#include "Population.hpp"

int main() {
    // Create an artificial list of females
    std::vector<Individual<2>> females;
    for (int i = 0; i < 5; ++i) {
        Individual<2> female(i + 1); // Using default constructor with ID i + 1
        female.t_next = static_cast<double>(i); // Assigning increasing t_next values
        females.push_back(female);
    }

    // Create a priority queue of track_time using cmptime
    std::priority_queue<track_time, std::vector<track_time>, decltype(cmptime)> event_queue(cmptime);

    // Add females to the priority queue
    for (auto& female : females) {
        event_queue.push(track_time(&female));
    }

    int cnt = 5;
    std::cout << "Prepop Qlen: " << event_queue.size() << std::endl;
    // Pop the queue and print t_next values
    while (!event_queue.empty()) {
        track_time next_event = event_queue.top();
        event_queue.pop();
        cnt--;
        if (cnt == 3){
            Individual<2> female(6); // Using default constructor with ID i + 1
            female.t_next = static_cast<double>(7); // Assigning increasing t_next values
            females.push_back(female);
            Individual<2> female2(7); // Using default constructor with ID i + 1
            female2.t_next = static_cast<double>(2.5); // Assigning increasing t_next values
            females.push_back(female);
            females.push_back(female2);
            event_queue.push(track_time(&female));
            event_queue.push(track_time(&female2));
        }
        std::cout << "t_next: " << next_event.time << " for Individual with ID: " << next_event.ind->ind_id << "\n";
        std::cout << "Pospop Qlen: " << event_queue.size() << std::endl;
    }

    return 0;
}
