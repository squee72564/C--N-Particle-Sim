#include <iostream>
#include <SFML/Graphics.hpp>

#include "ParticleSimulation.hpp"

static const float BIG_G = 35.00f;

static inline sf::Vector2f getMousePosition(const sf::RenderWindow &window)
{
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

template <typename T>
static inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

static inline float inv_Sqrt(float number)
{
    float squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}

ParticleSimulation::ParticleSimulation(int simulation_width,
                                       int simulation_height,
                                       sf::RenderWindow &window,
                                       int num_threads,
                                       float dt,
                                       int tree_depth,
                                       int node_cap)
  : num_threads_(num_threads),
    tree_max_depth_(tree_depth),
    simulation_width_(simulation_width),
    simulation_height_(simulation_height),
    game_view_(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize())),
    gen_(std::mt19937(rd_())),
    dis_(std::uniform_int_distribution<>(0, 255)),
    time_step_(dt),
    particle_mass_(1.03f),
    global_com_(sf::Vector2f(0.0f, 0.0f)),
    current_mouse_pos_f_(sf::Vector2f(0.0f, 0.0f)),
    initial_mouse_pos_f_(sf::Vector2f(0.0f, 0.0f)),
    final_mouse_Pos_f(sf::Vector2f(0.0f, 0.0f)),
    is_right_button_pressed_(false),
    is_middle_button_pressed_(false),
    is_aiming_(false),
    show_velocity_(false),
    show_quad_tree_(true),
    show_particles_(true),
    is_paused_(true),
    font_(),
    threads_(),
    quad_tree_leaf_nodes_(),
    particles_(),
    quad_tree_(QuadTree(simulation_width, simulation_height, tree_depth, node_cap))
{
    game_window_ = &window;

    font_.loadFromFile("fonts/corbel.TTF");
    threads_.reserve(num_threads);
    quad_tree_leaf_nodes_.reserve(pow(4,tree_depth));
    particles_.reserve(200000);

    particle_count_text_.setFont(font_);
    particle_count_text_.setCharacterSize(24);
    particle_count_text_.setFillColor(sf::Color::White);
    particle_count_text_.setOutlineColor(sf::Color::Blue);
    particle_count_text_.setOutlineThickness(1.0f);

    particle_mass_text_.setFont(font_);
    particle_mass_text_.setCharacterSize(12);
    particle_mass_text_.setFillColor(sf::Color::White);
    particle_mass_text_.setPosition(0, 100);
    particle_mass_text_.setOutlineColor(sf::Color::Blue);
    particle_mass_text_.setOutlineThickness(1.0f);

    velocity_text_.setFont(font_);
    velocity_text_.setCharacterSize(10);
    velocity_text_.setFillColor(sf::Color::White);

    is_paused_text_.setFont(font_);
    is_paused_text_.setCharacterSize(24);
    is_paused_text_.setFillColor(sf::Color::White);
    is_paused_text_.setOutlineColor(sf::Color::Red);
    is_paused_text_.setOutlineThickness(1.0f);
    is_paused_text_.setString("Paused");
    is_paused_text_.setPosition(0, 50);
}

ParticleSimulation::~ParticleSimulation()
{
    if (!particles_.empty())
        particles_.clear();

    if (!quad_tree_leaf_nodes_.empty())
        quad_tree_leaf_nodes_.clear();

    if (!threads_.empty())
        threads_.clear();
}

void ParticleSimulation::run()
{
    //addCheckeredParticleChunk();

    addSierpinskiTriangleParticleChunk((simulation_width_-simulation_height_)/2, 0, simulation_height_, 11);
    
    while (game_window_->isOpen())
    {
        pollUserEvent();
        updateAndDraw();
    }
}

