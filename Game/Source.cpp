#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "ConcentrationGame.h"

const int card_size = 80;
const int padding = 10;

// Draw a card based on what's inside the board in ConcentrationGame
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
	ALLEGRO_FONT* font = al_load_ttf_font("16020_FUTURAM.ttf", 14, 0);

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
            // If mouse click is within a cell, flip the card on that cell
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

        // Redraw the board after evnts are processed
        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(50, 100, 50));
            // Grabs the current board
            const auto& board = game.get_board();
            // Draw each card on the board
            for (int y = 0; y < game.board_size; ++y) {
                for (int x = 0; x < game.board_size; ++x) {
                    draw_card(x, y, board[y][x], game, font);
                }
            }
            // Update information on matched and remaining pairs
            draw_status(game, game.get_matched_pairs(), game.get_remaining_pairs(), font);
            al_flip_display();
            redraw = false;
        }
    }

    // Cleanup game objects
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}

void draw_card(int x, int y, const Card& card, const ConcentrationGame& game, ALLEGRO_FONT* font) {
    // Skip drawing the status cell (bottom-right)
    if (x == game.status_col && y == game.status_row)
        return;

    int px = padding + x * (card_size + padding);
    int py = padding + y * (card_size + padding);

    // Draw card background and border
    al_draw_filled_rectangle(px, py, px + card_size, py + card_size, al_map_rgb(200, 200, 200));
    al_draw_rectangle(px, py, px + card_size, py + card_size, al_map_rgb(0, 0, 0), 2);

    // Draw the shape if the card is revealed or matched
    if (card.state == REVEALED || card.state == MATCHED) {
        ALLEGRO_COLOR color = (card.state == MATCHED) ? al_map_rgb(180, 180, 180) : al_map_rgb(80, 80, 220);
        ConcentrationGame::draw_shape(card.shape, px, py, color, card_size);

        // Draw red cross if the card is matched
        if (card.state == MATCHED) {
            al_draw_line(px, py, px + card_size, py + card_size, al_map_rgb(255, 0, 0), 4);
            al_draw_line(px + card_size, py, px, py + card_size, al_map_rgb(255, 0, 0), 4);
        }
    }
}

// Draw the status cell (bottom right) showing matched/remaining pairs
void draw_status(const ConcentrationGame& game, int matched, int remaining, ALLEGRO_FONT* font) {
    int px = padding + game.status_col * (card_size + padding);
    int py = padding + game.status_row * (card_size + padding);

    // Draw status cell background and border
    al_draw_filled_rectangle(px, py, px + card_size, py + card_size, al_map_rgb(220, 220, 255));
    al_draw_rectangle(px, py, px + card_size, py + card_size, al_map_rgb(0, 0, 0), 2);

    // Draw matched and remaining pairs text
    al_draw_textf(font, al_map_rgb(0, 0, 0), px + card_size / 2, py + 10, ALLEGRO_ALIGN_CENTER,
        "Matched: %d", matched);
    al_draw_textf(font, al_map_rgb(0, 0, 0), px + card_size / 2, py + 40, ALLEGRO_ALIGN_CENTER,
        "Remain: %d", remaining);
}