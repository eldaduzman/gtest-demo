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

class PerformanceTestBase : public ::testing::Test
{
protected:
    std::mutex mtx;
    std::chrono::high_resolution_clock::time_point start_time;
    std::vector<std::chrono::nanoseconds> thread_durations;
    int num_iterations = 100;

    virtual void ExecuteTestLogic() = 0;
    void SetUp() override
    {
        start_time = std::chrono::high_resolution_clock::now();
        num_iterations = 100;
    }

    void TearDown() override
    {
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

        ExecuteTestLogic();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ns = end_time - start_time;

        std::lock_guard<std::mutex> lock(mtx);
        thread_durations.push_back(duration_ns);
    }
};
class TestHappyFlow : public PerformanceTestBase
{
protected:
    void ExecuteTestLogic() override
    {
        int arr[] = {1, 5, 4, 6, 7, 9, 8, 10, 2, 3};
        size_t n = sizeof(arr) / sizeof(arr[0]);
        float mean = calculate_mean(arr, n);
        EXPECT_FLOAT_EQ(mean, 5.5);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};
// class TestEmptyArr : public PerformanceTestBase
// {
// protected:
//     void ExecuteTestLogic() override
//     {
//         int arr[] = {};
//         size_t n = sizeof(arr) / sizeof(arr[0]);
//         float mean = calculate_mean(arr, n);
//         EXPECT_FLOAT_EQ(mean, 5.5);
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//     }
// };

TEST_F(TestHappyFlow, Test_Happy_Flow)
{
    const int num_threads = 4;
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
