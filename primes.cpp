#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <algorithm>

using namespace std;

string getTime() {
    auto nowTime = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(nowTime);
    return ctime(&currentTime);
}

// ----- NO THREADS -----
vector<unsigned int> primesVectNoThreads;

void getPrimes(int up_to) {
    primesVectNoThreads.push_back(2);
    for(int x = 1; x < up_to; x+=2) {
        for(int y = 2; y < x; y++) {
            if (x % y == 0)
                break;
            if (y + 1 == x)
                primesVectNoThreads.push_back(x);
        }
    }
}


// ----- THREADS -----

// Protect writing to vector
std::mutex vectLock;
vector<unsigned int> primesVectThreads;

void findPrimes(unsigned int start, unsigned int end) {
    for(unsigned int x = start; x < end; x++) {
        for(unsigned y = 2; y < x; y++) {
            if(x % y == 0)
                break;
            if(y + 1 == x) {
                vectLock.lock();
                primesVectThreads.push_back(x);
                vectLock.unlock();
            }
        }
    }
}

void getPrimesWithThreads(unsigned int start,
        unsigned int end,
        unsigned int numThreads) {

    vector<thread> threadVect;

    unsigned int primesPerThread = end / numThreads;
    unsigned int newEnd = start + primesPerThread - 1;

    primesVectThreads.push_back(2);
    for(unsigned int i = 0; i < numThreads; i++) {
        threadVect.emplace_back(findPrimes, start, newEnd);
        start += primesPerThread;
        newEnd += primesPerThread;
    }

    for(auto& t : threadVect) {
        t.join();
    }
}

int main() {
    // NO THREADS
    clock_t t = clock();
    getPrimes(300000);
    t = clock() - t;

    for(auto x : primesVectNoThreads) {
        cout << x << endl;
    }
    cout << "Execution time: " << (float)t / CLOCKS_PER_SEC << endl;
    cout << "# of primes: " << primesVectNoThreads.size() << endl;

    // THREADS
    t = clock();
    getPrimesWithThreads(0, 300000, 20);
    t = clock() - t;

    sort(primesVectThreads.begin(), primesVectThreads.end());
    for(auto x : primesVectThreads) {
        cout << x << endl;
    }
    cout << "Execution time: " << (float)t / CLOCKS_PER_SEC << endl;
    cout << "# of primes: " << primesVectThreads.size() << endl;
    return 0;
}
