
#include "res.h"
#include "raylib.h"

// exported
Model level_model = { 0 };
Model tree_model = { 0 };
Model pumpkin_attack[8];
Model pumpkin_walk[4];

// local
static Texture cube_tex = {0};
Texture flashlight_tex = {0};
Texture night_tex = { 0 };
Music song = { 0 };
Sound footstep_snd = { 0 };

void res_init(){

    cube_tex = LoadTexture("res/grass_tile.png");
    flashlight_tex = LoadTexture("res/overlay.png");

    level_model = LoadModel("res/level.glb");
    tree_model = LoadModel("res/trees.glb");
    
    pumpkin_attack[0] = LoadModel("res/pumpkin_attack1.glb");
    pumpkin_attack[1] = LoadModel("res/pumpkin_attack2.glb");
    pumpkin_attack[2] = LoadModel("res/pumpkin_attack3.glb");
    pumpkin_attack[3] = LoadModel("res/pumpkin_attack4.glb");
    pumpkin_attack[4] = LoadModel("res/pumpkin_attack5.glb");
    pumpkin_attack[5] = LoadModel("res/pumpkin_attack6.glb");
    pumpkin_attack[6] = LoadModel("res/pumpkin_attack7.glb");
    pumpkin_attack[7] = LoadModel("res/pumpkin_attack8.glb");
    // SetMaterialTexture(&level_model.materials[0], MATERIAL_MAP_DIFFUSE, cube_tex);
    pumpkin_walk[0] = LoadModel("res/pumpkin_walk1.glb");
    pumpkin_walk[1] = LoadModel("res/pumpkin_walk2.glb");
    pumpkin_walk[2] = LoadModel("res/pumpkin_walk3.glb");
    pumpkin_walk[3] = LoadModel("res/pumpkin_walk4.glb");
    // pumpkin_walk[4] = LoadModel("res/pumpkin_walk5.glb");

    song = LoadMusicStream("res/song.ogg");
    song.looping = 1;

    footstep_snd = LoadSound("res/footstep.wav");
    SetSoundVolume(footstep_snd, .7);

    night_tex = LoadTextureFromImage(GenImageColor(1, 1, DARKBLUE));
}
