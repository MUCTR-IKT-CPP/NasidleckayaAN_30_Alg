#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric> 
#include <fstream> 

void selectionSort(std::vector<double>& arr, int& passCount, int& swaps) {
    auto n = arr.size();
    passCount = 0;  
    swaps = 0;

    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        ++passCount;  

        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            std::swap(arr[i], arr[minIndex]);
            ++swaps;
        }
    }
}

void runTests() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    std::vector<int> sizes = { 1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000 };
    constexpr int kSeries = 20; 

    std::ofstream outputFile("sorting_results.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Ошибка при открытии файла для записи!" << std::endl;
        return;
    }

    outputFile << "Результаты сортировки\n";
    outputFile << "===========================\n";

    for (const int size : sizes) {
        std::vector<double> timeRecords(kSeries);
        std::vector<int> passCountRecords(kSeries);  
        std::vector<int> swapRecords(kSeries);

        for (int run = 0; run < kSeries; ++run) {
            std::vector<double> arr(size);
            for (int i = 0; i < size; ++i) {
                arr[i] = dis(gen);
            }

            int passCount = 0; 
            int swaps = 0;

            auto start = std::chrono::high_resolution_clock::now();
            selectionSort(arr, passCount, swaps);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            timeRecords[run] = elapsed.count();
            passCountRecords[run] = passCount;
            swapRecords[run] = swaps;
        }

        auto minmax_time = std::minmax_element(timeRecords.begin(), timeRecords.end());
        double average_time = std::accumulate(timeRecords.begin(), timeRecords.end(), 0.0) / kSeries;

        auto minmax_passCount = std::minmax_element(passCountRecords.begin(), passCountRecords.end());
        double average_passCount = std::accumulate(passCountRecords.begin(), passCountRecords.end(), 0.0) / kSeries;

        auto minmax_swaps = std::minmax_element(swapRecords.begin(), swapRecords.end());
        double average_swaps = std::accumulate(swapRecords.begin(), swapRecords.end(), 0.0) / kSeries;

        outputFile << "Размер массива: " << size << "\n";
        outputFile << "Среднее время: " << average_time << "с\n";
        outputFile << "Лучшее время: " << *minmax_time.first << "с\n";
        outputFile << "Худшее время: " << *minmax_time.second << "с\n";
        outputFile << "Среднее количество проходов: " << average_passCount << "\n";
        outputFile << "Лучшее количество проходов: " << *minmax_passCount.first << "\n";
        outputFile << "Худшее количество проходов: " << *minmax_passCount.second << "\n";
        outputFile << "Среднее количество обменов: " << average_swaps << "\n";
        outputFile << "Лучшее количество обменов: " << *minmax_swaps.first << "\n";
        outputFile << "Худшее количество обменов: " << *minmax_swaps.second << "\n";
        outputFile << "\n";
    }

    outputFile.close();
    std::cout << "Результаты были записаны в файл 'sorting_results.txt'.\n";
}

int main() {
    runTests();
    return 0;
}
