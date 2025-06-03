#include <vector>
#include <allegro5/allegro_primitives.h>

enum CardState {
    HIDDEN,    // Card is face down 
    REVEALED,  // Card is face up
    MATCHED    // Card has been matched and is no longer in play.
};

enum ShapeType {
    SHAPE_CIRCLE,
    SHAPE_SQUARE,
    SHAPE_TRIANGLE,
    SHAPE_RECTANGLE,
    SHAPE_ELLIPSES,
    SHAPE_DIAMOND,
    SHAPE_CIRCLE_FILLED,
    SHAPE_SQUARE_FILLED,
    SHAPE_TRIANGLE_FILLED,
    SHAPE_RECTANGLE_FILLED,
    SHAPE_ELLIPSES_FILLED,
    SHAPE_DIAMOND_FILLED,
    SHAPE_NONE
};

struct Card {
    ShapeType shape;  // Shape type of the card
    CardState state;  // Current state of the card
};

struct FlipInfo {
    int row, col;         // Board coordinates of the flipped card.
    double reveal_time;   // Time when the card was revealed (used for flip delay logic).
};

class ConcentrationGame {
public:
    // Board dimensions for rows and columns
    int board_size = 5;
    int pairs = 12;
    int status_row = board_size - 1;
    int status_col = board_size - 1;
    // Delay shown when cards are flipped
    double flip_delay = 5.0;

    ConcentrationGame();

    // Attempts to flip a card at the given board position.
    // Returns true if the card was successfully flipped.
    bool flip_card(int row, int col, double now);

    // Updates the cards on the board
    void update(double now);

    // Returns a const reference to the current board state.
    const std::vector<std::vector<Card>>& get_board() const;

    // Returns the number of matched pairs found so far.
    int get_matched_pairs() const;

    // Returns the number of pairs remaining to be matched.
    int get_remaining_pairs() const;

    // Draws a shape in the card cell at (x, y) using the global card_size
    static void draw_shape(ShapeType shape, int x, int y, ALLEGRO_COLOR color, int card_size);

private:
    std::vector<std::vector<Card>> board; // 2D board of cards.
    std::vector<FlipInfo> flipped;        // Currently flipped cards (max 2 at a time).
    int matched_pairs;                    // Number of pairs matched by the player.
};