#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#define N 1000
// #define DEBUG

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1, 6);

  std::vector<int> res(N);
  std::generate(res.begin(), res.end(), [&]() { return dist(gen); });

#ifdef DEBUG
  for (int i = 0; i < N; ++i) {
    std::cout << res[i] << ",";
  }
  std::cout << std::endl;
#endif

  for (int i = 1; i < N; ++i) {
    res[i] += res[i - 1];
  }

  std::ofstream file("1-3.csv");
  file << "n,mean" << std::endl;
  for (int i = 0; i < N; ++i) {
    file << i + 1 << "," << (double)res[i] / (i + 1) << std::endl;
  }

  return 0;
}
