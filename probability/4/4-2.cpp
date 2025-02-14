#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0.0, 1.0);

class Event {
 public:
  Event(const int i, const double t) : id(i), time(t) {}
  ~Event() {}

  int id;
  double time;
};

double rnd_exp(const double λ) {
  const double u = dist(gen);
  return -1 / λ * std::log(1 - u);
}

std::vector<double> rnd_exp_vec(const int n, const double λ) {
  std::vector<double> rnd(n);
  std::generate(rnd.begin(), rnd.end(), [&]() { return rnd_exp(λ); });
  return rnd;
}

std::vector<Event> poisson_exp(const int n, const double λ) {
  const std::vector<double> rnd = rnd_exp_vec(n, λ);
  double t = 0;
  std::vector<Event> events;
  events.reserve(n);

  for (int i = 0; i < n; i++) {
    t += rnd[i];
    events.emplace_back(i, t);
  }

  return events;
}

double checkout_counter_sim(const double λ,
                            const double μ,
                            const int s,
                            const int n) {
  const std::vector<Event> events = poisson_exp(n, λ);

  std::priority_queue<double, std::vector<double>, std::greater<double>> t_s;
  for (const auto rnd : rnd_exp_vec(s, μ)) {
    t_s.push(rnd);
  }

  int num_loss = 0;
  for (const auto& event : events) {
    const double t = event.time;
    const double min_t_s = t_s.top();

    if (min_t_s < t) {
      t_s.pop();
      t_s.push(t + rnd_exp(μ));
    } else {
      num_loss++;
    }
  }

  return num_loss / static_cast<double>(n);
}

int main() {
  const int c_N = 100000;
  const int c_S = 1;
  const double c_λ = 2.0;

  for (int s = 1; s <= c_S; s++) {
    const auto filename =
        "4-2-" + std::to_string(s) + ".csv";
    std::ofstream file(filename);
    file << "λ,μ,loss" << std::endl;

    for (double μ = 1.5; μ <= 2.5; μ += 0.1) {
      const auto loss = checkout_counter_sim(c_λ, μ, s, c_N);
      file << c_λ << "," << μ << "," << loss << std::endl;
    }
  }
}
