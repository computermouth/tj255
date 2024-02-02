
#include "res.h"
#include "raylib.h"

// exported
Model level_model = { 0 };
Model tree_model = { 0 };

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
    // SetMaterialTexture(&level_model.materials[0], MATERIAL_MAP_DIFFUSE, cube_tex);

    song = LoadMusicStream("res/song.ogg");
    song.looping = 1;

    footstep_snd = LoadSound("res/footstep.wav");
    SetSoundVolume(footstep_snd, .7);

    night_tex = LoadTextureFromImage(GenImageColor(1, 1, DARKBLUE));
}
