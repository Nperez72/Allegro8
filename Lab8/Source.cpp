#include<allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <iostream>

enum Facing { // Needed for direction logic
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
    Facing facing;

	// Function to move the player based on key input
    void move(int keycode);
};


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
    Player player = {SCREEN_W / 2, SCREEN_H / 2, 10.0f, SCREEN_W, SCREEN_H, player_bmp, UP};
    
    bool running = true, redraw = true;
    ALLEGRO_EVENT ev;
    al_start_timer(timer);

    ALLEGRO_KEYBOARD_STATE key_state;

    // Game loop for movement updates
    while (running) {
        al_wait_for_event(event_queue, &ev);
		// For smoother movement, we use timer event
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // We check for down key presses
            al_get_keyboard_state(&key_state);
			
            if (al_key_down(&key_state, ALLEGRO_KEY_LEFT))  
                player.move(ALLEGRO_KEY_LEFT);
            if (al_key_down(&key_state, ALLEGRO_KEY_RIGHT)) 
                player.move(ALLEGRO_KEY_RIGHT);
            if (al_key_down(&key_state, ALLEGRO_KEY_UP))    
                player.move(ALLEGRO_KEY_UP);
            if (al_key_down(&key_state, ALLEGRO_KEY_DOWN))  
                player.move(ALLEGRO_KEY_DOWN);

            redraw = true;
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

            float angle = 0.0f;
            switch (player.facing) {
                case UP:    
                    angle = 0.0f; 
                    break;
                case RIGHT: 
                    angle = ALLEGRO_PI / 2; 
                    break;
                case DOWN:  
                    angle = ALLEGRO_PI; 
                    break;
                case LEFT:  
                    angle = -ALLEGRO_PI / 2; 
                    break;
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


void Player::move(int keycode){
    switch (keycode) {
    case ALLEGRO_KEY_LEFT:
        x -= speed;
		facing = LEFT;
        break;
    case ALLEGRO_KEY_RIGHT:
        x += speed;
		facing = RIGHT;
        break;
    case ALLEGRO_KEY_UP:
        y -= speed;
		facing = UP;
        break;
    case ALLEGRO_KEY_DOWN:
        y += speed;
		facing = DOWN;
        break;
    }
    // Ensure player stays within screen bounds
    int pw = al_get_bitmap_width(sprite);
    int ph = al_get_bitmap_height(sprite);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > screen_w - pw) x = screen_w - pw;
    if (y > screen_h - ph) y = screen_h - ph;
}
