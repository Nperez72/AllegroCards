#include "ConcentrationGame.h"
#include <random>

ConcentrationGame::ConcentrationGame() : matched_pairs(0) {
    // Ensure status_row and status_col are set based on board_size
    status_row = board_size - 1;
    status_col = board_size - 1;

    // Create a vector with two of each shape (for pairs)
    std::vector<ShapeType> shape_ids;
    for (int i = 0; i < pairs; ++i) {
        shape_ids.push_back(static_cast<ShapeType>(i));
        shape_ids.push_back(static_cast<ShapeType>(i));
    }

    // Shuffle the shape_ids to randomize card placement
    std::shuffle(shape_ids.begin(), shape_ids.end(), std::default_random_engine{ std::random_device{}() });

    // Resize the board and assign shuffled cards, skipping the status info cell
    board.resize(board_size, std::vector<Card>(board_size));
    int idx = 0;
    for (int y = 0; y < board_size; ++y) {
        for (int x = 0; x < board_size; ++x) {
            if (y == status_row && x == status_col) continue; // Skip status info cell
            board[y][x] = { shape_ids[idx++], HIDDEN };
        }
    }
    // Set the status info cell (bottom-right) to no shape
    board[status_row][status_col] = { SHAPE_NONE, HIDDEN };
}

// Attempts to flip a card at (row, col). Returns true if successful.
bool ConcentrationGame::flip_card(int row, int col, double now) {
    // Do not allow flipping the status info cell
    if (row == status_row && col == status_col) return false;
    Card& card = board[row][col];
    // Only allow flipping if the card is hidden and fewer than 2 cards are currently flipped
    if (card.state == HIDDEN && flipped.size() < 2) {
        card.state = REVEALED;
        flipped.push_back({ row, col, now });
        return true;
    }
    return false;
}

// Updates the game state: checks for matches or hides cards after the delay.
void ConcentrationGame::update(double now) {
    // Only process if two cards are flipped
    if (flipped.size() == 2) {
        Card& c1 = board[flipped[0].row][flipped[0].col];
        Card& c2 = board[flipped[1].row][flipped[1].col];
        if (c1.shape == c2.shape) {
            // If the two cards match, mark them as matched and clear flipped list
            c1.state = c2.state = MATCHED;
            matched_pairs++;
            flipped.clear();
        }
        else if (now - flipped[1].reveal_time >= flip_delay) {
            // If not matched and delay has passed, hide both cards again
            c1.state = c2.state = HIDDEN;
            flipped.clear();
        }
    }
}

const std::vector<std::vector<Card>>& ConcentrationGame::get_board() const {
    return board;
}

int ConcentrationGame::get_matched_pairs() const {
    return matched_pairs;
}

int ConcentrationGame::get_remaining_pairs() const {
    return pairs - matched_pairs;
}

// Draws a shape in the card cell at (x, y) using the global card_size
void ConcentrationGame::draw_shape(ShapeType shape, int x, int y, ALLEGRO_COLOR color, int card_size) {
    int cx = x + card_size / 2;
    int cy = y + card_size / 2;
    int half = card_size / 2;

    switch (shape) {
    case SHAPE_CIRCLE:
        al_draw_circle(cx, cy, half * 0.7, color, 4);
        break;
    case SHAPE_SQUARE:
        al_draw_rectangle(cx - half * 0.7, cy - half * 0.7, cx + half * 0.7, cy + half * 0.7, color, 4);
        break;
    case SHAPE_TRIANGLE:
        al_draw_triangle(
            cx, cy - half * 0.8,
            cx - half * 0.7, cy + half * 0.7,
            cx + half * 0.7, cy + half * 0.7,
            color, 4);
        break;
    case SHAPE_RECTANGLE:
        al_draw_rectangle(cx - half * 0.8, cy - half * 0.4, cx + half * 0.8, cy + half * 0.4, color, 4);
        break;
    case SHAPE_ELLIPSES:
        al_draw_ellipse(cx, cy, half * 0.7, half * 0.4, color, 4);
        break;
    case SHAPE_DIAMOND: {
        float top_x = cx, top_y = cy - half * 0.7f;
        float left_x = cx - half * 0.7f, left_y = cy;
        float bottom_x = cx, bottom_y = cy + half * 0.7f;
        float right_x = cx + half * 0.7f, right_y = cy;

        al_draw_line(top_x, top_y, left_x, left_y, color, 4);
        al_draw_line(left_x, left_y, bottom_x, bottom_y, color, 4);
        al_draw_line(bottom_x, bottom_y, right_x, right_y, color, 4);
        al_draw_line(right_x, right_y, top_x, top_y, color, 4);
        break;
    }
    case SHAPE_CIRCLE_FILLED:
        al_draw_filled_circle(cx, cy, half * 0.7, color);
        break;
    case SHAPE_SQUARE_FILLED:
        al_draw_filled_rectangle(cx - half * 0.7, cy - half * 0.7, cx + half * 0.7, cy + half * 0.7, color);
        break;
    case SHAPE_TRIANGLE_FILLED:
        al_draw_filled_triangle(
            cx, cy - half * 0.8,
            cx - half * 0.7, cy + half * 0.7,
            cx + half * 0.7, cy + half * 0.7,
            color);
        break;
    case SHAPE_RECTANGLE_FILLED:
        al_draw_filled_rectangle(cx - half * 0.8, cy - half * 0.4, cx + half * 0.8, cy + half * 0.4, color);
        break;
    case SHAPE_ELLIPSES_FILLED:
        al_draw_filled_ellipse(cx, cy, half * 0.7, half * 0.4, color);
        break;
    case SHAPE_DIAMOND_FILLED: {
        float points[8] = {
            (float)cx, (float)(cy - half * 0.8),
            (float)(cx - half * 0.7), (float)cy,
            (float)cx, (float)(cy + half * 0.8),
            (float)(cx + half * 0.7), (float)cy
        };
        al_draw_filled_polygon(points, 4, color);
        break;
    }
    case SHAPE_NONE:
    default:
        break;
    }
}