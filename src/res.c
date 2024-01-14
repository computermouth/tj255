
#include "res.h"

// exported
Model level_model = { 0 };

// local
static Texture cube_tex = {0};

void res_init(){

    cube_tex = LoadTexture("res/grass_tile.png");

    level_model = LoadModel("res/level.glb");
    SetMaterialTexture(&level_model.materials[0], MATERIAL_MAP_DIFFUSE, cube_tex);
}