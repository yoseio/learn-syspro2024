#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#define N 1000

std::vector<double> rnd_exp(const int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  std::vector<double> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() { return dist(gen); });
  return rnd;
}

std::vector<int> random_walk(const std::vector<double>& rnd,
                             const double p,
                             const int d) {
  std::vector<int> walk(rnd.size());
  for (int i = 0; i < rnd.size(); ++i) {
    walk[i] = walk[i - 1] + (rnd[i] < p ? d : -d);
  }
  return walk;
}

double e(const std::vector<double>& rnd, const int d) {
  std::vector<double> ex(rnd.size());
  for (int i = 0; i < rnd.size(); ++i) {
    ex[i] = rnd[i] * d + (1 - rnd[i]) * -d;
  }

  return std::accumulate(ex.begin(), ex.end(), 0.0);
}

double v(const std::vector<double>& rnd, const int d) {
  double r1 = 0;
  std::vector<double> ex2(rnd.size());
  for (int i = 0; i < rnd.size(); ++i) {
    ex2[i] = rnd[i] * d * d + (1 - rnd[i]) * d * d;
  }
  r1 += std::accumulate(ex2.begin(), ex2.end(), 0.0);

  double r2 = 0;
  std::vector<double> ex(rnd.size());
  for (int i = 0; i < rnd.size(); ++i) {
    ex[i] = rnd[i] * d + (1 - rnd[i]) * -d;
  }
  for (int i = 0; i < rnd.size(); ++i) {
    for (int j = 0; j <= i; ++j) {
      r2 += ex[i] * ex[j];
    }
  }

  return r1 + 2 * r2;
}

double mean(const std::vector<double>& v) {
  return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double dist(const std::vector<double>& v) {
  const double m = mean(v);
  return std::accumulate(
             v.begin(), v.end(), 0.0,
             [&](double acc, double x) { return acc + std::pow(x - m, 2); }) /
         v.size();
}

int main() {
  std::cout << std::fixed;

  const double p = 0.5;
  const int d = 1;

  const auto rnd = rnd_exp(N);
  const auto walk = random_walk(rnd, p, d);
  std::ofstream file("2-3.csv");
  file << "t,st" << std::endl;
  for (int i = 0; i < N; ++i) {
    file << i + 1;
    file << "," << walk[i];
    file << std::endl;
  }

  std::vector<double> walk100(100);
  for (int i = 0; i < 100; ++i) {
    const auto rnd = rnd_exp(N);
    const auto walk = random_walk(rnd, p, d);

    std::cout << "E[S_t] : " << e(rnd, d) << "\t";
    std::cout << "V[S_t] : " << v(rnd, d) << std::endl;

    walk100[i] = walk[N - 1];
  }
  std::cout << "mean : " << mean(walk100) << std::endl;
  std::cout << "dist : " << dist(walk100) << std::endl;
}
