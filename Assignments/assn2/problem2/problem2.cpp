// How to compile: g++ problem2.cpp -pthread -latomic

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "blocking_hashtable.h"
// #include "nonblocking_hashtable.h"

using namespace std;

HashTable hashtable;

// read the file corresponding to the given thread id
void read_file(int tid, vector<vector<string>> &data) {
    string filename = "input" + to_string(tid) + ".txt";
    ifstream infile(filename);

    string line;
    while (getline(infile, line)) {
        stringstream ss(line);

        vector<string> splitted_line;
        while (ss.good()) {
            string section;
            getline(ss, section, ',');
            splitted_line.push_back(section);
        }

        data.push_back(splitted_line);
    }

    infile.close();
}

void do_work(int tid) {
    // read data from file corresponding to current thread
    vector<vector<string>> data;
    read_file(tid, data);

    for (auto operation_data : data) {
        string word = operation_data[1];

        if (operation_data[0] == "I") {
            string sentence = operation_data[2];

            hashtable.insert(word, sentence);
        }
        else if (operation_data[0] == "R") {
            hashtable.remove(word);
        }
        else if (operation_data[0] == "F") {
            hashtable.find(word);
        }
    }

}

int main() {

    // create threads
    vector<thread> threads(THREADS);
    for (int i = 0; i < THREADS; i++)
        threads[i] = thread(do_work, i + 1);

    // join all the threads to the main thread
    for (thread& t : threads)
        t.join();
    
    return 0;
}
