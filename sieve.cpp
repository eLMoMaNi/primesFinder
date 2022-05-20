//This file is deprecated and not used!

#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <pthread.h>

struct alignas(64) alignedInt
{
    int val;
};

struct prime_args
{
    int thIdx, p, l, r;
};

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// int totalNums;

std::vector<alignedInt> notPrimesCount;

std::vector<bool> notPrimes;

int startTarget, endTarget;

void *remove_composites(void *args)
{
    // covert args to int vars
    struct prime_args *temp = (struct prime_args *)args;
    int thIdx = temp->thIdx;
    int p = temp->p;
    int l = temp->l;
    int r = temp->r;

    delete temp;

    // start from first multiple of p after l
    int initial = l - l % p + (l % p == 0 ? 0 : p);

    for (int i = initial; i <= r; i += p)
    {
        // skip the prime itself (prime*1)
        if (i == initial && thIdx == 0)
        {
            continue;
        }

        // if non-prime is still defined as prime
        if (!notPrimes[i] && (i >= startTarget && i <= endTarget))
            ++(notPrimesCount[thIdx].val);
        notPrimes[i] = true;
    }

    pthread_exit(NULL);

    return NULL;
}

void find_primes(int n, int t)
{
    pthread_t threads[t];

    // memset(&primes[0], 1, primes.size() * sizeof primes[0]);
    // std::fill(primes.begin(), primes.end(), 1);
    // memset(primes, true, sizeof(primes));

    // set 0 and 1 as non prime since we will start from 2
    notPrimes[0] = notPrimes[1] = true;
    // 0,1 are not primes
    (startTarget == 0) ? notPrimesCount[0].val += 2 : (startTarget == 1) ? ++notPrimesCount[0].val
                                                                         : NULL;

    // calculate the number range of each thread
    int ranges[t][2];
    for (int i = 0; i < t; ++i)
    {
        ranges[i][0] = (n / t) * i + (i < n % t ? i : n % t) + 1;
        ranges[i][1] = ranges[i][0] + (n / t) - !(i < n % t);
    }

    for (int i = 2; i * i <= n; ++i)
    {
        if (!notPrimes[i])
        {
            for (int j = 0; j < t; ++j)
            {
                prime_args *args = new prime_args;
                args->thIdx = j;
                args->p = i;
                args->l = ranges[j][0];
                args->r = ranges[j][1];

                pthread_create(&threads[j], NULL, remove_composites, (void *)args);
            }

            // wait for all the threads to finish
            for (int j = 0; j < t; ++j)
            {
                pthread_join(threads[j], NULL);
            }

            //++numOfPrimes;
            notPrimes[i] = false;
        }
    }
}

int main(int argc, char **argv)
{

    // convert 2d char arr to string arr
    std::string args[argc];
    for (int i = 0; i < argc; i++)
    {
        const std::string x(argv[i]);
        args[i] = x;
    }

    const std::string usage = "./main number_of_threads [primes_file]";
    if (args[1] == "--help" || argc == 1)
    {
        std::cout << "This program find prime numbers (range from in.txt), usage:" << usage << "\n";
        return 0;
    }

    if (argc < 2)
    {
        std::cerr << "Invalid inputs! Please use" << usage << "\n";
        return -1;
    }

    // convert second arg to int
    int threadsCount = std::stoi(args[1]);

    // resize vector to match threadsCount
    notPrimesCount.resize(threadsCount);

    // read in.txt
    std::ifstream inFile("in.txt");

    inFile >> startTarget >> endTarget;
    inFile.close();

    // resize primes
    notPrimes.resize(1e8 + 1);

    find_primes(endTarget, threadsCount);

    // write output file
    if (argc > 2)
    {
        std::ofstream outFile(args[2]);
        for (int i = startTarget; i <= endTarget; i++)
        {
            if (!notPrimes[i])
            {
                outFile << i << "\n";
            }
        }

        outFile.close();
    }

    int numOfPrimes = endTarget - startTarget + 1;

    for (int i = 0; i < threadsCount; i++)
    {
        numOfPrimes -= notPrimesCount[i].val;
    }

    std::cout << numOfPrimes;

    pthread_exit(NULL);
}
