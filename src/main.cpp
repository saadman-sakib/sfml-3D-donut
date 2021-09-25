#include <SFML/Graphics.hpp>
#include <iostream>
#include<bits/stdc++.h>
#include <math.h>
using namespace std;

vector<float> matmul(vector<vector<float>> A, vector<float> B) {
    int R1 = A.size();
    int R2 = B.size();
    vector<float> rslt(R1,0);
    
    for (int i = 0; i < R1; i++) {
            for (int k = 0; k < R2; k++) {
                rslt[i] += A[i][k] * B[k];
            }
    }
    return rslt;
}

float dot(vector<float> A, vector<float> B) {
    float rslt = 0;
    for (int i = 0; i < 3; i++){
        rslt += A[i] * B[i];
    }
    return rslt;
}

vector<vector<float>> X_ROTATION(float theta_x){
    return {
        {1,0,0},
        {0,cos(theta_x),-sin(theta_x)},
        {0,sin(theta_x),cos(theta_x)}
    };
}

vector<vector<float>> Y_ROTATION(float theta_y){
    return {
        {cos(theta_y),0,sin(theta_y)},
        {0,1,0},
        {-sin(theta_y),0,cos(theta_y)}
    };
}

vector<vector<float>> Z_ROTATION(float theta_z){
    return {
        {cos(theta_z),-sin(theta_z),0},
        {sin(theta_z),cos(theta_z),0},
        {0,0,1}
    };
}

class Toroid{
    private:
        vector<vector<float>> points;
        vector<vector<float>> surface_normals;
        vector<sf::CircleShape> dots;
        vector<float> light = {0,-1,-1};
        float distance = 40000;
        float phi = 0;
        float increment = 360/100;

        vector<pair<float, float>> in_2D(float depth){
            vector<pair<float, float>> twoD_points;
            for(auto &point:this->points){
                float x=point[0], y=point[1], z=point[2];
                twoD_points.push_back({x*depth/(z+depth), y*depth/(z+depth)});
            }
            return twoD_points;
        }

        vector<float> luminance(){
            vector<float> luminance_points;
            for(auto &point:this->surface_normals){
                luminance_points.push_back(dot(point,light));
            }
            return luminance_points;
        }
        
    public:
        Toroid(float R1, float R2){
            while (phi < 360)
            {
                float theta = 0;
                vector<vector<float>> circle_points;
                vector<vector<float>> normal_points;
                
                while (theta < 360){
                    circle_points.push_back({R2+R1*cos(theta),R1*sin(theta),0});
                    normal_points.push_back({cos(theta),sin(theta),0});
                    theta += 3*increment;
                }

                for(auto &point:circle_points){
                    this->points.push_back(matmul(Y_ROTATION(phi),point));
                }

                for(auto &point:normal_points){
                    this->surface_normals.push_back(matmul(Y_ROTATION(phi),point));
                }

                phi += 1.5*increment;
            }
        }

        void init(){
            for (unsigned i = 0; i < this->points.size(); i++){
                this->dots.push_back(sf::CircleShape(1));
            }
        }

        void update_y(float theta_y){
            for(auto &point:this->points){
                point = matmul(Y_ROTATION(theta_y),point);
            }
            for(auto &point:this->surface_normals){
                point = matmul(Y_ROTATION(theta_y),point);
            }
        }

        void update_x(float theta_x){
            for(auto &point:this->points){
                point = matmul(X_ROTATION(theta_x),point);
            }
            for(auto &point:this->surface_normals){
                point = matmul(X_ROTATION(theta_x),point);
            }
        }

        void draw(sf::RenderWindow &win){
            vector<pair<float,float>> points_to_draw = this->in_2D(this->distance);
            vector<float> luminances = this->luminance();
            for (unsigned i = 0; i < points_to_draw.size(); i++){
                this->dots[i].setPosition(points_to_draw[i].first+win.getSize().x/2, points_to_draw[i].second+win.getSize().y/2);
                if(luminances[i]>0){
                this->dots[i].setRadius(luminances[i]*2.9+.5);
                } else {
                    this->dots[i].setRadius(.5);
                }
                win.draw(this->dots[i]);
            }
        }


        sf::CircleShape x;
        void test(){
            this->x.setPosition(100,200);
            this->x.setRadius(200);
        }
        void test_draw(sf::RenderWindow &win){
            win.draw(x);
        }
};


int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "window Name", sf::Style::Close | sf::Style::Resize | sf::Style::Titlebar);

    Toroid toroid = Toroid(80, 200);
    toroid.init();

    //window main loop
    while (window.isOpen()){

        //event loop (check for events)
        sf::Event event1;
        while (window.pollEvent(event1)){
            if (event1.type == sf::Event::Closed){
                window.close();
            }
        }

        toroid.update_x(.019);
        toroid.update_y(.009);
        
        //window update
        window.clear(sf::Color(0, 12, 25));
        toroid.draw(window);
        window.display();
    }

    return 0;
}