#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include <random>
#include <list>

using solution_t = std::vector < double >;

double sphere_function(solution_t x) {
    return -std::accumulate(x.begin(), x.end(), 0.0, 
        [](double a, double b){return a+b*b;});
}

double schaffer_4(solution_t p) {
	auto x = p.at(0);
	auto y = p.at(1);
	auto l = cos(sin(abs(x*x-y*y)));
	l=l*l-0.5;
	auto m = 1 + 0.001*(x*x+y*y);
	m = m*m; 
	return 0.5 + l/m;
}
solution_t hillclimb(std::function< double(solution_t ) > f, 
       solution_t start,
       int K = 1000) {
    using namespace std;
    auto current_solution = start;
    for (int i = 0; i  < K; i++) {

        auto new_solution = current_solution;
        new_solution[(i>>1)%current_solution.size()] += 0.01*((i%2)*2-1);
        if (f(new_solution) > f(current_solution) ) {
            current_solution = new_solution;
            //cout << "wynik: ";
            //for (auto e: current_solution) cout << e << " ";
            //cout << " new: ";
            //for (auto e: new_solution) cout << e << " ";
            //cout << endl;
        }

    }
    return current_solution;
}

std::random_device rd{};
std::mt19937 gen{rd()};

solution_t sim_annealing(std::function< double(solution_t ) > f, 
       const solution_t start,
       std::function<solution_t (solution_t) > N,
       std::function<double (int) > T,
       int K = 1000
       ) {
		   using namespace std;

		uniform_real_distribution<> uniform_dist(0.0, 1.0);		

		list<solution_t> s;
		s.push_back(start);
		for (int k = 1; k <= K; k++ ) {
			 auto t = N(s.back());
			if (f(t) > f(s.back())) {
				s.push_back(t);
			} else {
				if (uniform_dist(gen) < exp(-abs(f(t)-f(s.back()))/ T(k) )) {
					s.push_back(t);
				}
			}
		 }
		 return *max_element(s.begin(),s.end(), [f](auto a,auto b){
			 return f(a) < f(b);
		 });
}

void print_solution(solution_t solution) {
    using namespace std;
    cout << "solution : ";
	for (auto e: solution) cout << e << " ";
    cout << endl;
}

int main() {
    using namespace std;
    uniform_real_distribution<> dist_(-2.0, 2.0);
    vector <double> start = {dist_(gen),dist_(gen)};
    
    int K = 10000;
    
    auto solution_hc = hillclimb(schaffer_4,start,K);
	
	double r = 1.0;
	auto solution_sa = sim_annealing(schaffer_4,start,
		[&r](solution_t s) {
			r = r * 0.999;
			normal_distribution<> dist{0.0,r};
			//uniform_real_distribution<> dist(-1.0, 1.0);		
			std::cout << "---: f: " <<  sphere_function(s) << " ";
			print_solution(s);
			for (auto &e: s) {
				e = e + dist(gen);
			}
			std::cout << "   : f: " <<  sphere_function(s) << " ";
			print_solution(s);
			return s;
		},
        [](int k)->double{
			return 1.0/(double)k;
		},K
	);
	cout << "hill climb: ";
	print_solution(solution_hc);
	cout << "simmulated annealing: ";
	print_solution(solution_sa);

}
