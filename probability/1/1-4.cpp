#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#define N 1000
// #define DEBUG

int main() {
  const std::vector<double> probabilities = {1.0 / 9, 2.0 / 9, 1.0 / 9,
                                             2.0 / 9, 1.0 / 9, 2.0 / 9};

  std::random_device rd;
  std::mt19937 gen(rd());
  std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

  std::vector<int> res(N);
  std::generate(res.begin(), res.end(), [&]() { return dist(gen) + 1; });

#ifdef DEBUG
  for (int i = 0; i < N; ++i) {
    std::cout << res[i] << ",";
  }
  std::cout << std::endl;
#endif

  for (int i = 1; i < N; ++i) {
    res[i] += res[i - 1];
  }

  std::ofstream file("1-4.csv");
  file << "n,mean" << std::endl;
  for (int i = 0; i < N; ++i) {
    file << i + 1 << "," << (double)res[i] / (i + 1) << std::endl;
  }

  return 0;
}
