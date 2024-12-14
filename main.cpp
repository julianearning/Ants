#include "environment.hpp"
#include "antSimulator.hpp"

int main() {
    const double fps=120;
    const size_t number_ants=100;
    AntSimulator<1780, 780> as{static_cast<double>(60.0),static_cast<size_t>(number_ants)};
    //as.draw_ants();
    as.simulate();    
    //as.draw_environment();

    return 0;
}