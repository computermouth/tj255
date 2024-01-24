/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#include "res.h"
#include <math.h>

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame

Camera3D camera = { 0 };
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#define _DEBUG 1
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messsages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "crayjam");
    // ToggleFullscreen();

    res_init();

    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = (Vector3){ 1, 1000, 1 };
    camera.up = (Vector3){0,1,0};
    
    // Render texture to draw full screen, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    DisableCursor();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

bool grounded = false;
float grav = 0;
float yaw = 0;
float pitch = 0;

//--------------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    Vector2 md = GetMouseDelta();
    float mouse_spd = 0.0675;
    CameraYaw(&camera, -md.x * mouse_spd * GetFrameTime(), false);
    CameraPitch(&camera, -md.y * mouse_spd * GetFrameTime(), true, false, false);

    // player acceleration (input)
    float player_spd = 10.0;
    Vector3 a = Vector3Multiply(
        (Vector3){
            IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
            0,
            IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
        },
        (Vector3){
            player_spd * GetFrameTime() * (1.0 - (!grounded) * 0.3),
            player_spd * GetFrameTime() * (1.0 - (!grounded) * 0.3),
            player_spd * GetFrameTime() * (1.0 - (!grounded) * 0.3),
        }
    );

    CameraMoveForward(&camera, a.z, true);
    CameraMoveRight(&camera, a.x, true);

    fprintf(stderr, "vc: %d\n", level_model.meshes[0].vertexCount);
    fprintf(stderr, "tc: %d\n", level_model.meshes[0].triangleCount);
    for(int i = 0; i < level_model.meshes[0].triangleCount * 3; i++){
        fprintf(stderr, "ii[%d]: %d\n", i, level_model.meshes[0].indices[i]);
    }

    Ray downray = {.position = camera.position, .direction = {0,1,0} };
    RayCollision r = { 0 };
    for(int i = 0; i < level_model.meshes[0].vertexCount; i++){
        float * triangles = &level_model.meshes[0].vertices[i * 3 * 3];
        r = GetRayCollisionTriangle(
                downray,
                (Vector3){triangles[0], triangles[1], triangles[2]},
                (Vector3){triangles[3], triangles[4], triangles[5]},
                (Vector3){triangles[6], triangles[7], triangles[8]}
            );
        if(r.hit){
            break;
        }
    }

    if (r.hit && camera.position.y <= r.point.y + 2){
        camera.position.y = r.point.y + 2;
        grounded = true;
        grav = 0;
    }

    if (!grounded){
        grav -= 9.8 * GetFrameTime();
    }

    // todo, jump, climb
    CameraMoveUp(&camera, grav);

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further sahder postprocessing
    BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        
        // TODO: Draw your game screen here
        DrawRectangle(10, 10, screenWidth - 20, screenHeight - 20, SKYBLUE);
        
        BeginMode3D(camera);
            DrawModel(level_model, (Vector3){0}, .5, WHITE);
            DrawModelWires(level_model, (Vector3){0}, .5, BLACK);
        EndMode3D();
        
    EndTextureMode();
    
    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // TODO: Draw everything that requires to be drawn at this point, maybe UI?

    EndDrawing();
    //----------------------------------------------------------------------------------  
}
