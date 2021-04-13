#include <iostream>
#include <mutex>
#include <vector>
#include "hashtable.h"

using namespace std;

class DataNode {

public:
    string key;
    string value;
    DataNode* next;

    DataNode() : key(""), value(""), next(NULL) {}

    DataNode(string key, string value, DataNode* next) : key(key), value(value), next(next) {}
};

class HashTable {

public:
    // store heads and tails of the corresponding linked lists
    vector<DataNode*> heads;
    vector<DataNode*> tails;

    // 1 lock for each index and 1 lock for printing
    mutex index_locks[SIZE];
    mutex print_lock;

    HashTable() {
        heads.assign(SIZE, NULL);
        tails.assign(SIZE, NULL);

        for (int i = 0; i < SIZE; i++) {
            DataNode *dummynode = new DataNode();

            heads[i] = tails[i] = dummynode;
        }
    }

    int insert(string word, string sentence) {
        int index = hashcode(word) % SIZE;

        index_locks[index].lock();

        DataNode *newnode = new DataNode(word, sentence, NULL);
        tails[index] -> next = newnode;
        tails[index] = newnode;

        index_locks[index].unlock();

        return 0;
    }

    int find(string word) {
        int index = hashcode(word) % SIZE;

        index_locks[index].lock();

        vector<string> sentences_with_given_word;
        DataNode *curr = heads[index] -> next;
        
        while (curr != NULL) {
            string sentence = curr -> value;
            string key = curr -> key;

            if (key == word) {
                sentences_with_given_word.push_back(sentence);
            }

            curr = curr -> next;
        }

        if (sentences_with_given_word.size() == 0) {
            index_locks[index].unlock();

            print_lock.lock();
            cout << ENOWORD << endl;
            print_lock.unlock();

            return ENOWORD;
        }

        print_lock.lock();

        for (string sentence : sentences_with_given_word)
            cout << sentence << endl;

        print_lock.unlock();

        index_locks[index].unlock();

        return 0;
    }

    int remove(string word) {
        int index = hashcode(word) % SIZE;

        index_locks[index].lock();

        bool removed_any = false;
        DataNode *prev = heads[index];
        DataNode *curr = prev -> next;
        
        while (curr != NULL) {
            string key = curr -> key;
            if (key == word) {
                if (!removed_any) removed_any = true;
                prev -> next = curr -> next;
                delete curr;
            }
            else {
                prev = curr;
            }
            curr = prev -> next;
        }
        tails[index] = prev;
        
        if (removed_any == false) {
            index_locks[index].unlock();

            print_lock.lock();
            cout << ENOWORD << endl;
            print_lock.unlock();
            
            return ENOWORD;
        }

        index_locks[index].unlock();

        return 0;
    }


};
