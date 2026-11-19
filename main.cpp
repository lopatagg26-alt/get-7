
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace sf;
using namespace std;

const int W = 900;
const int H = 600;
const int TILE = 30;

vector<string> world = {
    "##############################",
    "#............................#",
    "#..........|.................#",
    "#..........|.................#",
    "#............................#",
    "#.....########...............#",
    "#............................#",
    "#.................|..........#",
    "#.................|..........#",
    "#............................#",
    "#............................#",
    "##############################"
};

int px = 3, py = 3;
int copX = 24, copY = 8;
int wanted = 0;
int speed = 0;
bool running = true;

bool canMove(int x, int y) {
    if (y < 0 || y >= (int)world.size()) return false;
    if (x < 0 || x >= (int)world[y].size()) return false;

    return world[y][x] != '#' &&
           world[y][x] != '|';
}

void movePlayer(int dx, int dy) {
    int nx = px + dx;
    int ny = py + dy;

    if (canMove(nx, ny)) {
        px = nx;
        py = ny;
        speed = 40;

        if (rand() % 5 == 0)
            wanted = min(5, wanted + 1);
    }
}

void movePolice() {
    if (wanted <= 0) return;

    if (copX < px && canMove(copX + 1, copY))
        copX++;
    else if (copX > px && canMove(copX - 1, copY))
        copX--;

    if (copY < py && canMove(copX, copY + 1))
        copY++;
    else if (copY > py && canMove(copX, copY - 1))
        copY--;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    RenderWindow window(
        VideoMode({W, H}),
        "CITY DRIVER - POLICE CHASE"
    );

    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("arial.ttf")) {
        cerr << "Put arial.ttf next to the game.";
        return 1;
    }

    Music music;
    if (music.openFromFile("music/city_music.ogg")) {
        music.setLooping(true);
        music.setVolume(25);
        music.play();
    }

    RectangleShape road({W, H});
    road.setFillColor(Color(45, 45, 45));

    RectangleShape wall({TILE - 2, TILE - 2});
    wall.setFillColor(Color(115, 115, 125));

    RectangleShape player({22, 22});
    player.setFillColor(Color::Red);

    RectangleShape police({22, 22});
    police.setFillColor(Color::Blue);

    Text hud(font);
    hud.setCharacterSize(17);
    hud.setFillColor(Color::White);
    hud.setPosition({15, 15});

    RectangleShape panel({150, 38});
    panel.setPosition({730, 15});
    panel.setFillColor(Color(20, 20, 20));

    Text musicText(font);
    musicText.setString("M - MUSIC ON/OFF");
    musicText.setCharacterSize(12);
    musicText.setPosition({738, 25});

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* key =
                event->getIf<Event::KeyPressed>()) {

                if (key->code == Keyboard::Key::Escape)
                    window.close();

                if (key->code == Keyboard::Key::W)
                    movePlayer(0, -1);

                if (key->code == Keyboard::Key::S)
                    movePlayer(0, 1);

                if (key->code == Keyboard::Key::A)
                    movePlayer(-1, 0);

                if (key->code == Keyboard::Key::D)
                    movePlayer(1, 0);

                if (key->code == Keyboard::Key::M) {
                    if (music.getStatus() == Music::Status::Playing)
                        music.pause();
                    else
                        music.play();
                }

                if (key->code == Keyboard::Key::F)
                    wanted = min(5, wanted + 1);
            }
        }

        movePolice();

        if (px == copX && py == copY) {
            wanted = 0;
            px = 3;
            py = 3;
            copX = 24;
            copY = 8;
        }

        window.clear(Color(25, 25, 25));

        // Отрисовка карты
        for (int y = 0; y < (int)world.size(); y++) {
            for (int x = 0; x < (int)world[y].size(); x++) {
                if (world[y][x] == '#' ||
                    world[y][x] == '|') {

                    wall.setPosition({
                        x * TILE + 100.f,
                        y * TILE + 80.f
                    });

                    window.draw(wall);
                }
            }
        }

        player.setPosition({
            px * TILE + 100.f,
            py * TILE + 80.f
        });

        police.setPosition({
            copX * TILE + 100.f,
            copY * TILE + 80.f
        });

        window.draw(player);
        if (wanted > 0)
            window.draw(police);

        hud.setString(
            "CITY DRIVER\n"
            "SPEED: " + to_string(speed) +
            "\nWANTED: " + string(wanted, '*') +
            "\nWASD - DRIVE | F - WANTED"
        );

        window.draw(hud);
        window.draw(panel);
        window.draw(musicText);

        window.display();
    }

    return 0;
}
