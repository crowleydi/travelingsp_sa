# Solving Traveling Salesperson Problem with Simulated Annealing

This uses the Simulated Annealing optimization method (SA) to solve
the Traveling Salesperson problem. It is written in C++ and uses
OpenMP for multi-threading. It doesn't produce the optimal solution
but it can produce a really good solution.

The output is a simple Matlab script that can be copy and pasted
into Matlab to display the solution.

The problem of the Traveling Salesman is difficult to solve because
it is NP. The only way to know if you have the best solution is to
try all of the solutions. The number of possible solutions is equal
to (N-1)! where N is the number of cities. For a small number of
cities this is feasible. For 7 cities the number of possible solutions
is 720 of which 2 are equally best solutions where one best solution
is in the opposite order of the other best solution.

For the given sample problem, this program comes up with the following
as the best solution:
![image](https://user-images.githubusercontent.com/14304909/220192650-36e5be2a-26c2-47b9-a730-8698c7996b57.png)

For the siven simple 7 city problem above, it is a little bit difficult
to understand what changes to C do to the solution. But by looking at
the equation for P and running the program solutions with more cities we
are able to determine that lower values for C give us more movement. As
C → 0, p → 1 so there is a high probability that if we are at a solution
x we will move to another solution y which may not be quite as good as
solution x.

The value N determines the number of starting points we generate. As N
increases, we have a better change of finding a really good solution. A
larger N helps us better cover the entire solution space. For the 7 city
problem, N=10 and C=1 worked adequate and the code finds the best solution
nearly every time.

When running the code for 50 cities, values of C that are too low cause
the program to run for too long because it jumps so much. For 50 cities,
C=0.04, N=400 worked really well and often returned really good solutions
where the criteria for a really good solution is one where the path never
crosses itself, as shown below:

![image](https://user-images.githubusercontent.com/14304909/220192405-f5dc4a99-7556-4c1c-8940-07be9b306816.png)

The value M is the number of iterations the code will run without finding
a better solution. The break condition for the loop is to run until the number
of iterations since the last better solution is equal to M. For 50 cities, a
value that seemes to work well for M is 1000.

The code is written in C++ and is multi-threaded by using openmp. As a result it
runs quite fast! It can generate really good solutions to a 50 city problem
in under 20 seconds. A sample run is shown below:
```
$ time ./tsp 50 400 0.04 1000 > tsp.txt

real	0m12.153s
user	0m49.010s
sys	0m28.928s
$ tail tsp.txt

matlab script to view solution:

cities=[97 55;38 48;46 63;10 25;32 36;71 11;52 9;93 64;82 35;69 2;40 1;48 90;44 52;95 100;56 15;89 50;53 34;27 99;99 20;25 67;68 99;53 2;17 6;76 74;8 49;74 63;52 27;74 4;11 21;4 60;46 47;47 12;74 8;3 89;61 13;80 16;83 7;24 28;89 68;87 30;30 94;12 47;86 31;3 5;95 66;17 70;50 11;18 98;41 85;60 68;];
best=[ 1 8 45 14 21 12 49 41 18 48 34 46 20 30 25 42 2 5 38 4 29 44 23 11 22 7 47 32 15 27 17 31 13 3 50 26 24 39 16 9 43 40 36 33 6 35 10 28 37 19 1 ];
scatter(cities(:,1),cities(:,2))
hold on
plot(cities(best,1),cities(best,2))
text(cities(:,1)+1,cities(:,2)+1,cellstr(num2str([1:length(cities)]')))
hold off
```
