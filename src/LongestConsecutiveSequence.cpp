#include <benchmark/benchmark.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "skarupke/flat_hash_map/flat_hash_map.h"

/**
 * 128. Longest Consecutive Sequence
 * Given an unsorted array of integers nums, return the length of the longest consecutive elements sequence.
 *
 * https://leetcode.com/problems/longest-consecutive-sequence/
 */

//----------------------------------------------------------------------------------------------------
//Benchmark                                          Time             CPU   Iterations UserCounters...
//----------------------------------------------------------------------------------------------------
//LongestConsecutiveSequence_Vector_Random        1022 ns          961 ns       848361 items_per_second=10.4055M/s
//LongestConsecutiveSequence_Vector_Same         18005 ns        17856 ns        38385 items_per_second=56.0051M/s
//LongestConsecutiveSequence_HashSet_Random       6286 ns         6220 ns       115452 items_per_second=1.60783M/s
//LongestConsecutiveSequence_HashSet_Same        73571 ns        73023 ns         8378 items_per_second=13.6943M/s
//LongestConsecutiveSequence_HashMap_Random       5933 ns         5901 ns       119248 items_per_second=1.69472M/s
//LongestConsecutiveSequence_HashMap_Same       100456 ns       100021 ns         7134 items_per_second=9.99794M/s
//LongestConsecutiveSequence_FlatMap_Random       6873 ns         6836 ns       101752 items_per_second=1.46293M/s
//LongestConsecutiveSequence_FlatMap_Same        54840 ns        54328 ns        13912 items_per_second=18.4066M/s

int LongestConsecutiveSequence_Vector(std::vector<int> nums) {
    if (nums.empty()) {
        return 0;
    }

    std::sort(nums.begin(), nums.end());
    int longestStreak = 1;
    int currentStreak = 1;
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] != nums[i - 1]) { // only process element if not duplicates
            if (nums[i] == nums[i - 1] + 1) {
                currentStreak += 1;
            } else {
                longestStreak = std::max(longestStreak, currentStreak);
                currentStreak = 1;
            }
        }
    }

    return std::max(longestStreak, currentStreak);
}

void BM_LongestConsecutiveSequence_Vector_Random(benchmark::State &state) {
    std::vector<int> nums {0,3,7,2,5,8,4,6,0,1};

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_Vector(nums));
    }
    state.SetItemsProcessed(10 * state.iterations());
}

void BM_LongestConsecutiveSequence_Vector_Same(benchmark::State &state) {
    std::vector<int> nums(1000);
    std::fill(nums.begin(), nums.end(), 1);

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_Vector(nums));
    }
    state.SetItemsProcessed(1000 * state.iterations());
}

int LongestConsecutiveSequence_HashSet(std::vector<int> nums) {
    if (nums.empty()) {
        return 0;
    }

    std::unordered_set<int> numSet(nums.begin(), nums.end());
    int longestStreak = 0;
    for (int num : numSet) {
        if (!numSet.count(num - 1)) {
            int currentNum = num;
            int currentStreak = 1;
            while (numSet.count(currentNum + 1)) {
                currentNum += 1;
                currentStreak += 1;
            }
            longestStreak = std::max(longestStreak, currentStreak);
        }
    }
    return longestStreak;
}

void BM_LongestConsecutiveSequence_HashSet_Random(benchmark::State &state) {
    std::vector<int> nums {0,3,7,2,5,8,4,6,0,1};

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_HashSet(nums));
    }
    state.SetItemsProcessed(10 * state.iterations());
}

void BM_LongestConsecutiveSequence_HashSet_Same(benchmark::State &state) {
    std::vector<int> nums(1000);
    std::fill(nums.begin(), nums.end(), 1);

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_HashSet(nums));
    }
    state.SetItemsProcessed(1000 * state.iterations());
}

