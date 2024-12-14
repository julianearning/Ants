#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include "enums.hpp"
#include "../Points/Point.hpp"
#include <iostream>
#include <array>
#include <memory>
#include <vector>

struct Food {
    std::vector<Point2D<int>> food_locations;
    std::vector<unsigned int> remaining_lifetime;

    Food(unsigned int remaining_lifetime_init): remaining_lifetime{remaining_lifetime} {}
    Food()=default;
};

template<size_t Row, size_t Col>
class Environment {
public:
    using Array2d = std::array<std::array<EnvironmentStates, Col>, Row>;
private:
    std::shared_ptr<Array2d> environment;

    Food food;
    const double food_radius{5};
    const unsigned int food_lifetime{5};

public: 
    Environment() : environment(std::make_shared<Array2d>(Array2d{{EnvironmentStates::Empty}})){
        int i=0;
        for(i = 0; i<Row; i++) {
            (*environment)[i][0]=EnvironmentStates::Border;
            (*environment)[i][Col-1]=EnvironmentStates::Border;
        }
        for(i=0; i<Col; i++) {
            (*environment)[0][i]=EnvironmentStates::Border;
            (*environment)[Row-1][i]=EnvironmentStates::Border;
        }
        generate_food();
    };
    std::shared_ptr<const Array2d> getEnvironment() const {
        return environment;
    }
    std::vector<Point2D<int>>& get_food_locations() {
        return food.food_locations;
    }

    void generate_food() {
        Point2D<double> location {static_cast<double>(rand()%(Row-1)), static_cast<double>(rand()%(Col-1))};

        for(int i = (location[0]-food_radius+1); (i>0) && (i<(Row-1)) && (i<(location[0]+food_radius)); i++) {
            int offset = food_radius - std::abs(location[0]-i);
            for(int j = location[1]-offset+1; (j>0) && (j<(Col-1)) && (j<(location[1]+offset)); j++) {
                (*environment)[i][j]=EnvironmentStates::Food;
                food.food_locations.push_back({i,j});
            }
        }
    }


    void printEnvironment() {
        for(int i = 0; i<Row; i++) {
            for(int j = 0; j<Col; j++) {
                switch((*environment)[i][j]) {
                    case EnvironmentStates::Empty:
                        std::cout<<" - ";
                        break;
                    case EnvironmentStates::Border:
                        std::cout<<" # ";
                        break;
                    default:
                        std::cout<<" ? ";
                }
            }
            std::cout<<"\n";
        }
    }
};


#endif