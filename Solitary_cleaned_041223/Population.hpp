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
#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream

// track_time struct for event queue ordered by next task time
struct track_time {
    float time;
    int nest_id;
    int ind_id;

    // event constructor
    track_time(const Individual<2>& input) {
      ind_id = input.ind_id;
      nest_id = input.nest_id;
      time = input.t_next;
    }
};

class Population {
public:
    Population(const params& par) : p(par) { };
    std::vector<Individual<1> > adult_males;        // vector for adult males to fertilize female
    std::vector<Nest> nests;
    std::vector<size_t> empty_nests;                // vector to store empty nest IDS.
    std::vector<unsigned int> storer_nest_id;       // vector to store nest IDs corresponding to nest indexes

    void initialise_pop();                          // Initialise populations
    std::tuple<bool, unsigned int> update_emptyNests(); // updates emty_nests list and returns empty nest ID
    void mate(Individual<2>& female);               // mates a female with a random male from adult_males
    std::tuple<bool, size_t> choose_RndmMale();     // function to choose random male for mating
    void removeDeadMales();                         // to remove dead males
    int findIndexByNestId(unsigned int nestId);     // returns index of Nest ID in nests vector

    void simulate_tst();                            // simulate function
    
    unsigned int nest_id_counter = 0;               // nest ID counter 
    unsigned int individual_id_counter = 0;         // individual ID counter
    
    params p;
};

auto cmptime = [](const track_time& a, const track_time& b) { return a.time > b.time; };

// initialise population function
void Population::initialise_pop() {
    // clear vectors
    adult_males.clear();
    nests.clear();
    empty_nests.clear();

    // starting with one adult male
    Individual<1> adam(individual_id_counter);
    ++individual_id_counter;
    adult_males.push_back(adam);
    
    for(int i=0; i < max_nests; ++i) {
        // initalise each nest with breeder female and mate with adam
        Individual<2> eve(individual_id_counter);
        ++individual_id_counter;
        eve.mate(adam);

        // change nest ID values in 
        eve.nest_id = nest_id_counter;
        storer_nest_id.push_back(i);
        eve.t_next = uni_real();                // impart asynchronous acting time
        eve.is_foraging = false;                // set breeder
        ++nest_id_counter;                   
        nests.emplace_back(eve, eve.nest_id);      
    }
}

// Function to find index of a particular Nest ID in nests vector
// By searching in the storer nest ID vector 
int Population::findIndexByNestId(unsigned int nestId) {
        auto it = std::find(storer_nest_id.begin(), storer_nest_id.end(), nestId);
        if (it != storer_nest_id.end()) {
            return static_cast<int>(std::distance(storer_nest_id.begin(), it));
        } else {
            return -1;  // Return -1 if nest_id is not found
        }
    }

// Simulate function for initalised population
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

        // remove dead males every check_dead_males events 
        if (event % check_dead_males == 0) {
            removeDeadMales();
        }

        // Get the individual with the earliest next action time, pop it from queue
        // find nest index (cnestid) based on event's nest ID and individual ID (cindid)
        // Find index of current indidvidual in it's respective next
        track_time next_event = event_queue.top();
        event_queue.pop();
        size_t cnestid = findIndexByNestId(next_event.nest_id);
        auto cindid = next_event.ind_id;
        auto current_index = nests[cnestid].findFemaleIndexById(cindid);

        // dummy current to hold old variable values for current individual
        auto current = nests[cnestid].adult_females[current_index];

        // Basic information
        std::cout << "\nE: " << event << " | Ind: " << cindid << " | Nid: " << next_event.nest_id << " | GT: " << gtime 
        << " | Nindex: " << cnestid <<  " | Alive_before: " << (current.is_alive ? "YES" : "NO ") 
        << " | Task_bef: " << (current.is_foraging ? "FOR":"REP") << " | Qlen: " << event_queue.size() << std::endl;
        
        // Update Gtime to current task time
        gtime = current.t_next;

        // Last task survival check
        if (current.is_alive) { // If individual survived till now
            if (current.is_foraging) {  // And old task is foraging

                // Feed random larva and also get it's gender and index
                std::tuple<bool, bool, size_t> index = nests[cnestid].feed(p.dForagingMean, p.dForagingSD);

                if (std::get<0>(index) == true) {       // If feed successful
                    if (std::get<1>(index) == false) {  // if male
                        // Check if larva matures after feeding
                        if (nests[cnestid].larval_males[std::get<2>(index)].check_mature(gtime, p)) {
                            
                            // Push mature larva to adult males vector
                            adult_males.push_back(nests[cnestid].larval_males[std::get<2>(index)]);
                            remove_from_vec(nests[cnestid].larval_males, std::get<2>(index));
                        } 
                    } else { // if female
                        // Check mature
                        if (nests[cnestid].larval_females[std::get<2>(index)].check_mature(gtime, p)) {
                            // Check if disperser
                            if (nests[cnestid].larval_females[std::get<2>(index)].check_disperser()) {
                                // If diserpser, mate larva
                                mate(nests[cnestid].larval_females[std::get<2>(index)]);

                                // Get Empty nest ID and it's index in nests vector
                                auto empty = update_emptyNests();
                                size_t empty_index = findIndexByNestId(std::get<1>(empty));
                                
                                if (std::get<0>(empty) == true) {       // If empty nest found

                                    // Push to adult females vector in empty nest and delete
                                    // from old larval vector
                                    nests[empty_index].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                    nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));
                                    
                                    // Since new colony, remove older larvae
                                    nests[empty_index].larval_females.clear();
                                    nests[empty_index].larval_males.clear();
                                    
                                    // Update t_next before task check
                                    nests[empty_index].adult_females[0].t_next = gtime;
                                    nests[empty_index].task_check(nests[empty_index].adult_females[0], p);
                                    // Update ID of new nest, new individual and storer_nest_id vector
                                    nests[empty_index].adult_females[0].nest_id = nest_id_counter;
                                    storer_nest_id[empty_index] = nest_id_counter;
                                    nests[empty_index].nest_id = nest_id_counter;
                                    nest_id_counter++;
                                    // Push mature disperser to queue
                                    event_queue.push(track_time(nests[empty_index].adult_females[0]));
                                    
                                } else {
                                    // If not disperser
                                    // erase larva
                                    nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));
                                }
                            } else { // else if not disperser
                                
                                size_t old_afemales = nests[cnestid].adult_females.size();      // Old adult females sizes
                                mate(nests[cnestid].larval_females[std::get<2>(index)]);        // Mate female on maturing

                                // Add to adult females and remove from larval females
                                nests[cnestid].adult_females.push_back(nests[cnestid].larval_females[std::get<2>(index)]);
                                nests[cnestid].larval_females.erase(nests[cnestid].larval_females.begin() + std::get<2>(index));

                                // Update gtime and check task
                                nests[cnestid].adult_females[old_afemales].t_next = gtime;
                                nests[cnestid].task_check(nests[cnestid].adult_females[old_afemales], p);
                                
                                // Push to queue
                                event_queue.push(track_time(nests[cnestid].adult_females[old_afemales]));
                            }
                        }
                    }
                }
            } else { // old task is reproducing

                // reproduce
                nests[cnestid].reproduce(nests[cnestid].adult_females[current_index], individual_id_counter, p);
                ++individual_id_counter;
            }

            // Task check for current female, followed by survival check
            nests[cnestid].task_check(nests[cnestid].adult_females[current_index], p);
            nests[cnestid].adult_females[current_index].survival(p);
            // push current individual back to queue
            event_queue.push(track_time(nests[cnestid].adult_females[current_index]));

        
        } // Death condition 
        else {
            nests[cnestid].adult_females.erase(nests[cnestid].adult_females.begin() + current_index);
        }
    }
}

