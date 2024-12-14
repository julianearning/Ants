#ifndef ANTS_HPP_
#define ANTS_HPP_

#include "enums.hpp"
#include "../Points/Point.hpp"
#include "environment.hpp"
#include <memory>
#include <algorithm>


class AntStepHelper {
public:
    static AntStep get_opposite_step(const AntStep as) {
        switch(as) {
            case AntStep::Down:
                return AntStep::Up;
            case AntStep::Up:
                return AntStep::Down;
            case AntStep::Left:
                return AntStep::Right;
            case AntStep::Right:
                return AntStep::Left;
            default:
                return AntStep::NR_ITEMS;
        }
    }
    static Point2D<double> get_step(const AntStep as) {
        switch(as) {
            case AntStep::Down:
                return Point2D<double>{0,1} ;
            case AntStep::Up:
                return Point2D<double>{0,-1} ;
            case AntStep::Left:
                return Point2D<double>{-1,0} ;
            case AntStep::Right:
                return Point2D<double>{1,0} ;
        }
        return Point2D<double>{-99.0,-99.0};
    }
};



template<size_t Row, size_t Col>
class Ant {
private:    
    Point2D<double> position;
    AntState state{AntState::Seeking};
    std::vector<AntStep> path;  // keeps the opposite AntSteps, to retrace the path
    std::shared_ptr<const typename Environment<Row,Col>::Array2d> environment_ptr;  // allows for read access to the environment

    Point2D<double> curr_direction;
    const double prob_random_move=0.01;
    double random_double{0.5};

    void reset_curr_direction() {
        curr_direction[0]= (static_cast<double>(rand()%200)*0.01)-1;
        curr_direction[1]= (static_cast<double>(rand()%200)*0.01)-1;
    }

    AntStep choose_random_move() {
        if((static_cast<double>(rand()%100)*0.01)<prob_random_move) { // exploratory move
            AntStep as = static_cast<AntStep>(rand()%static_cast<int>(AntStep::NR_ITEMS));
            curr_direction[0]=curr_direction[0]+(static_cast<double>(rand()%100)*0.01)-0.5;
            curr_direction[1]= curr_direction[1]+(static_cast<double>(rand()%100)*0.01)-0.5;
            random_double=static_cast<double>(rand()%100)*0.01;
            return as;
        } else {  // move according to stategy (follow curr_direction)
            if((curr_direction[0]>=0)&&(curr_direction[1]>=0)) {  // it could either be Down or Right
                return (((rand()%100)*0.01)>random_double) ? AntStep::Down : AntStep::Right;
            } else if((curr_direction[0]>=0)&&(curr_direction[1]<0)) { // it could either be Up or Right
                return (((rand()%100)*0.01)>random_double) ? AntStep::Up : AntStep::Right;
            } else if((curr_direction[0]<0) && (curr_direction[1]>=0)) {  // it could either be Left or Down
                return (((rand()%100)*0.01)>random_double) ? AntStep::Down : AntStep::Left;
            } else if((curr_direction[0]<0) && (curr_direction[1]<0)) {  // it could either be Left or Up
                return (((rand()%100)*0.01)>random_double) ? AntStep::Up : AntStep::Left;
            } else {
                return AntStep::Down;
            }
        }
    }

   AntStep check_environment(AntStep as) {
        Point2D<double> offset = AntStepHelper::get_step(as);
        switch((*environment_ptr)[offset[0]][offset[1]]) {
            case EnvironmentStates::Border:
                return AntStepHelper::get_opposite_step(as);
            case EnvironmentStates::Food:
                state=AntState::Returning;
                return as;
        }
        return as;
   }

    void make_step(AntStep as) {
        path.push_back(AntStepHelper::get_opposite_step(as));
        position+=AntStepHelper::get_step(as);
    }

    void random_walk() {
        // Generate a random number and cast to AntStep
        AntStep as = choose_random_move();
        as = check_environment(as);
        make_step(as);
    } 

    void retrace_steps() {
        if(path.empty()) state=AntState::Seeking;
        AntStep as = path.back();
        make_step(as);
        set_pheromone_trail();
        path.pop_back();
    }

    void set_pheromone_trail() {
        return;
    }

public:
    Ant(const Point2D<double>& nestposition,std::shared_ptr<const typename Environment<Row, Col>::Array2d> env) 
            : position(nestposition), environment_ptr(env), curr_direction{(static_cast<double>(rand()%200)*0.01)-1.0, (static_cast<double>(rand()%200)*0.01)-1.0}
                 {};

    Point2D<double> get_position() const {
        return position;
    }

    const Point2D<double>& get_curr_direction() {
        return curr_direction;
    }

    const std::vector<AntStep>& get_trail() {
        return path;
    }

    void make_ant_take_step() {
        switch(state) {
            case AntState::Seeking:
                random_walk();
                break;
            case AntState::Returning:
                retrace_steps();
                break;
        }
    }

};


#endif 