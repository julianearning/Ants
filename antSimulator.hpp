#ifndef ANTSIMULATOR_HPP_
#define ANTSIMULATOR_HPP_


#include <SFML/Graphics.hpp>
#include <cassert>
#include <memory>
#include <chrono>
#include <thread>

#include "../Points/Point.hpp"
#include "environment.hpp"
#include "ants.hpp"


class Grey_Gradient {
public:
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
    Grey_Gradient(int decay, int max_value) { // the smaller the decay, the more faded is the grey
        alpha=10;
        red=((max_value-decay)/max_value) * 255; 
        green=red;
        blue=red;
    }
};


template<size_t Row, size_t Col>
class AntSimulator {
private:
    Environment<Row, Col> environment;
    std::shared_ptr<const typename Environment<Row,Col>::Array2d> environment_ptr; //= environment.getEnvironment();
    std::vector<Ant<Row, Col>> ants;
    double ms_per_frame = 16.66;
    Point2D<double> nest_position;



    // SFML stuff
    sf::RenderWindow window;
    sf::Texture dirtTexture;
    sf::Texture stoneTexture;
    sf::Sprite stoneSprite;
    sf::Texture antTexture1;
    sf::Texture antTexture2;
    sf::Texture sugarTexture;
    sf::Texture nestTexture;
    std::vector<sf::Sprite> dirtSprites;
    std::vector<sf::Sprite> antSprites1;
    std::vector<sf::Sprite> antSprites2;
    std::vector<sf::Sprite> sugarSprites;
    sf::Sprite nestSprite;

    bool alternate{true};

    static double max(const double a, const double b) {
        return (a>b)?a:b;
    }
public:
    AntSimulator(const double fps, const size_t number_ants) : environment_ptr(environment.getEnvironment()), window(sf::VideoMode(Row,Col), "Ants!") {
        assert((number_ants>0)&&"num_ants must be at least 1.");
        assert((fps>0)&&"fps must be a positive value.");

        if(!dirtTexture.loadFromFile("textures/dirt.jpeg")) {
            std::cout<<"Error: dirt texture cannot be opened\n";
        }
        if(!stoneTexture.loadFromFile("textures/stone.jpeg")) {
            std::cout<<"Error: stone texture cannot be opened\n";
        }
        if(!antTexture1.loadFromFile("textures/antwalk1.png")) {
            std::cout<<"Error: ant texture 1 cannot be opened\n";
        }
        if(!antTexture2.loadFromFile("textures/antwalk2.png")) {
            std::cout<<"Error: ant texture 2 cannot be opened\n";
        }
        if(!nestTexture.loadFromFile("textures/anthill.png")) {
            std::cout<<"Error: anthill texture cannot be opened\n";
        }
        if(!sugarTexture.loadFromFile("textures/sugar.png")) {
            std::cout<<"Error: sugar texture cannot be opened\n";
        }

        //std::cout<<(Row / 50)+1<<" "<<(Col / 50)+1<<"\n";
        for(int i = 0; i<(static_cast<int>(Row * 0.02)+1); i++) { // images are about 50x50
            for(int j = 0; j<(static_cast<int>(Col * 0.02)+1); j++) {
                dirtSprites.push_back(sf::Sprite());
                dirtSprites.at(dirtSprites.size()-1).setTexture(dirtTexture);
                dirtSprites.at(dirtSprites.size()-1).setPosition(static_cast<int>(i)+49*i, static_cast<int>(j)+49*j);
                //std::cout<<static_cast<int>(i)+50*i<<"  "<<static_cast<int>(j)+50*j<<"\n";
            }
        }

        for(auto food_location : environment.get_food_locations()) {
            sugarSprites.push_back(sf::Sprite());
            sugarSprites.at(sugarSprites.size()-1).setTexture(sugarTexture);
            sugarSprites.at(sugarSprites.size()-1).setPosition(food_location[0], food_location[1]);
        }



        ms_per_frame=0.001*fps;
        srand(time(NULL));
        nest_position[0]=(rand()%(static_cast<int>(Row*0.5)))+Row*0.25;
        nest_position[1]=(rand()%(static_cast<int>(Col*0.5)))+Col*0.25;
        Point2D<double> border;
        
        nestSprite.setTexture(nestTexture);
        nestSprite.setPosition(nest_position[0]-30, nest_position[1]-30);
        nestSprite.scale(3.0,3.0);


        for(int i = 0; i<number_ants; i++) {
            ants.push_back( Ant<Row, Col>(nest_position , environment.getEnvironment()));

            antSprites1.push_back(sf::Sprite());
            antSprites1.at(i).setTexture(antTexture1);
            antSprites1.at(i).setPosition(nest_position[0], nest_position[1]);

            antSprites2.push_back(sf::Sprite());
            antSprites2.at(i).setTexture(antTexture2);
            antSprites2.at(i).setPosition(nest_position[0], nest_position[1]);

            const Point2D<double>& curr_direction=ants.at(i).get_curr_direction();
            antSprites1.at(i).rotate(atan2(curr_direction[1], curr_direction[0]));
            antSprites2.at(i).rotate(atan2(curr_direction[1], curr_direction[0]));

            antSprites1.at(i).scale(0.5,0.5);
            antSprites2.at(i).scale(0.5,0.5);
        }
    }

