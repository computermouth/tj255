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
#include <stddef.h>

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
    InitAudioDevice();

    res_init();
    PlayMusicStream(song);

    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = (Vector3){ 100, 500, 100 };
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

float yaw_from_camera(Camera3D camera) {
    // Calculate direction vector from camera position to target
    Vector3 direction = Vector3Subtract(camera.target, camera.position);
    // Calculate yaw angle using atan2
    float yaw = atan2f(direction.z, direction.x);
    // Convert yaw to degrees
    yaw = yaw * (180.0f / PI);
    // Adjust yaw to be in range [0, 360)
    if (yaw < 0)
        yaw += 360.0f;
    return yaw;
}

bool grounded = false;
float grav = 0;
float yaw = 0;
float pitch = 0;
float footstep_time = 0;
Vector3 pumpkin_pos = { 10, 10, 10 };
bool pumpkin_spawned = true;

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
    UpdateMusicStream(song);

    footstep_time += GetFrameTime();

    // just in case
    if(camera.position.y < -40)
        camera.position = (Vector3){0, 10, 0};

    Vector2 md = GetMouseDelta();
    float mouse_spd = 0.0675;
    CameraYaw(&camera, -md.x * mouse_spd / 50, false);
    CameraPitch(&camera, -md.y * mouse_spd / 50, true, false, false);

    // player acceleration (input)
    float player_spd = 40.0;
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

    if(IsKeyPressed(KEY_SPACE)){
        grav += 2;
        CameraMoveUp(&camera, grav);
    }

    CameraMoveForward(&camera, a.z, true);
    CameraMoveRight(&camera, a.x, true);

    // if (pumpkin_spawned){
    //     Vector2 toward = Vector2Subtract((Vector2){camera.position.x, camera.position.z},(Vector2){pumpkin_pos.x, pumpkin_pos.z});
    //     fprintf(stderr, "tw.x: %f tw.y: %f\n", toward.x, toward.y);
    //     pumpkin_pos.x += toward.x;
    //     pumpkin_pos.z += toward.y;

    //     // put pumpkin on ground
    //     Ray downray = {.position = pumpkin_pos, .direction = {0,-1,0}};
    //     RayCollision r = { 0 };
    //     for(int i = 0; i < level_model.meshes[0].triangleCount; i++){
    //         unsigned short * ind = &level_model.meshes[0].indices[i * 3];
    //         float * t1 = &level_model.meshes[0].vertices[ind[0] * 3]; 
    //         float * t2 = &level_model.meshes[0].vertices[ind[1] * 3]; 
    //         float * t3 = &level_model.meshes[0].vertices[ind[2] * 3]; 
    //         r = GetRayCollisionTriangle(
    //                 downray,
    //                 (Vector3){t1[0], t1[1], t1[2]},
    //                 (Vector3){t2[0], t2[1], t2[2]},
    //                 (Vector3){t3[0], t3[1], t3[2]}
    //             );
    //         if(r.hit){
    //             // pumpkin_pos.y = r.point.y + 1;
    //             break;
    //         }
    //     }

    // }

    // fprintf(stderr, "vc: %d\n", level_model.meshes[0].vertexCount);
    // fprintf(stderr, "tc: %d\n", level_model.meshes[0].triangleCount);
    // for(int i = 0; i < level_model.meshes[0].triangleCount * 3; i++){
    //     fprintf(stderr, "ii[%d]: %d\n", i, level_model.meshes[0].indices[i]);
    // }


    Ray downray = {.position = camera.position, .direction = {0,-1,0}};
    RayCollision r = { 0 };
    Vector3 hit_tri[3] = { 0 };
    for(int i = 0; i < level_model.meshes[0].triangleCount; i++){
        unsigned short * ind = &level_model.meshes[0].indices[i * 3];
        float * t1 = &level_model.meshes[0].vertices[ind[0] * 3]; 
        float * t2 = &level_model.meshes[0].vertices[ind[1] * 3]; 
        float * t3 = &level_model.meshes[0].vertices[ind[2] * 3]; 
        r = GetRayCollisionTriangle(
                downray,
                (Vector3){t1[0], t1[1], t1[2]},
                (Vector3){t2[0], t2[1], t2[2]},
                (Vector3){t3[0], t3[1], t3[2]}
            );
        if(r.hit){
            hit_tri[0] = (Vector3){t1[0], t1[1], t1[2]};
            hit_tri[1] = (Vector3){t2[0], t2[1], t2[2]};
            hit_tri[2] = (Vector3){t3[0], t3[1], t3[2]};
            break;
        }
    }

    CameraMoveUp(&camera, grav);

    if (r.hit && camera.position.y - r.point.y <= 2){
        float diff = (r.point.y + 2) - camera.position.y;
        grav = 0;
        CameraMoveUp(&camera, diff);
        if(grav == 0 && (a.z != 0 || a.x != 0) && footstep_time > 0.5){
            PlaySound(footstep_snd);
            footstep_time = 0;
        }
    } else if (r.hit) {
        grav -= 9.8/2 * GetFrameTime();
    } else if (!r.hit){
        CameraMoveForward(&camera, -a.z, true);
        CameraMoveRight(&camera, -a.x, true);
    }

static float f = 0;
f += GetFrameTime() * 8;
f = fmodf(f, 4.0);

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further sahder postprocessing
    BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        
        // TODO: Draw your game screen here
        DrawRectangle(10, 10, screenWidth - 20, screenHeight - 20, BLACK);
        
        BeginMode3D(camera);
            // DrawModel(level_model, (Vector3){0}, 1, WHITE);
            DrawMesh(level_model.meshes[0], level_model.materials[1], MatrixIdentity());
            DrawModelWires(level_model, (Vector3){0}, 1, (Color){64,64,64,127});

            for(int i = 0; i < tree_model.meshCount; i++){
                DrawMesh(tree_model.meshes[i], tree_model.materials[(i % 2) + 1], MatrixIdentity());
            }

            if (pumpkin_spawned){
                // DrawMesh(pumpkin_attack[(size_t)f].meshes[0], pumpkin_attack[0].materials[1], MatrixIdentity());
                DrawMesh(pumpkin_walk[(size_t)f].meshes[0], pumpkin_walk[0].materials[1], MatrixAdd(MatrixIdentity(), MatrixTranslate(pumpkin_pos.x, pumpkin_pos.y, pumpkin_pos.z)));
            }

            Color tr = BLACK;
            tr.a = 127;

            if(r.hit){
                DrawTriangle3D(hit_tri[0], hit_tri[1], hit_tri[2], tr);
            }

        EndMode3D();

        DrawFPS(10, 10);
        
    EndTextureMode();

    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawTexturePro(night_tex, (Rectangle){0,0,1,1}, (Rectangle){0,0,screenWidth,screenHeight}, (Vector2){0}, 0, (Color){255,255,255,127});
        DrawTexturePro(
            flashlight_tex,
            (Rectangle){0, 0, flashlight_tex.width, flashlight_tex.height},
            (Rectangle){flashlight_tex.width / 4., flashlight_tex.height / 8., flashlight_tex.width, flashlight_tex.height},
            (Vector2){flashlight_tex.width/2., flashlight_tex.height/2.},
            yaw_from_camera(camera),
            WHITE
        );

        // TODO: Draw everything that requires to be drawn at this point, maybe UI?

    EndDrawing();
    //----------------------------------------------------------------------------------  
}
