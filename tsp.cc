#include <cmath>
#include <vector>
#include <random>
#include <iostream>

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used

typedef struct city
{
	int x, y;
} city;


// setup the demo cities
void
setDemoCities(std::vector<city>& v)
{
	v.resize(0);
	v.push_back({0,4});
	v.push_back({2,2});
	v.push_back({3,5});
	v.push_back({6,5});
	v.push_back({6,1});
	v.push_back({9,4});
	v.push_back({5,9});
}

void
initCities(std::vector<city>& v, int numCities)
{
	std::uniform_int_distribution<int> uni(1,100);

	v.resize(0);
	for (int n = 0; n < numCities; n++)
		// create a random city at x,y
		// add it to vector
		v.push_back({uni(rng),uni(rng)});
}

// write out a tour vector
std::ostream&
operator<<(std::ostream& os, std::vector<size_t>& v)
{
	os << "[";
	for(auto vv: v)
		os << ' ' << vv ;
	os << " ]";
	return os;
}

// write out a tour vector
std::ostream&
operator<<(std::ostream& os, std::vector<city>& v)
{
	os << '[';
	for(auto c: v)
		os << c.x << ' ' << c.y << ';';
	os << ']';
	return os;
}

// compute distance between two cities
double dist(std::vector<city>& cities, size_t a, size_t b)
{
	int x = cities[a-1].x - cities[b-1].x;
	int y = cities[a-1].y - cities[b-1].y;
	return sqrt(x*x+y*y);
}

// function f computes total distance of travel
double tour_dist(std::vector<city>& cities, const std::vector<size_t>& x)
{
	double sum = 0.0;
	// sum all the cities from second city to last
	for (size_t i = 1; i < x.size(); i++)
		sum += dist(cities,x[i-1],x[i]);

	// The SA method maximizes the function
	// but we want the minimum distance so
	// return the negative to flip the function
	return -sum;
}

// probability that we bump the solution
// to get away from local minimum
double p(size_t n, double C, double fy, double fx)
{
	if (fy >= fx) return 1.0;
	return pow(n+1,C*(fy-fx));
}

void mutate(std::vector<size_t>& v, std::uniform_int_distribution<size_t>& uni)
{
	size_t c1,c2,temp;
	// pick two random cities to swap
	do {
		c1 = uni(rng);
		c2 = uni(rng);
	} while (c1 == c2);

	// swap the two cities
	temp = v[c1];
	v[c1] = v[c2];
	v[c2] = temp;
}

void createSolution(std::vector<size_t>& v, std::uniform_int_distribution<size_t>& uni, size_t n)
{
	v.resize(0);
	// create a solution vector filled with city numbers 1 .. n
	for (size_t i = 1; i <= n; i++)
		v.push_back(i);
	// add 1 to the end which is the return to starting city
	v.push_back(1);

	// mutate the solution vector 2*n times to create
	// a new random solution.
	for (size_t i = 0; i < n*2; i++)
		mutate(v, uni);
}

int main(int argc, char **argv)
{
	int N = 10;
	double C = 1.0;
	double M = 100;

	std::vector<city> cities;
	setDemoCities(cities);
	
	if (argc > 1)
	{
		int numCities = atoi(argv[1]);

		// if numCities <= 7 just use the demo cities
		// but...
		if (numCities > 7)
		{
			// maximum of 60 cities
			if (numCities > 60) numCities = 60;
			initCities(cities, numCities);
		}
	}

	// N = number of tours to optimize
	if (argc > 2) N = atoi(argv[2]);
	// C = constant for our probablity function
	if (argc > 3) C = atof(argv[3]);
	// M = maximum number of mutations which we are
	// willing to make without improving. If we don't
	// improve after this many iterations, break
	if (argc > 4) M = atoi(argv[4]);

	size_t numCities = cities.size();

	// setup rng
	std::uniform_int_distribution<size_t> uni(1,numCities-1);
	std::uniform_real_distribution<> unir(0, 1);

	std::vector<size_t> best; // best solution vector
	// for now, our best solution is just an initial guess
	createSolution(best, uni, numCities);
	double fbest = tour_dist(cities, best);

	// start with N different solutions
#pragma omp parallel for schedule(dynamic) default(shared) private(rng)
	for (int i = 1; i <= N; i++)
	{
		// create a new starting solution
		std::vector<size_t> x, y;
		createSolution(x, uni, numCities);
		double fx = tour_dist(cities, x);

		std::cout << "starting point " << i << std::endl;
		std::cout << "solution: fx=" << fx << " " << x << std::endl;

		int n = 0;
		int m = 0;
		while (1)
		{
			n++;
			// copy current solution to next solution
			y = x;
			// move/mutate to try a new solution
			mutate(y, uni);
			// score our new solution
			double fy = tour_dist(cities, y);

			// always save the best solution!
#pragma omp critical
			if (fy > fbest)
			{
				best = y;
				fbest = fy;
			}

			// if this solution is better or
			// if we bump to get past local minimum
			double pct = p(n,C,fy,fx);
			if (fy > fx || unir(rng) < pct)
			{
				// reset iterations since a better solution
				m = 0;
				// save new solution as the reference
				// solution and continue
				x = y;
				fx = fy;
				std::cout << "new solution: iteration=" << n << " fx=" << fx << " " << x << std::endl;
			}
			else
			{
				// increse the count of number of times since a
				// better solution
				m++;
			}

			// if we have gone too many iterations without improving
			// then we need to break out of the loop
			if (m >= M)
			{
				std::cout << "m=M after " << n << " iterations." << std::endl;
				std::cout << "local: fx=" << fx << std::endl;
				std::cout << "local: x=" << x << std::endl;
				std::cout << "global: fbest=" << fbest << std::endl;
				std::cout << std::endl;
				break;
			}
		}
	}

	std::cout << "matlab script to view solution:" << std::endl << std::endl;
	std::cout << "cities=" << cities << ";" << std::endl;
	std::cout << "best=" << best << ";" << std::endl;
	std::cout << "scatter(cities(:,1),cities(:,2))" << std::endl;
	std::cout << "hold on" << std::endl;
	std::cout << "plot(cities(best,1),cities(best,2))" << std::endl;
	std::cout << "text(cities(:,1)+1,cities(:,2)+1,cellstr(num2str([1:length(cities)]')))" << std::endl;
	std::cout << "hold off" << std::endl;

	return 0;
}