    void draw_environment() {
        for(int i = 0; i<dirtSprites.size(); i++) {
            window.draw(dirtSprites.at(i));
        }

        
        window.draw(nestSprite);

        for(int i = 0; i<sugarSprites.size(); i++) {
            window.draw(sugarSprites.at(i));
        }

        //window.display();
        //using namespace std::literals;
        //std::this_thread::sleep_for(1min);
    }

    
    void draw_ants() {
        const int length_trail=1000;
        sf::CircleShape circle;
        circle.setRadius(1);
        for(int i = 0; i<ants.size(); i++) {    
            
            // draw ants, alternating texture for really basic 'animation'
            if(alternate) {
                window.draw(antSprites1.at(i));
            } else {
                window.draw(antSprites2.at(i));
            }


            Point2D<double> ant_position=ants.at(i).get_position();
            const std::vector<AntStep>& ant_trail=ants.at(i).get_trail();
            for(int j = (ant_trail.size()-1); j>=max(0,static_cast<int>(ant_trail.size())-1-length_trail); j--) {
                Grey_Gradient gg(j, length_trail);
                ant_position+=AntStepHelper::get_step(ant_trail.at(j));
                circle.setPosition(ant_position[0], ant_position[1]);
                circle.setFillColor(sf::Color(gg.red,gg.green,gg.blue,gg.alpha));
                window.draw(circle);
            }
        }
        if(alternate) {
            alternate=false;
        } else {
            alternate=true;
        }
    }

    
    void simulate() {

        // Start the game loop
        while (window.isOpen())
        {
            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                // Close window: exit
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            // Clear screen
            //window.clear(sf::Color::White);

            auto l1 = std::chrono::high_resolution_clock::now();
            
            draw_environment();
            draw_ants();

            // Update the window
            window.display();


            for(int i = 0; i<ants.size(); i++) {
                ants.at(i).make_ant_take_step();
                Point2D<double> curr_position=ants.at(i).get_position();
                antSprites1.at(i).setPosition(curr_position[0], curr_position[1]);
                antSprites2.at(i).setPosition(curr_position[0], curr_position[1]);
                const Point2D<double>& curr_direction=ants.at(i).get_curr_direction();
                antSprites1.at(i).setRotation(57.29*atan2(curr_direction[1], curr_direction[0]));
                antSprites2.at(i).setRotation(57.29*atan2(curr_direction[1], curr_direction[0]));
            }

            auto l2 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(l2 - l1);

            if(ms_int.count()<=16.6) {
                std::this_thread::sleep_for(std::chrono::milliseconds((int)(ms_per_frame-ms_int.count())));
            } 
        }
    
    }
};

#endif