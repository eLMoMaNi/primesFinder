#include <iostream>
#include <thread>
#include <vector>

// may return 0 when not able to detect
uint lcores = std::thread::hardware_concurrency();

const int N = 3; // how many time to evaluate on each number of threads

int main(int argc, char **argv)
{
    std::string executable = "primeFinder";
    if (argc >= 2)
    {
        executable = argv[1];
    }

    if (lcores == 0)
    {
        std::cout << "Unable to detect number of cores/hw_threads, assuming 8\n";
        lcores = 8;
    }

    std::vector<float> avgs(lcores * 2);

    for (int i = 1; i <= lcores * 2; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // start ticing
            auto start = std::chrono::steady_clock::now();
            // std::to_string(i)
            const std::string command = "./" + executable + " " + std::to_string(i);
            const char *constCommand = command.c_str();
            std::system(constCommand);

            auto end = std::chrono::steady_clock::now();

            // Store the time difference between start and end
            auto diff = end - start;

            // get time in ms
            auto time = std::chrono::duration<double, std::milli>(diff).count();
            avgs[i] += time / N;
        }
    }

    for (int i = 1; i <= lcores * 2; i++)
    {
        std::cout << "Avg time on " << i << "-threads =" << avgs[i] << "\n";
    }
}
