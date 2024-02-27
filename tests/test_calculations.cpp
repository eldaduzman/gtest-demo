#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <numeric>

extern "C"
{
#include "../modules/calculations.h"
}

class PerformanceTestBase : public ::testing::Test
{
protected:
    std::mutex mtx;
    std::chrono::high_resolution_clock::time_point start_time;
    std::vector<float> thread_durations;
    int num_iterations = 0;
    bool sorted = false;

    virtual void ExecuteTestLogic() = 0;
    void SetUp() override
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    float calculate_duration_percentile(int percentile)
    {
        if (!sorted)
        {
            sorted = true;
            std::sort(thread_durations.begin(), thread_durations.end());
        }
        float percentile_index = float(percentile) / 100.0;
        size_t sorted_array_index = thread_durations.size() * percentile_index;
        float indexed_duration_cron = thread_durations[sorted_array_index];
        float indexed_duration_milliseconds = (float)(indexed_duration_cron);
        return indexed_duration_milliseconds;
    }

    float calculate_median_duration_milliseconds()
    {
        return calculate_duration_percentile(50);
    }
    float calculate_average_duration_milliseconds()
    {
        if (thread_durations.empty())
        {
            return 0;
        }

        auto const count = static_cast<float>(thread_durations.size());
        return (std::accumulate(thread_durations.begin(), thread_durations.end(), 0.0) / count);
    }

    void RunTest()
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        ExecuteTestLogic();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ms = (end_time - start_time).count() / 1000000.0;
        std::cout << duration_ms << std::endl;

        std::lock_guard<std::mutex> lock(mtx);
        thread_durations.push_back(duration_ms);
    }
    void RunMultiThreadedTest(int input_num_iterations, int num_threads, float median_duration_upper_threshold_milliseconds)
    {
        std::vector<std::thread> threads;
        this->num_iterations = input_num_iterations;
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([this]
                                 {
                for (int j = 0; j < num_iterations; ++j) {
                    RunTest();
                } });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        std::cout
            << "Total iterations: " << thread_durations.size() << std::endl
            << "Mean duration: " << calculate_average_duration_milliseconds() << " milliseconds" << std::endl
            << "1th percentile: " << calculate_duration_percentile(1) << " milliseconds" << std::endl
            << "10th percentile: " << calculate_duration_percentile(10) << " milliseconds" << std::endl
            << "Median duration: " << calculate_median_duration_milliseconds() << " milliseconds" << std::endl
            << "90th percentile: " << calculate_duration_percentile(90) << " milliseconds" << std::endl
            << "95th percentile: " << calculate_duration_percentile(95) << " milliseconds" << std::endl
            << "99th percentile: " << calculate_duration_percentile(99) << " milliseconds" << std::endl;
        EXPECT_LE(calculate_median_duration_milliseconds(), median_duration_upper_threshold_milliseconds);
    }
};
class TestHappyFlow : public PerformanceTestBase
{
protected:
    void ExecuteTestLogic() override
    {
        int arr[] = {1, 5, 4, 6, 7, 9, 8, 10, 2, 3};
        size_t n = sizeof(arr) / sizeof(arr[0]);
        float mean = calculate_stupid_mean(arr, n, 20000);
        EXPECT_FLOAT_EQ(mean, 5.5);
    }
};
class TestEmptyArr : public PerformanceTestBase
{
protected:
    void ExecuteTestLogic() override
    {
        int arr[] = {};
        float mean = calculate_stupid_mean(arr, 0, 2000);
        EXPECT_FLOAT_EQ(mean, 0);
    }
};

TEST_F(TestHappyFlow, Test_Happy_Flow)
{
    RunMultiThreadedTest(10, 3, 900.0);
}