#include <algorithm>
#include <iostream>
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

void coin_toss(const double p) {
  const auto rnd = rnd_exp(N);
  int head = 0;

  for (const auto& r : rnd) {
    if (r <= p) {
      head++;
    }
  }

  std::cout << "p: " << p;
  std::cout << ", 表: " << head;
  std::cout << ", 裏: " << N - head;
  std::cout << std::endl;
}

int main() {
  coin_toss(0.2);
  coin_toss(0.5);
  coin_toss(0.7);

  return 0;
}
