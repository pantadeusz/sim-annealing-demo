/**
 * Przykład porównawczy dla algorytmu wspinaczkowego i algorytmu symulowanego wyżarzania.
 * 
 * zadanie polega na maksymalizacji funkcji celu
 * 
 * */

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <map>

using solution_t = std::vector<double>;

double sphere_function(solution_t x)
{
	return -std::accumulate(x.begin(), x.end(), 0.0,
							[](double a, double b) { return a + b * b; });
}

double schaffer_4(solution_t p)
{
	auto x = p.at(0);
	auto y = p.at(1);
	auto l = cos(sin(abs(x * x - y * y)));
	l = l * l - 0.5;
	auto m = 1.0 + 0.001 * (x * x + y * y);
	m = m * m;
	return -(0.5 + l / m);
}

solution_t hillclimb(std::function<double(solution_t)> f, solution_t start,
					 int K = 1000)
{
	using namespace std;
	auto current_solution = start;
	for (int i = 0; i < K; i++)
	{
		auto new_solution = current_solution;
		// check every coordinate
		new_solution[(i >> 1) % current_solution.size()] +=
			0.01 * ((i % 2) * 2 - 1);
		if (f(new_solution) > f(current_solution))
		{
			current_solution = new_solution;
		}
	}
	return current_solution;
}

std::random_device rd{};
std::mt19937 gen{rd()};

solution_t sim_annealing(std::function<double(solution_t)> f,
						 const solution_t start,
						 std::function<solution_t(solution_t)> N,
						 std::function<double(int)> T, int K = 1000)
{
	using namespace std;

	uniform_real_distribution<> uniform_dist(0.0, 1.0);

	list<solution_t> s;
	s.push_back(start);
	for (int k = 1; k <= K; k++)
	{
		auto t = N(s.back());
		if (f(t) > f(s.back()))
		{
			s.push_back(t);
		}
		else
		{
			if (uniform_dist(gen) < exp(-abs(f(t) - f(s.back())) / T(k)))
			{
				s.push_back(t);
			}
		}
	}
	return *max_element(s.begin(), s.end(),
						[f](auto a, auto b) { return f(a) < f(b); });
}

void print_solution(solution_t solution)
{
	using namespace std;
	cout << "solution : ";
	for (auto e : solution)
		cout << e << " ";
	cout << endl;
}

int main()
{
	using namespace std;

	int hc_vs_sa = 0;
	map<string, double> averages = {{"annealing", 0.0}, {"hillclimb", 0.0}};
	for (int zz = 0; zz < 100; zz++)
	{
		uniform_real_distribution<> dist_(-50.0, 50.0);
		vector<double> start = {dist_(gen), dist_(gen)};

		int K = 10000;

		auto f = schaffer_4;

		auto solution_hc = hillclimb(f, start, K);

		auto solution_sa =
			sim_annealing(f, start,
						  [&f](solution_t s) {
							  normal_distribution<> dist{0.0, 1.0};
							  for (auto &e : s)
								  e = e + dist(gen);
							  return s;
						  },
						  [](int k) -> double { return 1.0 / log((double)k); }, K);
		cout << "hill climb: ";
		std::cout << "   : f: " << f(solution_hc) << " ";
		print_solution(solution_hc);
		cout << "simmulated annealing: ";
		std::cout << "   : f: " << f(solution_sa) << " ";
		print_solution(solution_sa);
		averages["hillclimb"] += f(solution_hc);
		averages["annealing"] += f(solution_sa);
		
		if (f(solution_hc) < f(solution_sa))
		{
			hc_vs_sa++;
		}
		else if (f(solution_hc) > f(solution_sa))
		{
			hc_vs_sa--;
		}
	}
	std::cout << "Wynik to " << hc_vs_sa << "  (ile razy wyzarzanie bylo lepsze)"
			  << std::endl;
	for (auto &[k,v]:averages) {
		cout <<"średnia dla " << k << " wynosi " << (v/100.0) << " (im wyższa wartość tym lepiej)" << endl; 
	}
}
