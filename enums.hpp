#ifndef ENUMS_HPP_
#define ENUMS_HPP_

enum class EnvironmentStates : unsigned char {Empty, Food, Border, Trail, Pheromone, Ant};
enum class AntState : unsigned char { Seeking, Following, Returning };
enum class AntStep : unsigned char { Up, Right, Down, Left, NR_ITEMS };
enum class ReverseAntStep : unsigned char { Down, Left, Up, Right, NR_ITEMS };


#endif