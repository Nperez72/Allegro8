#include<allegro5/allegro.h>
#include <allegro5/allegro_image.h>


int main() {
	// Initialize Allegro and addons
	if (!al_init()) {
		return -1;
	}
	
	if (!al_init_image_addon()) {
		return -1;
	}
	
	ALLEGRO_DISPLAY* display = al_create_display(800, 600);
	if (!display) {
		return -1;
	}

	// Set up event listener for keyboard mapping
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	ALLEGRO_EVENT* ev = NULL;
	
	al_install_keyboard();
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	// Load a background image
	ALLEGRO_BITMAP* image = al_load_bitmap("example.png");
	if (!image) {
		al_destroy_display(display);
		return -1;
	}
	
	// Game loop
	while (true) {

	}


	// Clean up
	al_destroy_bitmap(image);
	al_destroy_display(display);
	return 0;
}