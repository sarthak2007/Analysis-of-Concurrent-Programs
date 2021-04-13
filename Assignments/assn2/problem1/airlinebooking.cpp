// How to compile: g++ airlinebooking.cpp -pthread -latomic

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "airlinebooking.h"
#include "nb_queue.h"

using namespace std;

Queue q;
mutex print_lock;
atomic<int> seats_available(SEATS);

/**
 * USAGE of dump_queue => q.dump_queue(print_lock);
 */

void enqueue_seats() {
    for (int seat = 1; seat <= SEATS; seat++) {
        q.enqueue(seat);

        // q.dump_queue(print_lock);
    }
}

void book_seats(int customer_id) {
    // use different seeds for different threads
    srand(time(0) * customer_id);

    // generate random number = seats to book
    int num_bookings_todo = rand() % 4 + 1;
    int num_bookings_done = 0;

    vector<int> seats_booked;

    // retry until booking is complete or there are no available seats
    while (num_bookings_done != num_bookings_todo && seats_available > 0) {
        int seat_num;
        if (q.dequeue(seat_num)) {

            // q.dump_queue(print_lock);

            num_bookings_done++;
            seats_available.fetch_sub(1);
            seats_booked.push_back(seat_num);
        }
    }

    print_lock.lock();
    cout << "Customer with ID " << customer_id << " has booked " << num_bookings_done << " seats ";
    if (num_bookings_done != num_bookings_todo) cout << "out of " << num_bookings_todo << " ";
    cout << "with seat numbers ";

    for (int i = 0; i < seats_booked.size(); i++) cout << seats_booked[i] << ","[i == seats_booked.size() - 1];
    cout << endl;
    
    print_lock.unlock();
}

int main() {

    // create producer thread
    thread producer(enqueue_seats);

    // create customer threads
    vector<thread> customers(CUSTOMERS);
    for (int i = 0; i < CUSTOMERS; i++)
        customers[i] = thread(book_seats, i + 1);

    // join all the threads to the main thread
    producer.join();
    for (thread& customer : customers)
        customer.join();

    return 0;
}
