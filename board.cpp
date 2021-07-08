#include <SDL2/SDL.h>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <deque>

#define MAX_ANTS 10
#define START_ANTS 2

class Board {
    public:
        Board(int width) {
            SDL_Init(SDL_INIT_VIDEO);
            srand(time(0));
            window = SDL_CreateWindow("Jhess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, width, SDL_WINDOW_SHOWN);
            screenSurface = SDL_GetWindowSurface(window);
            foodImage = SDL_LoadBMP("Apple.bmp");
            bigAntImage = SDL_LoadBMP("Sand.bmp");
            wallImage = SDL_LoadBMP("Wall.bmp");
            antImage = SDL_LoadBMP("Fire-Ant-Small.bmp");

            // int *mx = (int *) malloc(sizeof(int));
            // int *my = (int *) malloc(sizeof(int));

            for (int i = 0; i < START_ANTS; i++) {
                antLocation[i][0] = rand() % 8;
                antLocation[i][1] = rand() % 8;
            }

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if (e.type == SDL_MOUSEBUTTONUP) {
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            if (!inAnt(e.button.x / 80, e.button.y / 80)) {
                                wall[0].push_back(e.button.x / 80);
                                wall[1].push_back(e.button.y / 80);
                            }
                        } else {
                            if (!inAnt(e.button.x / 80, e.button.y / 80)) {
                                food[0].push_back(e.button.x / 80);
                                food[1].push_back(e.button.y / 80);
                            }
                        } 
                    } 
                }

                // SDL_PumpEvents();
                // SDL_GetMouseState(mx, my);

                if (frame % 2 == 0) {
                    if (ants != 0)
                        moveAnt(rand() % ants);
                }

                // drawBoard();
                SDL_BlitSurface(bigAntImage, NULL, screenSurface, NULL);
                drawWall();
                drawFood();

                for (int i = 0; i < ants; i++) {
                    drawImg(antImage, antLocation[i][0], antLocation[i][1]);
                    for (int j = 0; j < food[0].size(); j++) {
                        if (food[0][j] == antLocation[i][0] && food[1][j] == antLocation[i][1] && ants < MAX_ANTS) {
                            antLocation[ants][0] = food[0][j];
                            antLocation[ants][1] = food[1][j];
                            ants++;

                            auto it = food[0].begin();
                            food[0].erase(it + j);
                            it = food[1].begin();
                            food[1].erase(it + j);
                        }
                    }
                }

                SDL_UpdateWindowSurface(window);
                usleep(15000);
                frame++;
            }

            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    private:
        SDL_Window* window;
        SDL_Surface* screenSurface;
        SDL_Surface* antImage;
        SDL_Surface* bigAntImage;
        SDL_Surface* foodImage;
        SDL_Surface* wallImage;
        bool quit = false;
        int lastClick[2];
        int antLocation[MAX_ANTS][2];
        int ants = START_ANTS;
        int frame = 0;
        std::deque<int> wall[2];
        std::deque<int> food[2];
        SDL_Event e;
        
        void moveAnt(int antIdx) {
            int oldAnt[2];
            oldAnt[0] = antLocation[antIdx][0];
            oldAnt[1] = antLocation[antIdx][1];
            if (rand() % 2 == 0) {
                if (antLocation[antIdx][0] == 0) {
                    antLocation[antIdx][0] += 1;
                } else if (antLocation[antIdx][0] == 7) {
                    antLocation[antIdx][0] -= 1;
                } else {
                    antLocation[antIdx][0] += (rand() % 3) - 1;
                }
            } else {
                if (antLocation[antIdx][1] == 0) {
                    antLocation[antIdx][1] += 1;
                } else if (antLocation[antIdx][1] == 7) {
                    antLocation[antIdx][1] -= 1;
                } else {
                    antLocation[antIdx][1] += (rand() % 3) - 1;
                }
            }

            if (inWall(antLocation[antIdx][0], antLocation[antIdx][1])) {
                antLocation[antIdx][0] = oldAnt[0];
                antLocation[antIdx][1] = oldAnt[1];
            }
        }

        void drawBoard() {
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0x99));

            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) {
                        drawRect(x * 80, y * 80, 80, 80, 0x51, 0x10, 0x10);
                    }
                }
            }
        }

        bool inWall(int x, int y) {
            for (int i = 0; i < wall[0].size(); i++) {
                if (x == wall[0][i] && y == wall[1][i]) {
                    return true;
                }
            }
            return false;
        }

        bool inAnt(int x, int y) {
            for (int i = 0; i < ants; i++) {
                if (x == antLocation[i][0] && y == antLocation[i][1]) {
                    return true;
                }
            }
            return false;
        }

        void drawWall() {
            if (wall[0].size() > 16) {
                wall[0].pop_front();
                wall[1].pop_front();
            }

            for (int i = 0; i < wall[0].size(); i++) {
                drawImg(wallImage, wall[0][i], wall[1][i]);
            }
        }

        void drawFood() {
            if (food[0].size() > 3) {
                food[0].pop_front();
                food[1].pop_front();
            }

            for (int i = 0; i < food[0].size(); i++) {
                drawImg(foodImage, food[0][i], food[1][i]);
            }
        }

        void drawRect(int x, int y, int width, int height, int r, int g, int b) {
            SDL_Rect rect;
            
            rect.x = x;
            rect.y = y;
            rect.w = width;
            rect.h = height;

            SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, r, g, b));
        }

        void drawImg(SDL_Surface* img, int x, int y) {
            SDL_Rect rect;
            
            rect.x = x * 80;
            rect.y = y * 80;
            rect.w = 80;
            rect.h = 80;

            SDL_BlitSurface(img, NULL, screenSurface, &rect);
        }
};