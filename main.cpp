#include <raylib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

struct WorldObject{
    Model model;
    Vector3 position;
    float scale;
};

int main(){

    int fps = GetFPS();

    Color BABYBLUE = { 137, 207, 240, 255 };

    InitWindow(1280, 720, "3D_GAME");

    SetTargetFPS(9999);
    WindowShouldClose();

    DisableCursor();

    Camera3D camera = { 0 };
    camera.position = { Vector3{0, 0, 10} };
    camera.target = { Vector3{0, 0, 0} };
    camera.up = { Vector3{0, 1, 0} };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::vector<WorldObject> worldObjects;

    std::ifstream file("map.txt");
    if(file.is_open()){
        std::string line;
        while(std::getline(file, line)){
            if(line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string modelPath;
            float x, y, z, scale;

            if(ss >> modelPath >> x >> y >> z >> scale){
                WorldObject obj;
                obj.model = LoadModel(modelPath.c_str());
                obj.position = (Vector3){x, y, z};
                obj.scale = scale;

                worldObjects.push_back(obj);
            }
        }
    }

    while(!WindowShouldClose()){
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(BABYBLUE);
        
        BeginMode3D(camera);
        DrawGrid(30, 1.0f);

        for(const auto& obj : worldObjects){
            DrawModel(obj.model, obj.position, obj.scale, WHITE);
        }

        EndMode3D();

        DrawText("3D Game", 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 40, 20, RAYWHITE);

        EndDrawing();
    }

    for(auto& obj : worldObjects){
        UnloadModel(obj.model);
    }

    CloseWindow();
    return 0;
}