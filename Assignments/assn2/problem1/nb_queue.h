#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>

using namespace std;

// forward declaration to prevent issues due to interdependence
class DataNode;

class Pointer {

public: 
    DataNode *ptr;
    int cnt;

    Pointer() : ptr(NULL), cnt(0) {}

    Pointer(DataNode *ptr, int cnt) : ptr(ptr), cnt(cnt) {}

    bool operator==(Pointer other) {
        return (ptr == other.ptr) && (cnt == other.cnt);
    }
};

class DataNode {

public:
    int value;
    atomic<Pointer> next;

    DataNode(int value) : value(value) {}
};

class Queue {

public:
    atomic<Pointer> Head;
    atomic<Pointer> Tail;

    atomic<int> pending_enq_deqs;
    atomic<int> pending_dumps;

    Queue() {
        DataNode *node = new DataNode(0);
        Head.store(Pointer(node, 0));
        Tail.store(Pointer(node, 0));

        pending_enq_deqs = pending_dumps = 0;
    }

    void enqueue(int value) {
        while (pending_dumps > 0);
        pending_enq_deqs.fetch_add(1);

        DataNode *node = new DataNode(value);
        Pointer tail;

        while (true) {
            tail = Tail;
            Pointer next = tail.ptr -> next;

            if (tail == Tail.load()) {
                if (next.ptr == NULL) {
                    if ((tail.ptr -> next).compare_exchange_strong(next, Pointer(node, next.cnt + 1)))
                        break;
                }
                else {
                    Tail.compare_exchange_strong(tail, Pointer(next.ptr, tail.cnt + 1));
                }
            }
        }

        Tail.compare_exchange_strong(tail, Pointer(node, tail.cnt + 1));

        pending_enq_deqs.fetch_sub(1);
    }

    bool dequeue(int &pvalue) {
        while (pending_dumps > 0);
        pending_enq_deqs.fetch_add(1);

        Pointer head;

        while (true) {
            head = Head;
            Pointer tail = Tail;
            Pointer next = head.ptr -> next;

            if (head == Head.load()) {
                if (head.ptr == tail.ptr) {
                    if (next.ptr == NULL) {
                        pending_enq_deqs.fetch_sub(1);
                        return false;
                    }
                    Tail.compare_exchange_strong(tail, Pointer(next.ptr, tail.cnt + 1));
                }
                else {
                    pvalue = next.ptr -> value;
                    if (Head.compare_exchange_strong(head, Pointer(next.ptr, head.cnt + 1)))
                        break;
                }
            }
        }

        free(head.ptr);
        pending_enq_deqs.fetch_sub(1);
        
        return true;
    }

    /**
     * dump queue will wait for threads who are already inside enqueue/dequeue.
     * but if a thread invokes dump_queue then threads who invoke enqueue/dequeue after dump_queue's invocation
     * will have to wait inside enqueue/dequeue till dump_queue completes
     */
    void dump_queue(mutex& print_lock) {
        pending_dumps.fetch_add(1);
        while (pending_enq_deqs > 0);

        vector<int> data;
        DataNode *curr = (Head.load().ptr -> next).load().ptr;

        while (curr != NULL) {
            data.push_back(curr -> value);
            curr = (curr -> next).load().ptr;
        }

        print_lock.lock();

        cout << "Current state of queue: ";
        for (int val : data) cout << val << " ";
        cout << endl;

        print_lock.unlock();

        pending_dumps.fetch_sub(1);
    }
};
