#include <raylib.h>

int main(){

    InitWindow(1280, 720, "3D_GAME");

    SetTargetFPS(60);
    WindowShouldClose();

    while (!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(RAYWHITE);


        EndDrawing();
    }
}