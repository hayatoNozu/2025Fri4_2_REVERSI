#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>

const int BOARD_SIZE = 8;
const int CELL_SIZE = 80;
const int WINDOW_SIZE = BOARD_SIZE * CELL_SIZE;

enum CellState { EMPTY, BLACK, WHITE };

class Othello {
private:
    std::array<std::array<CellState, BOARD_SIZE>, BOARD_SIZE> board;
    CellState currentPlayer;

public:
    Othello() {
        // 全セルを空にする
        for (auto& row : board) {
            row.fill(EMPTY);
        }
        // 初期配置
        board[3][3] = WHITE;
        board[3][4] = BLACK;
        board[4][3] = BLACK;
        board[4][4] = WHITE;

        currentPlayer = BLACK;
    }

    CellState getCell(int x, int y) const {
        return board[y][x];
    }

    CellState getCurrentPlayer() const {
        return currentPlayer;
    }

    bool placeStone(int x, int y) {
        if (board[y][x] != EMPTY)
            return false;

        bool flippedAny = false;
        const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
        const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };

        CellState opponent = (currentPlayer == BLACK) ? WHITE : BLACK;

        for (int dir = 0; dir < 8; ++dir) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            std::vector<std::pair<int, int>> toFlip;

            while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == opponent) {
                toFlip.emplace_back(nx, ny);
                nx += dx[dir];
                ny += dy[dir];
            }

            if (!toFlip.empty() &&
                nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == currentPlayer) {
                for (auto [fx, fy] : toFlip) {
                    board[fy][fx] = currentPlayer;
                }
                flippedAny = true;
            }
        }

        if (flippedAny) {
            board[y][x] = currentPlayer;

            // 相手に合法手がなければパスして自分のターンを継続
            CellState opponent = (currentPlayer == BLACK) ? WHITE : BLACK;
            if (hasValidMove(opponent)) {
                currentPlayer = opponent;
            }
        }



        return flippedAny;
    }

    bool hasValidMove(CellState player) const {
        const int dx[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
        const int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };

        CellState opponent = (player == BLACK) ? WHITE : BLACK;

        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                if (board[y][x] != EMPTY) continue;

                for (int dir = 0; dir < 8; ++dir) {
                    int nx = x + dx[dir];
                    int ny = y + dy[dir];

                    bool hasOpponent = false;

                    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[ny][nx] == opponent) {
                        hasOpponent = true;
                        nx += dx[dir];
                        ny += dy[dir];
                    }

                    if (hasOpponent &&
                        nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[ny][nx] == player) {
                        return true; // 合法手あり
                    }
                }
            }
        }

        return false; // 置ける場所なし
    }

    bool isGameOver() const {
        return !hasValidMove(BLACK) && !hasValidMove(WHITE);
    }

    std::string getWinnerText() const {
        int blackCount = 0, whiteCount = 0;
        for (const auto& row : board) {
            for (CellState cell : row) {
                if (cell == BLACK) blackCount++;
                if (cell == WHITE) whiteCount++;
            }
        }

        if (blackCount > whiteCount) return "Black Wins!";
        else if (whiteCount > blackCount) return "White Wins!";
        else return "Draw!";
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Othello - SFML");


    Othello game;
    sf::Font font;
    font.loadFromFile("ARIALN.TTF"); // 適当なフォントを用意してください
    if (!font.loadFromFile("ARIALN.TTF")) {
        std::cerr << "フォントの読み込みに失敗しました" << std::endl;
    }

    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(48);
    resultText.setFillColor(sf::Color::Yellow);
    resultText.setPosition(50, WINDOW_SIZE / 2 - 24);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / CELL_SIZE;
                int y = event.mouseButton.y / CELL_SIZE;
                game.placeStone(x, y);
            }
        }

        if (game.isGameOver()) {
            resultText.setString(game.getWinnerText());
            window.draw(resultText);
        }

        window.clear(sf::Color::Green);

        // 盤面描画
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                cell.setPosition(x * CELL_SIZE + 1, y * CELL_SIZE + 1);
                cell.setFillColor(sf::Color(0, 100, 0));
                window.draw(cell);

                CellState state = game.getCell(x, y);
                if (state != EMPTY) {
                    sf::CircleShape stone(CELL_SIZE / 2 - 5);
                    stone.setPosition(x * CELL_SIZE + 5, y * CELL_SIZE + 5);
                    stone.setFillColor(state == BLACK ? sf::Color::Black : sf::Color::White);
                    window.draw(stone);
                }
            }
        }

        if (game.isGameOver()) {
            resultText.setString(game.getWinnerText());
            window.draw(resultText);
        }

        window.display();
    }

    return 0;
}