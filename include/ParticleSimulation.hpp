#ifndef PARTICLE_SIMULATION
#define PARTICLE_SIMULATION

#include "Particle.hpp"
#include "QuadTree.hpp"
#include "Profiler.hpp"

#include <vector>
#include <thread>
#include <random>   // std::random_device
#include <cmath>    // std::pow()

class ParticleSimulation
{
private:
    sf::RenderWindow* game_window_;
    int num_threads_;
    int tree_max_depth_;

    int simulation_width_;
    int simulation_height_;

    sf::View game_view_;
	
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;

    float time_step_;
    float particle_mass_;

    sf::Vector2f global_com_;

    sf::Vector2f current_mouse_pos_f_;
    sf::Vector2f initial_mouse_pos_f_;
    sf::Vector2f final_mouse_Pos_f;
    sf::Vector2f scroll_mouse_pos_f_;

    bool is_right_button_pressed_;
    bool is_middle_button_pressed_;
    bool is_aiming_;
    bool show_velocity_;
    bool show_quad_tree_;
    bool show_particles_;
    bool is_paused_;

    sf::Font font_;
    sf::Text particle_count_text_;
    sf::Text particle_mass_text_;
    sf::Text velocity_text_;
    sf::Text is_paused_text_;

    sf::Event event_;

    std::vector<std::thread> threads_;
    std::vector<QuadTree::TreeNode*> quad_tree_leaf_nodes_;
    std::vector<Particle> particles_;

    QuadTree quad_tree_;

public:
    ParticleSimulation(int simulation_width,
                       int simulation_height,
                       sf::RenderWindow &window,
                       int num_threads,
                       float dt,
                       int tree_depth,
                       int node_cap);

    ~ParticleSimulation();

    void run();
    void pollUserEvent();
    void updateAndDraw();

    inline void drawAimLine();
    inline void drawParticleVelocity();

    void updateForces(float total_mass);

    void addSierpinskiTriangleParticleChunk(int x, int y, int size, int depth);
    void addCheckeredParticleChunk();
    void addParticleDiagonal(int tiles, int num_particles);
    void addParticleDiagonal2(int tiles, int num_particles);
};

#endif
