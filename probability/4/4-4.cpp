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
  Event(const int i, const double t) : id(i), time(t), wait(0) {}
  ~Event() {}

  int id;
  double time;
  double wait;
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
  std::vector<Event> events = poisson_exp(n, λ);

  std::priority_queue<double, std::vector<double>, std::greater<double>> t_s;
  for (const auto rnd : rnd_exp_vec(s, μ)) {
    t_s.push(rnd);
  }

  for (int i = 0; i < n; i++) {
    const double t = events[i].time;
    const double min_t_s = t_s.top();

    double start_service_time = std::max(t, min_t_s);
    events[i].wait = start_service_time - t;

    t_s.pop();
    t_s.push(start_service_time + rnd_exp(μ));
  }

  double total_waiting = 0;
  for (const auto event : events) {
    total_waiting += event.wait;
  }

  return total_waiting / static_cast<double>(n);
}

int main() {
  const int c_N = 100000;
  const int c_S = 1;

  const auto fn1 = "4-4-1.csv";
  std::ofstream f1(fn1);
  f1 << "λ,μ,loss" << std::endl;
  for (double μ = 2.0; μ <= 3.0; μ += 0.1) {
    const auto loss = checkout_counter_sim(0.5, μ, c_S, c_N);
    f1 << 0.5 << "," << μ << "," << loss << std::endl;
  }

  const auto fn2 = "4-4-2.csv";
  std::ofstream f2(fn2);
  f2 << "λ,μ,loss" << std::endl;
  for (double μ = 2.0; μ <= 3.0; μ += 0.1) {
    const auto loss = checkout_counter_sim(1.0, μ, c_S, c_N);
    f2 << 1.0 << "," << μ << "," << loss << std::endl;
  }

  const auto fn3 = "4-4-3.csv";
  std::ofstream f3(fn3);
  f3 << "λ,μ,loss" << std::endl;
  for (double μ = 2.0; μ <= 3.0; μ += 0.1) {
    const auto loss = checkout_counter_sim(1.5, μ, c_S, c_N);
    f3 << 1.5 << "," << μ << "," << loss << std::endl;
  }
}
