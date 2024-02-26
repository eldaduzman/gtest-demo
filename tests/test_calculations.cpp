#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
extern "C"
{
#include "../modules/calculations.h"
}


class PerformanceTest : public ::testing::Test
{
protected:
    std::mutex mtx; // Mutex for synchronized access to shared data
    std::chrono::high_resolution_clock::time_point start_time;
    int num_iterations; // Number of test iterations per thread (set in SetUp)
    // std::vector<std::chrono::microseconds> thread_durations;
    std::vector<std::chrono::nanoseconds> thread_durations;

    void SetUp() override
    {
        start_time = std::chrono::high_resolution_clock::now();
        num_iterations = 100 /* Set desired number of iterations */; // Replace with actual value
    }

    void TearDown() override
    {
        // Calculate and print the median duration
        std::sort(thread_durations.begin(), thread_durations.end());
        size_t mid_index = thread_durations.size() / 2;
        std::chrono::nanoseconds median_duration_cron = thread_durations[mid_index];
        if (thread_durations.size() % 2 == 1)
        {
            median_duration_cron = (thread_durations[mid_index - 1] + thread_durations[mid_index + 1]) / 2;
        }
        long long median_duration = median_duration_cron.count() / 1000000;
        std::cout << "Median thread execution duration: " << median_duration << " milliseconds" << std::endl;
        EXPECT_LE(median_duration, 1000);
    }

    void RunTest()
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        int arr[] = {1, 5, 4, 6, 7, 9, 8, 10, 2, 3};
        size_t n = sizeof(arr) / sizeof(arr[0]);
        float mean = calculate_mean(arr, n);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_FLOAT_EQ(mean, 5.5);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ns = end_time - start_time;
        // auto duration_us_count = std::chrono::duration_cast<long long>(duration_ns).count() / 1000.0; // Convert to microseconds using division

        // Acquire mutex before pushing to the vector (optional if thread-safe)
        std::lock_guard<std::mutex> lock(mtx);
        thread_durations.push_back(duration_ns);
    }
};

TEST_F(PerformanceTest, MultiThreadedTest)
{
    const int num_threads = 4; // Number of threads

    // Create and start threads
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([this]
                             {
      for (int j = 0; j < num_iterations; ++j) {
        RunTest(); // Execute the test logic
      } });
    }

    // Wait for all threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }
}
