#include <iostream>
#include <mutex>
#include <atomic>
#include <vector>
#include <algorithm>
#include "hashtable.h"

using namespace std;

// forward declaration to prevent issues due to interdependence
class DataNode;

// Pointer data structure similar to the one in problem1 to help in CAS
class Pointer {

public: 
    DataNode *ptr;
    int cnt;
    bool marked;

    Pointer() : ptr(NULL), cnt(0), marked(false) {}

    Pointer(DataNode *ptr, int cnt, bool marked) : ptr(ptr), cnt(cnt), marked(marked) {}

    bool operator==(Pointer other) {
        return (ptr == other.ptr) && (cnt == other.cnt) && (marked == other.marked);
    }
};

// Actual node of the linked list
class DataNode {

public:
    string key;
    string value;
    atomic<Pointer> next;

    DataNode() : key(""), value("") {}

    DataNode(string key, string value) : key(key), value(value) {}
};

class HashTable {

public:
    // store heads of the corresponding linked lists
    vector<Pointer> heads;

    // lock for printing
    mutex print_lock;

    HashTable() {
        heads.reserve(SIZE);

        for (int i = 0; i < SIZE; i++) {
            DataNode *dummynode1 = new DataNode();
            DataNode *dummynode2 = new DataNode();

            (dummynode1 -> next).store(Pointer(dummynode2, 0, false));

            heads[i] = Pointer(dummynode1, 0, false);
        }
    }

    // remove the nodes whose mark bit is set
    void remove_marked_nodes(int index) {

        Pointer pred;
        Pointer curr, next;
        DataNode *prednode;
        bool retry;
        
        while (true) {
            retry = false;
            pred = heads[index].ptr -> next;
            prednode = heads[index].ptr;
            curr = pred.ptr -> next;

            while (true) {
                if (curr.ptr == NULL) return;

                while (curr.marked) {
                    if (curr.ptr == NULL) return;

                    bool snip = (prednode -> next).compare_exchange_strong(pred,
                        Pointer(curr.ptr, pred.cnt + 1, false));
                    
                    if (!snip) {
                        retry = true;
                        break;
                    }
                    
                    curr = curr.ptr -> next;
                }
                if (retry) break;

                prednode = pred.ptr;
                pred = curr;
                if (curr.ptr == NULL) return;
                curr = curr.ptr -> next;
            }

            if (retry) continue;
        }
    }

    int insert(string word, string sentence) {
        int index = hashcode(word) % SIZE;

        DataNode *newnode = new DataNode(word, sentence);
        Pointer curr;
        Pointer next;

        while (true) {
            curr = heads[index].ptr -> next;
            newnode -> next = Pointer(curr.ptr, 0, false);

            bool snip = (heads[index].ptr -> next).compare_exchange_strong(curr,
                Pointer(newnode, curr.cnt + 1, false));
            if (snip) break;
        }

        remove_marked_nodes(index);

        return 0;
    }

    int find(string word) {
        int index = hashcode(word) % SIZE;

        bool first_occurence = true;

        vector<string> sentences_with_given_word;
        Pointer curr = heads[index].ptr -> next;
        Pointer next;
        DataNode *first_word_node;
        
        while (curr.ptr != NULL) {
            next = curr.ptr -> next;
            string sentence = curr.ptr -> value;
            string key = curr.ptr -> key;

            if (key == word) {
                if (first_occurence) {
                    if (next.marked) {
                        print_lock.lock();
                        cout << ENOWORD << endl;
                        print_lock.unlock();

                        return ENOWORD;
                    }
                    else {
                        first_occurence = false;
                        first_word_node = curr.ptr;
                    }
                }
                if (!first_occurence) {
                    if (next.marked) break;
                    sentences_with_given_word.push_back(sentence);
                }
            }

            curr = curr.ptr -> next;
        }

        // print_lock.lock();
        // cout << "Marked bit << " << first_word_node -> value << " " << (first_word_node -> next).load().marked << '\n';
        // print_lock.unlock();

        if (sentences_with_given_word.size() == 0 || (first_word_node -> next).load().marked) {

            print_lock.lock();
            cout << ENOWORD << endl;
            print_lock.unlock();

            return ENOWORD;
        }

        print_lock.lock();

        reverse(sentences_with_given_word.begin(), sentences_with_given_word.end());
        cout << endl;
        for (string sentence : sentences_with_given_word)
            cout << sentence << endl;
        cout << endl;

        print_lock.unlock();


        return 0;
    }

    int remove(string word) {
        int index = hashcode(word) % SIZE;

        bool removed_any = false;
        Pointer curr = heads[index].ptr -> next;
        Pointer next;
        
        while (curr.ptr != NULL) {
            next = curr.ptr -> next;
            string key = curr.ptr -> key;

            if (key == word) {
                while (true) {
                    next = curr.ptr -> next;
                    if (next.marked) {
                        print_lock.lock();
                        cout << ENOWORD << endl;
                        print_lock.unlock();

                        return ENOWORD;
                    }
                    else {
                        bool snip = (curr.ptr -> next).compare_exchange_strong(next,
                            Pointer(next.ptr, next.cnt + 1, true));
                        if (!snip) continue;
                        else {
                            removed_any = true;
                            break;
                        }
                    }
                }
            }

            curr = curr.ptr -> next;
        }
        
        if (removed_any == false) {

            print_lock.lock();
            cout << ENOWORD << endl;
            print_lock.unlock();
            
            return ENOWORD;
        }


        return 0;
    }


};
