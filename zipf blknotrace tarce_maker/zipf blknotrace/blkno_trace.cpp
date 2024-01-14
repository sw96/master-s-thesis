#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <sstream>
// Zipf distribution parameters
const double s = 0.3;  // Zipf 계수
const int numBlocks = 150000000; // trace 범위 => numBlocks-1 까지 범위를 가진
const int numTraces = 100000000; // total trace 갯수

int main() {
    // Generate Zipf distribution
    std::vector<double> zipfDist;
    for (int i = 1; i <= numBlocks; ++i) {
        zipfDist.push_back(1.0 / pow(i, s));
    }

    double sum = std::accumulate(zipfDist.begin(), zipfDist.end(), 0.0);
    for (auto& val : zipfDist) {
        val /= sum;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> dist(zipfDist.begin(), zipfDist.end());

    std::vector<std::pair<int, int>> traces;
    for (int i = 0; i < numTraces; ++i) {
        int blockNumber = dist(gen) + 1; 
        int blockSize = 4096 + (gen() % 4096); 
        traces.push_back(std::make_pair(blockNumber, blockSize));
    }
    /*
    for (const auto& trace : traces) {
        std::cout << "Block Number: " << trace.first << ", Size: " << trace.second << std::endl;
    }
    */
    FILE* fp = fopen("trace0.3.txt", "w");

    if (fp != nullptr) {
        for (const auto& trace : traces) {
            std::ostringstream blockNumberStream;
            blockNumberStream << trace.first;
            std::string blockNumberStr = blockNumberStream.str();

            fputs(blockNumberStr.c_str(), fp);
            fputc('\n', fp); // Add a new line
        }

        fclose(fp); // Close the file
        std::cout << "Data written to trace.txt." << std::endl;
    }
    else {
        std::cerr << "Error opening file for writing." << std::endl;
        return 1;
    }
    return 0;
}
