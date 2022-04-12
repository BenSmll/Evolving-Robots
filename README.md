# Evolving-Robots
An implementation of a genetic algorithm in C++.

A assortment of robots is given randomly-generated genes that determine their behavior in regards to tiles adjacent to them on the board.
Robots require energy to survive. Robots acquire energy by moving over a battery.
Robots that survive longer are deemed more "fit", and are prioritized for breeding.
Selectively breeding the highest-fitness robots causes the average fitness of the population to increase over time, meaning that robots will get progressively better at finding batteries.