void ParticleSimulation::pollUserEvent()
{
    while (game_window_->pollEvent(event_))
    {
        int scroll_delta = 0;

        switch (event_.type)
        {
            case sf::Event::Closed:
                game_window_->close();
                break;

            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    particle_mass_ -= 0.1f;
                    if (particle_mass_ < 0.1f) particle_mass_ = 0.1f;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    particle_mass_ += 0.1f;
                    if (particle_mass_ > 20.0f) particle_mass_ = 20.0f;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    if (quad_tree_.getMaxDepth() > 0) quad_tree_.setMaxDepth(quad_tree_.getMaxDepth()-1);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    if (quad_tree_.getMaxDepth() < tree_max_depth_) quad_tree_.setMaxDepth(quad_tree_.getMaxDepth()+1);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
                {
                    show_velocity_ = (show_velocity_) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
                {
                    show_quad_tree_ = (show_quad_tree_) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
                {
                    show_particles_ = (show_particles_) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    is_paused_ = (is_paused_) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
                {
                    show_particles_ = (show_particles_) ? false : true;
                }

                break;

            case sf::Event::MouseButtonReleased: // RMB or LMB released

                if (is_right_button_pressed_ && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    is_right_button_pressed_ = false;
                }

                if (is_aiming_ && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    is_aiming_ = false;
                    final_mouse_Pos_f = getMousePosition(*game_window_);
                    particles_.emplace_back(Particle(initial_mouse_pos_f_, (initial_mouse_pos_f_-final_mouse_Pos_f), particle_mass_));
                }

                if (is_middle_button_pressed_ && !sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    is_middle_button_pressed_ = false;
                }
 
                break;

            case sf::Event::MouseButtonPressed:	// RMB or LMB pressed
						
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    is_right_button_pressed_ = true;
                    current_mouse_pos_f_ = getMousePosition(*game_window_);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    is_aiming_ = true;
                    initial_mouse_pos_f_ = getMousePosition(*game_window_);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    is_middle_button_pressed_ = true;
                    scroll_mouse_pos_f_ = getMousePosition(*game_window_);
                }

                break;
            
            case sf::Event::MouseWheelScrolled:	// Scroll in

                scroll_delta = event_.mouseWheelScroll.delta;

                if (scroll_delta < 0) game_view_.move((getMousePosition(*game_window_) - game_view_.getCenter()) * 0.13f);

                game_view_.zoom(1 + (event_.mouseWheelScroll.delta*0.05f));
                game_window_->setView(game_view_);

                break;
            
            default:
                break;
        }
    }
}

// These are some constants used for rendering particle triangles
#define P_RADIUS_DIV_2 (0.5f / 2.0f)
#define TRI_X_OFFSET ((0.5f * std::sqrt(3.0f) / 2.0f)) 

DEFINE_API_PROFILER(PopAndSwap);
DEFINE_API_PROFILER(InsertIntoQuadTree);
DEFINE_API_PROFILER(UpdateForces);
DEFINE_API_PROFILER(DrawParticles);
DEFINE_API_PROFILER(DrawVelocities);
DEFINE_API_PROFILER(DrawQuadTree);
DEFINE_API_PROFILER(DeleteQuadTree);

void ParticleSimulation::updateAndDraw()
{
    game_window_->clear();

    {
        API_PROFILER(DeleteQuadTree);
        quad_tree_.deleteTree();
    }

    quad_tree_leaf_nodes_.clear();

    if (is_right_button_pressed_ || is_aiming_) {
        current_mouse_pos_f_ = getMousePosition(*game_window_);
    }

    if (is_middle_button_pressed_) {
        game_view_.move((scroll_mouse_pos_f_ - getMousePosition(*game_window_)) * 0.07f);
        game_window_->setView(game_view_);
    }

    {
        API_PROFILER(PopAndSwap);
        for (std::size_t i = 0; i < particles_.size(); ++i) {

            if (particles_[i].position.x < 0 || particles_[i].position.x > simulation_width_ ||
                particles_[i].position.y > simulation_height_ || particles_[i].position.y < 0) {

                std::swap(particles_[i], particles_.back());
                particles_.pop_back();
                --i;
            }
        }
    }

    global_com_.x = 0;
    global_com_.y = 0;

    {
        API_PROFILER(InsertIntoQuadTree);
        quad_tree_.insert(particles_);
    }
    
    int total_leaf_nodes = 0;
    float global_mass = 0.0f;

    global_com_ = quad_tree_.getLeafNodes(quad_tree_leaf_nodes_, total_leaf_nodes, global_mass);

    if (!is_paused_) {

        if (!particles_.empty()) {
            API_PROFILER(UpdateForces);
            updateForces(global_mass);
        }

    }

    if (!particles_.empty() && show_particles_) {


        {
            API_PROFILER(DrawParticles);
            sf::VertexArray particles_vertices(sf::Triangles, particles_.size() * 3);

            if (particles_.size() >= 8E5) {
                const std::size_t chunk_size = particles_.size() / num_threads_;
                const std::size_t remainder = particles_.size() % num_threads_;

                for (int i = 0; i < num_threads_; i++) {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index = (i==num_threads_-1) ? start_index + chunk_size + remainder : start_index + chunk_size;
                    
                    auto thread_function = [this, start_index, end_index, &particles_vertices]() {
                        std::size_t vi = start_index*3;
                        
                        for (std::size_t i = start_index; i < end_index; ++i) {
                            const float center_x = particles_[i].position.x;
                            const float center_y = particles_[i].position.y;
                            const float y_pos = center_y - P_RADIUS_DIV_2;

                            // Right now to lower time for drawing function we are only drawing a triangle
                            // where the particle circle would be inscribed within the triangle. This will lead total
                            // some visual overlap close to the triangle vertices when particles_ are not actually overlapping,
                            // but allows us to use a vertex array of triangles with only 3 vertices per particle for a batch render
                            
                            // Top vertex
                            particles_vertices[vi].position.x = center_x;
                            particles_vertices[vi].position.y = center_y + 0.5f;
                            particles_vertices[vi++].color = particles_[i].color;

                            // Left vertex
                            particles_vertices[vi].position.x = center_x - TRI_X_OFFSET;
                            particles_vertices[vi].position.y = y_pos;
                            particles_vertices[vi++].color = particles_[i].color;

                            // Right vertex
                            particles_vertices[vi].position.x = center_x + TRI_X_OFFSET;
                            particles_vertices[vi].position.y = y_pos;
                            particles_vertices[vi++].color = particles_[i].color;
                           
                        }
                    };

                    threads_.emplace_back(thread_function);
                }

                for (auto& thread : threads_)
                {
                    thread.join();
                }
            } else {
                std::size_t vi = 0;
                for (std::size_t i = 0; i < particles_.size(); ++i) {
                    const float center_x = particles_[i].position.x;
                    const float center_y = particles_[i].position.y;
                    const float y_pos = center_y - P_RADIUS_DIV_2;

                    // Top vertex
                    particles_vertices[vi].position.x = center_x;
                    particles_vertices[vi].position.y = center_y + 0.5f;
                    particles_vertices[vi++].color = particles_[i].color;

                    // Left vertex
                    particles_vertices[vi].position.x = center_x - TRI_X_OFFSET;
                    particles_vertices[vi].position.y = y_pos;
                    particles_vertices[vi++].color = particles_[i].color;

                    // Right vertex
                    particles_vertices[vi].position.x = center_x + TRI_X_OFFSET;
                    particles_vertices[vi].position.y = y_pos;
                    particles_vertices[vi++].color = particles_[i].color;
                }
            }

            game_window_->draw(particles_vertices);

            threads_.clear();
        }

        if (show_velocity_) {
            API_PROFILER(DrawVelocities);
            drawParticleVelocity();
        }

    }

    if (is_aiming_) {
        drawAimLine();
    }

    if (show_quad_tree_) {
        API_PROFILER(DrawQuadTree);
        quad_tree_.display(game_window_, total_leaf_nodes);

        if (quad_tree_leaf_nodes_.size() != 0) {
            sf::CircleShape circle(20.0f);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(global_com_);
            circle.setFillColor(sf::Color(255,0,0,20));
            game_window_->draw(circle);
        }
    }

    particle_count_text_.setString("Particle count: " + std::to_string(particles_.size()));
    particle_mass_text_.setString("Particle mass: " + std::to_string(particle_mass_));

    game_window_->draw(particle_count_text_);
    game_window_->draw(particle_mass_text_);

    if (is_paused_) {
        game_window_->draw(is_paused_text_);
    }
    game_window_->display();
}

inline void ParticleSimulation::drawAimLine() 
{	
    velocity_text_.setPosition(initial_mouse_pos_f_.x+5.0f, initial_mouse_pos_f_.y);
    float angle =  (initial_mouse_pos_f_.y - current_mouse_pos_f_.y) / (initial_mouse_pos_f_.x - current_mouse_pos_f_.x);
    velocity_text_.setString(std::to_string( abs( ( atan(angle) * 180.0f ) * 0.31831015f) ));

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = initial_mouse_pos_f_;
    line[1].position = current_mouse_pos_f_;
    line[0].color  = sf::Color(0, 255, 0, 155);
    line[1].color = sf::Color(0, 255, 0, 25);
        
    game_window_->draw(velocity_text_);
    game_window_->draw(line);
}

inline void ParticleSimulation::drawParticleVelocity() 
{
    sf::VertexArray lines(sf::Lines, particles_.size()*2);
    
    int pIdx = 0;
    for (std::size_t i = 0; i < particles_.size()*2; i+=2) {
        lines[i].position = particles_[pIdx].position;
        lines[i+1].position = particles_[pIdx].position + particles_[pIdx].velocity * 0.00222f;
        lines[i].color  = sf::Color(0,0,255,85);
        lines[i+1].color = sf::Color(255,0,0,0);

        pIdx++;

    }
   
    game_window_->draw(lines);
}

static inline void attractParticleToMousePos(Particle& particle, sf::Vector2f& current_mouse_pos_f) 
{
    const float distance_squared = dot(particle.position - current_mouse_pos_f,
                                particle.position - current_mouse_pos_f);

    particle.acceleration += (3.5E6f / distance_squared) * 
                                BIG_G * (current_mouse_pos_f - particle.position);

    //particle.velocity -= (particle.position - current_mouse_pos_f) * 0.35f;
}

void ParticleSimulation::updateForces(float global_mass)
{
    const int n_threads = num_threads_;
    
    if  (quad_tree_leaf_nodes_.size() < static_cast<std::size_t>(num_threads_)) {
        num_threads_ = quad_tree_leaf_nodes_.size();
    }
	
    // Divide the leaf nodes up evenly among threads
    // It may be better to load balance based on distribution of particles
    const std::size_t chunk_size = quad_tree_leaf_nodes_.size() / num_threads_;
    const std::size_t remainder = quad_tree_leaf_nodes_.size() % num_threads_;
    
    for (int i = 0; i < num_threads_; i++) {
        
		const std::size_t start_index = i * chunk_size;
        const std::size_t end_index = (i==num_threads_-1) ? start_index + chunk_size + remainder : start_index + chunk_size;
        
        auto thread_function = [this, start_index, end_index, global_mass]() {

            const std::vector<QuadTree::ParticleElementNode>& particle_element_nodes = quad_tree_.getParticleElementNodeVec();

            for (std::size_t j = start_index; j < end_index; j++) {
				
                QuadTree::TreeNode* curr_tree_node = quad_tree_leaf_nodes_[j];

                // First apply gravitational force from all other cells to the particle
                sf::Vector2f new_com(0,0);

                const int non_local_particle_count = (particles_.size() - curr_tree_node->count);
                const float non_local_mass = global_mass - quad_tree_.getNodeTotalMass(curr_tree_node);

                if (non_local_particle_count != 0) {
                    const sf::Vector2f curr_node_com = quad_tree_.getNodeCOM(curr_tree_node);

                    new_com.x = static_cast<float>(global_mass * global_com_.x - curr_node_com.x) /
                                    static_cast<float>(non_local_mass);
                    new_com.y = static_cast<float>(global_mass * global_com_.y - curr_node_com.y) /
                                    static_cast<float>(non_local_mass);
                }

                const int first_particle_idx = curr_tree_node->first_particle;
				
                // Handle Particle to Particle interactions within the leaf node
                for (int k = first_particle_idx; k != -1; k = particle_element_nodes[k].next_element_index) {

                    const int particle_index = particle_element_nodes[k].particle_index;
                    Particle& particle = particles_[particle_index];

                    if (non_local_particle_count != 0) {
                        const float distance_squared = dot(particle.position - new_com,
                                                      particle.position - new_com);

                        particle.acceleration += (non_local_mass / distance_squared) * BIG_G *
                                                        (new_com - particle.position);
                    }

                    for (int l = first_particle_idx; l != -1; l = particle_element_nodes[l].next_element_index) {
						
                        const int other_index = particle_element_nodes[l].particle_index;
                        Particle& other = particles_[other_index];

                        if (&other == &particle) continue;

                        const float distance_squared = dot(particle.position - other.position,
                                                    particle.position - other.position);

                        if (distance_squared < 0.01f) continue;
                        
                        const float radius_squared = 1.0f;

                        const bool is_colliding = (distance_squared <= radius_squared);
                        
                        if (is_colliding) {
                            sf::Vector2f r_hat = (other.position - particle.position) * inv_Sqrt(distance_squared);

                            const float a1 = dot(particle.velocity, r_hat);
                            const float a2 = dot(other.velocity, r_hat);

                            const float p = 2.0f * particle.mass * other.mass * (a1-a2) / (particle.mass + other.mass);

                            particle.velocity -= p / particle.mass * r_hat;
                            other.velocity += p / other.mass * r_hat;

                        } else {

                            // Softening factor to prevent infinite forces at very small distances
                            const float epsilon = 0.01f;
                            const float softened_distance_squared = distance_squared + epsilon;

                            particle.acceleration += (other.mass / softened_distance_squared) * 
                                                        BIG_G * (other.position - particle.position);

                        }
                    }

                }
            }
        };

        threads_.emplace_back(thread_function);
    }

    for (auto& thread : threads_)
    {
        thread.join();
    }

    threads_.clear();
	
    const std::size_t chunk_size2 = particles_.size() / num_threads_;
    const std::size_t remainder2 = particles_.size() % num_threads_;

    // Loop through particles to set new velocity and the color based on that velocity
    for (int i = 0; i < num_threads_; i++) {
        const std::size_t start_index = i * chunk_size2;
        const std::size_t end_index = (i==num_threads_-1) ? start_index + chunk_size2 + remainder2 : start_index + chunk_size2;
        
        auto thread_function = [this, start_index, end_index]() {
            
            sf::Color c;

            for (std::size_t j = start_index; j < end_index; j++) {
				
                Particle& particle = particles_[j];

                if (is_right_button_pressed_)
                    attractParticleToMousePos(particle, current_mouse_pos_f_);

                particle.velocity += particle.acceleration * time_step_;
                particle.position += particle.velocity * time_step_;
                
                float velocity = std::sqrt(particle.velocity.x * particle.velocity.x +
                                           particle.velocity.y * particle.velocity.y);

                const float max_velocity = 3000.0f;

                if (velocity > max_velocity) velocity = max_velocity;
                
                const float p = velocity / max_velocity;

                c.r = static_cast<uint8_t>(15.0f + (240.0f * p));
                c.g = 0;
                c.b = static_cast<uint8_t>(240.0f * (1.0f-p));
                c.a = static_cast<uint8_t>(30.0f + (225.0f * p));

                particle.color = c;

                particle.acceleration.x = 0.0f;
                particle.acceleration.y = 0.0f;

            }
        };

        threads_.emplace_back(thread_function);
    }

    for (auto& thread : threads_)
    {
        thread.join();
    }

    threads_.clear();

    num_threads_ = n_threads;
}

void ParticleSimulation::addSierpinskiTriangleParticleChunk(const int x, const int y, const int size, const int depth)
{
    if (depth == 0) {
        particles_.emplace_back(Particle(sf::Vector2f(x,y), sf::Vector2f(0,0), particle_mass_));
    } else {
        const int half_size = size/2;

        addSierpinskiTriangleParticleChunk(x, y, half_size, depth - 1);
        addSierpinskiTriangleParticleChunk(x + half_size, y, half_size, depth - 1);
        addSierpinskiTriangleParticleChunk(x + half_size / 2, y + half_size, half_size, depth - 1);

    }
}

void ParticleSimulation::addCheckeredParticleChunk()
{
    for (int i = simulation_width_/3; i < ((2*simulation_width_)/3); ++i) {
        for (int j = simulation_height_/3; j < ((2*simulation_height_)/3); ++j) {
            if((i/7) % 6 == (j/5) % 6)
                particles_.emplace_back(Particle(sf::Vector2f(i,j), sf::Vector2f(0,0), particle_mass_));
        }
    }
}

void ParticleSimulation::addParticleDiagonal(int tiles, int num_particles)
{
    const int col = sqrt(num_particles/tiles);
    const int row = col;

    const float small_width = static_cast<float>(simulation_width_) / tiles;
    const float small_height = static_cast<float>(simulation_height_) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++ ) {
                const float x = (j * small_width / col) + small_width * i;
                const float y = (k * small_height / row) + small_height * i;
                particles_.emplace_back(Particle(sf::Vector2f(x,y), sf::Vector2f(0,0), particle_mass_));
            }
        }
    }
}

void ParticleSimulation::addParticleDiagonal2(int tiles, int num_particles)
{
    const int col = sqrt(num_particles / tiles);
    const int row = col;

    const float small_width = static_cast<float>(simulation_width_) / tiles;
    const float small_height = static_cast<float>(simulation_height_) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++) {
                const float x = static_cast<float>(simulation_width_) - ((j * small_width / col) + small_width * i);
                const float y = (k * small_height / row) + small_height * i;

                particles_.emplace_back(Particle(sf::Vector2f(x, y), sf::Vector2f(0, 0), particle_mass_));
            }
        }
    }
}
