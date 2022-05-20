#include <iostream>
#include <cstring>
#include <fstream>
#include <pthread.h>
#include <vector>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int startTarget, endTarget;
struct prime_args
{
    int l, r;
};

int numOfPrimes = 0;
int totalNums = 0;

std::vector<bool> primes;

void *count_primes(void *args)
{
    // covert args to int vars
    struct prime_args *temp = (struct prime_args *)args;

    int l = temp->l;

    // edge case, skip if l=1
    if (l == 1)
    {
        l = 2;
        pthread_mutex_lock(&mutex);
        ++totalNums;
        pthread_mutex_unlock(&mutex);
    }

    int r = temp->r;

    delete temp;

    for (int i = l; i <= r; ++i)
    {
        bool prime = true;

        // check if i is divisible by 2:sqrt(i)
        for (int j = 2; j * j <= i; ++j)
        {
            if (i % j == 0)
            {
                prime = false;
                break;
            }
        }

        pthread_mutex_lock(&mutex);
        if (prime)
        {
            primes[i - startTarget] = true;

            ++numOfPrimes;
        }

        ++totalNums;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);

    return NULL;
}

void find_primes(int l, int r, int t)
{
    pthread_t threads[t];

    // calculate the number range of each thread
    int ranges[t][2];
    for (int i = 0; i < t; ++i)
    {
        ranges[i][0] = l + ((r - l + 1) / t) * i + (i < (r - l + 1) % t ? i : (r - l + 1) % t);
        ranges[i][1] = ranges[i][0] + ((r - l + 1) / t) - !(i < (r - l + 1) % t);
    }

    for (int i = 0; i < t; ++i)
    {
        prime_args *args = new prime_args;

        args->l = ranges[i][0];
        args->r = ranges[i][1];
        if (args->r >= args->l)
            std::cout << "ThreadIndex=" << i << ", startNum=" << args->l << ", endNum=" << args->r << "\n";
        else
            std::cout << "ThreadIndex=" << i << " Not Needed\n";
        pthread_create(&threads[i], NULL, count_primes, (void *)args);
    }

    // wait for all the threads to finish
    for (int i = 0; i < t; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}

void readRange(std::string filename, int &l, int &r)
{
    std::ifstream inFile(filename);
    inFile >> l >> r;
    inFile.close();
}
void writePrimes(std::string filename)
{
    std::ofstream outFile(filename);
    outFile << "The prime numbers are:\n";
    for (int i = 0; i < (uint)primes.size(); ++i)
    {
        if (primes[i])
            outFile << i + startTarget << '\n';
    }

    outFile.close();
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

    const std::string usage = "primesFinder number_of_threads [primes_file]";
    if (args[1] == "--help" || argc == 1)
    {
        std::cout << "This program finds prime numbers (range from in.txt), usage:" << usage << "\n";
        return 0;
    }

    if (argc < 2)
    {
        std::cerr << "Invalid inputs! Please use" << usage << "\n";
        return -1;
    }

    // convert second arg to int
    int threadsCount = std::stoi(args[1]);

    // read in.txt
    readRange("in.txt", startTarget, endTarget);

    // resize primes to range
    primes.resize(endTarget - startTarget + 1);

    find_primes(startTarget, endTarget, threadsCount);

    // write output file
    if (argc > 2)
    {
        writePrimes(args[2]);
    }

    // print statistics
    std::cout << "numOfPrimes=" << numOfPrimes << ", totalNums="
              << totalNums << "\n";

    pthread_exit(NULL);
}