// returns index of selected random male
std::tuple<bool, size_t> Population::choose_RndmMale(){
    if (adult_males.size() != 0) {
        size_t index = uni_int(0, adult_males.size()) + 1;
        return std::make_tuple(true, index);
    }
    else return std::make_tuple(false, 0);
}

// Mate female with random male if available
void Population::mate(Individual<2>& female){
    std::tuple<bool, size_t> index_tuple = choose_RndmMale();
    if (std::get<0>(index_tuple)){
        female.mate(adult_males[std::get<1>(index_tuple)]);
    }
}

// updates empty nest list and returns tuple of the form < if_empty, ID >
std::tuple<bool, unsigned int> Population::update_emptyNests() {
    empty_nests.clear();        // clear emmpt_nests vector

    for (size_t i = 0; i < nests.size(); ++i) {
        // add nest ID to vector if corrsesponding nest is empty
        if (!nests[i].adult_females.size()) {
            empty_nests.push_back(nests[i].nest_id);
        }
    }
    if (empty_nests.empty()) {
        return std::make_tuple(false, 0); // No empty nests found
    } else {
        size_t randomIndex = uni_int(0, empty_nests.size());    // random index chosen from empty_nests vector 
        return std::make_tuple(true, empty_nests[randomIndex]); // return random Nest ID that is empty
    }
}

// function to remove dead males based on t_death compared to gtime
void Population::removeDeadMales() {
    auto Condition = [](const Individual<1>& male) {
        return male.t_death <= gtime;
    };
    adult_males.erase(std::remove_if(adult_males.begin(), adult_males.end(), Condition), adult_males.end());
    // How about incorporating erase_if here itself // ??
}

// Print individual details for debugging TST
template<int Ploidy>
void printIndividualDetails(const Individual<Ploidy>& individual) {
    std::cout << "Individual ID: " << individual.ind_id << " | Nest ID: " << individual.nest_id << std::endl;
    std::cout << "Genome Values: ";
    for (int i = 0; i < Ploidy; ++i) {
        std::cout << "(" << individual.genome[i].genes_dispersal << ", " << individual.genome[i].genes_choice[0] << ", " << individual.genome[i].genes_choice[1] << ") ";
    }
    std::cout << std::endl;
    std::cout << "Phenotype Dispersal: " << individual.phenotype_dispersal << std::endl;
    std::cout << "Phenotype Choice Intercept: " << individual.phenotype_choice[0] << std::endl;
    std::cout << "Phenotype Choice Slope: " << individual.phenotype_choice[1] << std::endl;
    std::cout << "Is Alive: " << (individual.is_alive ? "true" : "false") << std::endl;
    std::cout << "Is Larvae: " << (individual.is_larvae ? "true" : "false") << std::endl;
    std::cout << "Is Foraging: " << (individual.is_foraging ? "true" : "false") << std::endl;
    std::cout << "Is Disperser: " << (individual.is_disperser ? "true" : "false") << std::endl;
    std::cout << "Is Mated: " << (individual.is_mated ? "true" : "false") << std::endl;
    std::cout << "Body Size: " << individual.body_size << std::endl;
    std::cout << "t_death: " << individual.t_death << std::endl;
    // std::cout << "Number of Offspring: " << individual.num_offspring << std::endl;
    // std::cout << "Number of Female Offspring: " << individual.num_fem_offspring << std::endl;
    // std::cout << "Number of Larvae: " << individual.num_larva << std::endl;
    // std::cout << "Number of Female Larvae: " << individual.num_female_larva << std::endl;
    std::cout << std::endl;
}

// Function to print individual information concisely for debugging TST
void printIndividualInfo(const Individual<2>& individual) {
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

#endif /* Population_hpp */

