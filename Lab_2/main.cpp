#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

void mergeSortRecursive(vector<double>& arr, vector<double>& helper, size_t left, size_t right,
    size_t depth, size_t& calls, size_t& maxDepth, size_t& additionalMemory) {
    calls++;
    maxDepth = max(maxDepth, depth);

    if (left >= right) return;

    size_t mid = left + (right - left) / 2;

    mergeSortRecursive(arr, helper, left, mid, depth + 1, calls, maxDepth, additionalMemory);
    mergeSortRecursive(arr, helper, mid + 1, right, depth + 1, calls, maxDepth, additionalMemory);

    size_t i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        helper[k++] = arr[i] <= arr[j] ? arr[i++] : arr[j++];
    }
    while (i <= mid) helper[k++] = arr[i++];
    while (j <= right) helper[k++] = arr[j++];

    const size_t usedMemory = (right - left + 1) * sizeof(double);
    additionalMemory = max(additionalMemory, usedMemory);

    copy(helper.begin() + left, helper.begin() + right + 1, arr.begin() + left);
}

void mergeSort(vector<double>& arr, size_t& calls, size_t& maxDepth, size_t& additionalMemory) {
    calls = maxDepth = additionalMemory = 0;
    vector<double> helper(arr.size());
    mergeSortRecursive(arr, helper, 0, arr.size() - 1, 1, calls, maxDepth, additionalMemory);
}

vector<double> generateArray(size_t size) {
    static mt19937 gen(random_device{}());
    uniform_real_distribution<double> dist(-1.0, 1.0);
    vector<double> arr(size);
    generate(arr.begin(), arr.end(), [&]() { return dist(gen); });
    return arr;
}

template <typename T>
T average(const vector<T>& vec) {
    return accumulate(vec.begin(), vec.end(), T{}) / vec.size();
}

void runBenchmark(const vector<size_t>& sizes, const size_t attempts, const string& filename) {
    ofstream fout(filename);
    fout << fixed << setprecision(3);

    for (auto size : sizes) {
        vector<double> timings;
        vector<size_t> callsVec, depthsVec, memoryVec;

        for (size_t attempt = 0; attempt < attempts; ++attempt) {
            auto arr = generateArray(size);
            size_t calls{}, depth{}, extraMemory{};

            const auto start = chrono::steady_clock::now();
            mergeSort(arr, calls, depth, extraMemory);
            const auto finish = chrono::steady_clock::now();

            double elapsedMs = chrono::duration<double, milli>(finish - start).count();

            timings.push_back(elapsedMs);
            callsVec.push_back(calls);
            depthsVec.push_back(depth);
            memoryVec.push_back(extraMemory);
        }

        fout << "Размер массива: " << size << '\n';
        fout << "Среднее время (мс): " << average(timings)
            << " | Лучшее: " << *min_element(timings.begin(), timings.end())
            << " | Худшее: " << *max_element(timings.begin(), timings.end()) << '\n';

        fout << "Кол-во вызовов рекурсии Сред.: " << average(callsVec)
            << " | Лучшее: " << *min_element(callsVec.begin(), callsVec.end())
            << " | Худшее: " << *max_element(callsVec.begin(), callsVec.end()) << '\n';

        fout << "Глубина рекурсии Сред.: " << average(depthsVec)
            << " | Лучшая: " << *min_element(depthsVec.begin(), depthsVec.end())
            << " | Худшая: " << *max_element(depthsVec.begin(), depthsVec.end()) << '\n';

        fout << "Доп. память (байт) Сред.: " << average(memoryVec)
            << " | Лучшая: " << *min_element(memoryVec.begin(), memoryVec.end())
            << " | Худшая: " << *max_element(memoryVec.begin(), memoryVec.end()) << "\n\n";
    }
    fout.close();
}

int main() {
    const vector<size_t> sizes{ 1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000 };
    const size_t attempts_per_series = 20;

    runBenchmark(sizes, attempts_per_series, "merge_sort_results.txt");
}