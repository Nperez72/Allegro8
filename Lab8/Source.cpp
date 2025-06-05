#include<allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <iostream>

// Needed for direction logic
enum Facing {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Player {
    float x, y;
    float speed;
    int screen_w, screen_h;
    ALLEGRO_BITMAP* sprite;
    Facing facing = UP;
    bool can_move = true;
    double flip_timer = 0.0;

    // Function to update direction
    void move(ALLEGRO_EVENT& ev);
    // Function for auto move
    void move_continuous();
    // Handle player bounds and flip direction
    void handle_bounds();
    // Update timer for flip delay
    void update_timer(double now);
};

const double FLIP_DELAY = 0.3;

int main() {

    // Initialize Allegro and its addons
    if (!al_init()) {
        std::cout << "Failed to initialize Allegro!" << std::endl;
        return -1;
    }
    if (!al_init_image_addon()) {
        std::cout << "Failed to initialize Allegro image addon!" << std::endl;
        return -1;
    }
    if (!al_install_keyboard()) {
        std::cout << "Failed to install keyboard!" << std::endl;
        return -1;
    }

    // Set up display 
    const int SCREEN_W = 900, SCREEN_H = 800;
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        std::cout << "Failed to create display!" << std::endl;
        return -1;
    }

    // Set up event queue
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // Load background and player image
    ALLEGRO_BITMAP* bg = al_load_bitmap("space.jpg");
    ALLEGRO_BITMAP* player_bmp = al_load_bitmap("player.png");
    if (!bg || !player_bmp) {
        std::cout << "Failed to load images!" << std::endl;
        al_destroy_display(display);
        return -1;
    }

    // Set up player object in middle of screen
    Player player = {SCREEN_W / 2, SCREEN_H / 2, 10.0f, SCREEN_W, SCREEN_H, player_bmp};
    
    bool running = true, redraw = true;
    ALLEGRO_EVENT ev;
    al_start_timer(timer);

    ALLEGRO_KEYBOARD_STATE key_state;

    // Game loop for movement updates
    while (running) {
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Necessary for time between flipping and moving
            double now = al_get_time();
            player.update_timer(now);
            // Player moves until they hit bounds, prompting flip
            player.move_continuous();
            player.handle_bounds();
            redraw = true;
        }
        // Changes direction of player
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            player.move(ev);
        }
        // Redraw screen after movement
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_draw_bitmap(bg, 0, 0, 0);

            // Draw player at its position with rotation based on facing direction
            float cx = al_get_bitmap_width(player.sprite) / 2.0f;
            float cy = al_get_bitmap_height(player.sprite) / 2.0f;
            float draw_x = player.x + cx;
            float draw_y = player.y + cy;

            // UP is deafult so change angle relatively
            float angle = 0.0f;
            if (player.facing == UP) {
                angle = 0.0f;
            } else if (player.facing == RIGHT) {
                angle = ALLEGRO_PI / 2;
            } else if (player.facing == DOWN) {
                angle = ALLEGRO_PI;
            } else if (player.facing == LEFT) {
                angle = -ALLEGRO_PI / 2;
            }
            al_draw_rotated_bitmap(player.sprite, cx, cy, draw_x, draw_y, angle, 0);

            al_flip_display();
        }
    }

    // Cleanup game objects
    al_destroy_bitmap(bg);
    al_destroy_bitmap(player_bmp);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    return 0;
}

// Function to move the player direction based on input
void Player::move(ALLEGRO_EVENT& ev)
{
    if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
        facing = LEFT;
    }
    else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
        facing = RIGHT;
    }
    else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
        facing = UP;
    }
    else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
        facing = DOWN;
    }
}

// Auto move player in set direcion
void Player::move_continuous() {
    if (can_move) {
        switch (facing) {
            case LEFT: 
                x -= speed; 
                break;
            case RIGHT: 
                x += speed; 
                break;
            case UP: 
                y -= speed; 
                break;
            case DOWN: 
                y += speed; 
                break;
        }
    }
}



// Handle player bounds and flip direction if needed
void Player::handle_bounds() {
    int pw = al_get_bitmap_width(sprite);
    int ph = al_get_bitmap_height(sprite);
    bool flipped = false;

    if (x < 0) {
        x = 0;
        if (facing == LEFT) {
            facing = RIGHT;
            flipped = true;
        }
    }
    if (x > screen_w - pw) {
        x = screen_w - pw;
        if (facing == RIGHT) {
            facing = LEFT;
            flipped = true;
        }
    }
    if (y < 0) {
        y = 0;
        if (facing == UP) {
            facing = DOWN;
            flipped = true;
        }
    }
    if (y > screen_h - ph) {
        y = screen_h - ph;
        if (facing == DOWN) {
            facing = UP;
            flipped = true;
        }
    }

	// Add a time delay when player is flipped
    if (flipped) {
        can_move = false;
        flip_timer = al_get_time() + FLIP_DELAY;
    }
}

// Update timer for flip delay
void Player::update_timer(double now) {
    if (!can_move && now >= flip_timer) {
        can_move = true;
    }
}
