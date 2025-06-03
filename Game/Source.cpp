#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "ConcentrationGame.h"

const int card_size = 80;
const int padding = 10;

// Draw entire board of cards based on logic in ConcentrationGame except status info
// Will draw the shape when card is flipped and mark it when matched
void draw_card(int x, int y, const Card& card, const ConcentrationGame& game, ALLEGRO_FONT* font);

// Draw the status info (bottom right) showing matched/remaining pairs
void draw_status(const ConcentrationGame& game, int matched, int remaining, ALLEGRO_FONT* font);

int main() {
    // Initialize Allegro and its addons
    al_init();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ConcentrationGame game;

    // Calculate window size based on board size and card size
    const int window_width = game.board_size * (card_size + padding) + padding;
    const int window_height = game.board_size * (card_size + padding) + padding;

    ALLEGRO_DISPLAY* display = al_create_display(window_width, window_height);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    // Timer needed for doing flip delays and checking cards
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
	ALLEGRO_FONT* font = al_load_ttf_font("16020_FUTURAM.ttf", 22, 0);

    // Register emouse and timer to event queue
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    bool running = true;

    al_start_timer(timer);

    // Main event loop
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        // User attempts to flip a card
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x, my = ev.mouse.y;
            int col = (mx - padding) / (card_size + padding);
            int row = (my - padding) / (card_size + padding);
            if (row >= 0 && row < game.board_size && col >= 0 && col < game.board_size) {
                if (game.flip_card(row, col, al_get_time())) {
                    redraw = true;
                }
            }
        }
        // Update all cards on board
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            game.update(al_get_time());
            redraw = true;
        }
    }

    // Cleanup game objects
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}