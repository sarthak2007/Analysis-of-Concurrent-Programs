#include <string>

using namespace std;

#define SIZE (1 << 12)
#define MAX_KEY (1 << 5)
#define MAX_VALUE (1 << 8)
#define THREADS 1

// error numbers
#define ENOSPACE -1
#define ENOWORD -2

unsigned long hashcode(string str) {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}
