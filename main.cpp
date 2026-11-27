#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "rlgl.h"
#include <cmath>

struct WorldObject {
    Model model;
    Vector3 position;
    float scale;
    std::string path;
};

int main() {
    Color BABYBLUE = { 137, 207, 240, 255 };

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1920, 1080, "3D_GAME");

    ToggleFullscreen();

    Shader alphaShader = LoadShader(0, "alpha.fs");

    SetTargetFPS(60);
    DisableCursor();

    int health = 100;

    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 playerPos = { 0.0f, 2.0f, 10.0f };
    float velocityY = 0.0f;
    bool isGrounded = true;
    camera.position = playerPos;

    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

    std::vector<WorldObject> worldObjects;

    std::ifstream file("map.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string modelPath;
            float x, y, z, scale;

            if (ss >> modelPath >> x >> y >> z >> scale) {
                WorldObject obj;
                obj.model = LoadModel(modelPath.c_str());
                obj.position = (Vector3){x, y, z};
                obj.scale = scale;
                obj.path = modelPath;

                if (modelPath.find("tree") != std::string::npos) {
                    for (int i = 0; i < obj.model.materialCount; i++) {
                        obj.model.materials[i].shader = alphaShader;
                    }
                }

                worldObjects.push_back(obj);
            }
        }
    }

    while (!WindowShouldClose()) {
        float moveSpeed = 4.0f;
        float targetHeight = 2.0f;

        if(IsKeyDown(KEY_F)){
            ToggleFullscreen();
        };

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            moveSpeed = 1.5f;
            targetHeight = 1.1f;
        } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
            moveSpeed = 8.0f;
        }

        if (IsKeyPressed(KEY_SPACE) && isGrounded) {
            velocityY = 0.2f;
            isGrounded = false;
        }

        if (!isGrounded) {
            velocityY -= 0.01f;
            playerPos.y += velocityY;
        } else {
            playerPos.y += (targetHeight - playerPos.y) * 10.0f * GetFrameTime();
            if (playerPos.y < targetHeight + 0.01f && playerPos.y > targetHeight - 0.01f) {
                playerPos.y = targetHeight;
            }
        }

        if (playerPos.y <= targetHeight && !isGrounded && velocityY <= 0.0f) {
            playerPos.y = targetHeight;
            velocityY = 0.0f;
            isGrounded = true;
        } else if (isGrounded && playerPos.y < targetHeight) {
            playerPos.y = targetHeight;
        }

        Vector2 mouseDelta = GetMouseDelta();
        cameraYaw -= mouseDelta.x * 0.003f;
        cameraPitch -= mouseDelta.y * 0.003f;

        if (cameraPitch > 1.48f) cameraPitch = 1.48f;
        if (cameraPitch < -1.48f) cameraPitch = -1.48f;

        Vector3 forward = {
            sinf(cameraYaw) * cosf(cameraPitch),
            sinf(cameraPitch),
            cosf(cameraYaw) * cosf(cameraPitch)
        };
        forward = Vector3Normalize(forward);

        Vector3 flatForward = { forward.x, 0.0f, forward.z };
        flatForward = Vector3Normalize(flatForward);
        Vector3 right = Vector3Normalize(Vector3CrossProduct(flatForward, camera.up));

        Vector3 oldPlayerPos = playerPos;
        camera.position = playerPos;

        float dt = GetFrameTime();
        if (IsKeyDown(KEY_W)) camera.position = Vector3Add(camera.position, Vector3Scale(flatForward, moveSpeed * dt));
        if (IsKeyDown(KEY_S)) camera.position = Vector3Subtract(camera.position, Vector3Scale(flatForward, moveSpeed * dt));
        if (IsKeyDown(KEY_A)) camera.position = Vector3Subtract(camera.position, Vector3Scale(right, moveSpeed * dt));
        if (IsKeyDown(KEY_D)) camera.position = Vector3Add(camera.position, Vector3Scale(right, moveSpeed * dt));

        playerPos = camera.position;
        camera.target = Vector3Add(camera.position, forward);

        float playerRadius = 0.2f;
        for (const auto& obj : worldObjects) {
            if (obj.path.find("tree") != std::string::npos) {
                float treeRadius = 0.8f * obj.scale;
                
                float dx = playerPos.x - obj.position.x;
                float dz = playerPos.z - obj.position.z;
                float distanceSq = (dx * dx) + (dz * dz);
                float minDist = playerRadius + treeRadius;

                if (distanceSq < (minDist * minDist)) {
                    playerPos = oldPlayerPos;
                    camera.position = playerPos;
                    break;
                }
            }
        }

        if (playerPos.y < targetHeight && isGrounded) playerPos.y = targetHeight;
        camera.position = playerPos;
        camera.target = Vector3Add(camera.position, forward);

        BeginDrawing();
        ClearBackground(BABYBLUE);

        BeginMode3D(camera);

        rlDisableBackfaceCulling();

        for (const auto& obj : worldObjects) {
            DrawModel(obj.model, obj.position, obj.scale, WHITE);
        }

        rlEnableBackfaceCulling();

        EndMode3D();

        DrawText("3D Game", 10, 10, 20, RAYWHITE);
        DrawText("Press ESC to exit", 120 , 10, 20, RAYWHITE);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 40, 20, RAYWHITE);

        DrawText(TextFormat("Health: %d", health), 10, 740, 20, RAYWHITE);

        EndDrawing();
    }

    for (auto& obj : worldObjects) {
        UnloadModel(obj.model);
    }
    UnloadShader(alphaShader);

    CloseWindow();
    return 0;
}