int LongestConsecutiveSequence_HashMap(std::vector<int> nums) {
    if (nums.empty()) {
        return 0;
    }

    std::unordered_map<int, int> numSet;
    for (auto const & n: nums) {
        numSet[n]++;
    }

    int maxStreak = 0;
    for (auto num: numSet) {
        // num is the smallest in the sequence
        if (numSet.find(num.first - 1) == numSet.end()) {
            int curNum = num.first;
            // start loop to count sequence
            while (numSet.find(++curNum) != numSet.end());
            maxStreak = std::max(curNum - num.first, maxStreak);
        }
    }

    return maxStreak;
}

void BM_LongestConsecutiveSequence_HashMap_Random(benchmark::State &state) {
    std::vector<int> nums {0,3,7,2,5,8,4,6,0,1};

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_HashMap(nums));
    }
    state.SetItemsProcessed(10 * state.iterations());
}

void BM_LongestConsecutiveSequence_HashMap_Same(benchmark::State &state) {
    std::vector<int> nums(1000);
    std::fill(nums.begin(), nums.end(), 1);

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_HashMap(nums));
    }
    state.SetItemsProcessed(1000 * state.iterations());
}

int LongestConsecutiveSequence_FlatMap_iterative(int key, auto& cnt, auto& memo) {
    int depth = 0;
    int deep_res = 0;
    while (true) {
        if (auto it = memo.find(key); it != memo.end()) {
            deep_res = it->second;
            break;
        }

        if (auto it = cnt.find(key); it != cnt.end()) {
            if (it->second == 1) {
                ++key;
                ++depth;
            } else
                deep_res = 1;
        } else
            break;
    }
    while (depth >= 0) {
        memo[key] = deep_res;
        --key;
        --depth;
        ++deep_res;
    }
    return deep_res - 1;
}

// TODO: does this ignore duplicates?
int LongestConsecutiveSequence_FlatMap(std::vector<int> in) {
    if (in.empty()) {
        return 0;
    }

    ska::flat_hash_map<int, int> cnt;
    ska::flat_hash_map<int, int> memo;
    for (const auto& x : in)
        ++cnt[x];

    int max_len = 1;
    for (const auto& [k, v] : cnt)
        max_len = std::max(max_len, 1 + LongestConsecutiveSequence_FlatMap_iterative(k + 1, cnt, memo));
    return max_len;
}

void BM_LongestConsecutiveSequence_FlatMap_Random(benchmark::State &state) {
    std::vector<int> nums {0,3,7,2,5,8,4,6,0,1};

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_FlatMap(nums));
    }
    state.SetItemsProcessed(10 * state.iterations());
}

void BM_LongestConsecutiveSequence_FlatMap_Same(benchmark::State &state) {
    std::vector<int> nums(1000);
    std::fill(nums.begin(), nums.end(), 1);

    for (auto _: state) {
        benchmark::DoNotOptimize(LongestConsecutiveSequence_FlatMap(nums));
    }
    state.SetItemsProcessed(1000 * state.iterations());
}

BENCHMARK(BM_LongestConsecutiveSequence_Vector_Random)->Name("LongestConsecutiveSequence_Vector_Random");
BENCHMARK(BM_LongestConsecutiveSequence_Vector_Same)->Name("LongestConsecutiveSequence_Vector_Same");
BENCHMARK(BM_LongestConsecutiveSequence_HashSet_Random)->Name("LongestConsecutiveSequence_HashSet_Random");
BENCHMARK(BM_LongestConsecutiveSequence_HashSet_Same)->Name("LongestConsecutiveSequence_HashSet_Same");
BENCHMARK(BM_LongestConsecutiveSequence_HashMap_Random)->Name("LongestConsecutiveSequence_HashMap_Random");
BENCHMARK(BM_LongestConsecutiveSequence_HashMap_Same)->Name("LongestConsecutiveSequence_HashMap_Same");
BENCHMARK(BM_LongestConsecutiveSequence_FlatMap_Random)->Name("LongestConsecutiveSequence_FlatMap_Random");
BENCHMARK(BM_LongestConsecutiveSequence_FlatMap_Same)->Name("LongestConsecutiveSequence_FlatMap_Same");