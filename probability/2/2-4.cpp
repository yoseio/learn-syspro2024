#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

std::vector<int> rnd_lgc(const int n, const int m, const int a, const int b) {
  std::random_device rd;
  int x = rd();

  std::vector<int> rnd(n);
  for (int i = 0; i < n; ++i) {
    x = ((long long)a * x + b) % m;
    rnd[i] = x;
  }
  return rnd;
}

std::vector<int> rnd_mt(const int n, const int m) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, m);

  std::vector<int> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() { return dist(gen); });
  return rnd;
}

int main() {
  const int n = 100000;
  const int m = 16;

  auto lgc = rnd_lgc(n, m, 5, 7);
  auto mt = rnd_mt(n, m);

  std::ofstream file("2-4.csv");
  file << "lgc_x,lgc_y,mt_x,mt_y" << std::endl;
  for (int i = 1; i < n; ++i) {
    file << lgc[i - 1] << "," << lgc[i] << ",";
    file << mt[i - 1] << "," << mt[i] << std::endl;
  }
}
