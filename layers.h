#ifndef LAYER_H_
#define LAYER_H_

#include "javarnd.h"
#include <assert.h>
#include <node_api.h>

#define __STDC_FORMAT_MACROS 1

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define STRUCT(S) typedef struct S S; struct S

#if __GNUC__
#define PREFETCH(PTR,RW,LOC)    __builtin_prefetch(PTR,RW,LOC)
#define L(COND)                 (__builtin_expect(!!(COND),1))  // [[likely]]
#define U(COND)                 (__builtin_expect((COND),0))    // [[unlikely]]
#else
#define PREFETCH(PTR,RW,LOC)
#define L(COND)                 (COND)
#define U(COND)                 (COND)
#endif

/* Minecraft versions */
enum MCversion
{
    MC_1_0, MC_1_1, MC_1_2, MC_1_3, MC_1_4, MC_1_5, MC_1_6, //<1.7 Experimental!
    MC_1_7, MC_1_8, MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_14,
    MC_1_15, MC_1_16,
};

enum BiomeID
{
    none = -1,
    // 0
    ocean = 0,
    plains,
    desert,
    mountains,                  extremeHills = mountains,
    forest,
    taiga,
    swamp,                      swampland = swamp,
    river,
    nether_wastes,              hell = nether_wastes,
    the_end,                    sky = the_end,
    // 10
    frozen_ocean,               frozenOcean = frozen_ocean,
    frozen_river,               frozenRiver = frozen_river,
    snowy_tundra,               icePlains = snowy_tundra,
    snowy_mountains,            iceMountains = snowy_mountains,
    mushroom_fields,            mushroomIsland = mushroom_fields,
    mushroom_field_shore,       mushroomIslandShore = mushroom_field_shore,
    beach,
    desert_hills,               desertHills = desert_hills,
    wooded_hills,               forestHills = wooded_hills,
    taiga_hills,                taigaHills = taiga_hills,
    // 20
    mountain_edge,              extremeHillsEdge = mountain_edge,
    jungle,
    jungle_hills,               jungleHills = jungle_hills,
    jungle_edge,                jungleEdge = jungle_edge,
    deep_ocean,                 deepOcean = deep_ocean,
    stone_shore,                stoneBeach = stone_shore,
    snowy_beach,                coldBeach = snowy_beach,
    birch_forest,               birchForest = birch_forest,
    birch_forest_hills,         birchForestHills = birch_forest_hills,
    dark_forest,                roofedForest = dark_forest,
    // 30
    snowy_taiga,                coldTaiga = snowy_taiga,
    snowy_taiga_hills,          coldTaigaHills = snowy_taiga_hills,
    giant_tree_taiga,           megaTaiga = giant_tree_taiga,
    giant_tree_taiga_hills,     megaTaigaHills = giant_tree_taiga_hills,
    wooded_mountains,           extremeHillsPlus = wooded_mountains,
    savanna,
    savanna_plateau,            savannaPlateau = savanna_plateau,
    badlands,                   mesa = badlands,
    wooded_badlands_plateau,    mesaPlateau_F = wooded_badlands_plateau,
    badlands_plateau,           mesaPlateau = badlands_plateau,
    // 40  --  1.13
    small_end_islands,
    end_midlands,
    end_highlands,
    end_barrens,
    warm_ocean,                 warmOcean = warm_ocean,
    lukewarm_ocean,             lukewarmOcean = lukewarm_ocean,
    cold_ocean,                 coldOcean = cold_ocean,
    deep_warm_ocean,            warmDeepOcean = deep_warm_ocean,
    deep_lukewarm_ocean,        lukewarmDeepOcean = deep_lukewarm_ocean,
    deep_cold_ocean,            coldDeepOcean = deep_cold_ocean,
    // 50
    deep_frozen_ocean,          frozenDeepOcean = deep_frozen_ocean,
    BIOME_NUM,

    the_void = 127,

    // mutated variants
    sunflower_plains                = plains+128,
    desert_lakes                    = desert+128,
    gravelly_mountains              = mountains+128,
    flower_forest                   = forest+128,
    taiga_mountains                 = taiga+128,
    swamp_hills                     = swamp+128,
    ice_spikes                      = snowy_tundra+128,
    modified_jungle                 = jungle+128,
    modified_jungle_edge            = jungle_edge+128,
    tall_birch_forest               = birch_forest+128,
    tall_birch_hills                = birch_forest_hills+128,
    dark_forest_hills               = dark_forest+128,
    snowy_taiga_mountains           = snowy_taiga+128,
    giant_spruce_taiga              = giant_tree_taiga+128,
    giant_spruce_taiga_hills        = giant_tree_taiga_hills+128,
    modified_gravelly_mountains     = wooded_mountains+128,
    shattered_savanna               = savanna+128,
    shattered_savanna_plateau       = savanna_plateau+128,
    eroded_badlands                 = badlands+128,
    modified_wooded_badlands_plateau = wooded_badlands_plateau+128,
    modified_badlands_plateau       = badlands_plateau+128,
    // 1.14
    bamboo_jungle                   = 168,
    bamboo_jungle_hills             = 169,
    // 1.16
    soul_sand_valley                = 170,
    crimson_forest                  = 171,
    warped_forest                   = 172,
    basalt_deltas                   = 173,
};


enum BiomeTempCategory
{
    Oceanic, Warm, Lush, Cold, Freezing, Special
};


STRUCT(PerlinNoise)
{
    int d[512];
    double a, b, c;
};

STRUCT(DoublePerlinNoise)
{
    double amplitude;
    double lacuna[2];
    double persist[2];
    PerlinNoise *octaves[2];
    int octcnt;
};

struct Layer;
typedef int (mapfunc_t)(const struct Layer *, int *, int, int, int, int);

STRUCT(Layer)
{
    mapfunc_t *getMap;

    int8_t mc;          // minecraft version
    int8_t zoom;        // zoom factor of layer
    int8_t edge;        // maximum border required from parent layer
    int scale;          // scale of this layer (cell = scale x scale blocks)

    int64_t layerSalt;  // processed salt or initialization mode
    int64_t startSalt;  // (depends on world seed) used to step PRNG forward
    int64_t startSeed;  // (depends on world seed) starting point for chunk seeds

    void *noise;        // (depends on world seed) noise map data
    void *data;         // generic data for custom layers

    Layer *p, *p2;      // parent layers
};

STRUCT(NetherNoise)
{
    // altitude and wierdness don't affect nether biomes
    // and the weight is a 5th noise parameter which is constant
    DoublePerlinNoise temperature;
    DoublePerlinNoise humidity;
    PerlinNoise oct[8]; // buffer for octaves in double perlin noise
};

typedef PerlinNoise EndNoise;


#ifdef __cplusplus
extern "C"
{
#endif

//==============================================================================
// Essentials
//==============================================================================

void initBiomes();

/* Applies the given world seed to the layer and all dependent layers. */
void setLayerSeed(Layer *layer, int64_t worldSeed);


//==============================================================================
// Noise
//==============================================================================

void perlinInit(PerlinNoise *rnd, int64_t *seed);
double samplePerlin(const PerlinNoise *rnd, double x, double y, double z);
double sampleSimplex2D(const PerlinNoise *rnd, double x, double y);

void doublePerlinInit(DoublePerlinNoise *rnd, int64_t *seed,
        PerlinNoise *octavesA, PerlinNoise *octavesB, int omin, int len);
double sampleDoublePerlin(const DoublePerlinNoise *rnd,
        double x, double y, double z);

//==============================================================================
// Nether (1.16+) and End (1.9+) Biome Generation
//==============================================================================

/**
 * Nether biomes are 3D, and generated at scale 1:4. Use voronoiAccess3D() to
 * convert coordinates at 1:1 scale to their 1:4 access. Biome checks for
 * structures are generally done at y=0.
 *
 * The function getNetherBiome() determines the nether biome at a given
 * coordinate at scale 1:4. The parameter 'ndel' is an output noise delta for
 * optimization purposes and can be ignored (nullable).
 *
 * Use mapNether2D() to get a 2D area of nether biomes at y=0, scale 1:4.
 *
 * The mapNether3D() function attempts to optimize the generation of a volume
 * at scale 1:4, ranging from (x,y,z) to (x+w, y+yh, z+h) [exclusive] and the
 * output is indexed with out[y_k*(w*h) + z_j*w + x_i]. If the optimization
 * parameter 'confidence' has a value less than 1.0, the generation will
 * generally be faster, but can yield incorrect results in some circumstances.
 *
 * The output buffer for the map-functions need only be of sufficient size to
 * hold the generated area (i.e. w*h or w*h*yh).
 */
void setNetherSeed(NetherNoise *nn, int64_t seed);
int getNetherBiome(const NetherNoise *nn, int x, int y, int z, float *ndel);
int mapNether2D(const NetherNoise *nn, int *out, int x, int z, int w, int h);
int mapNether3D(const NetherNoise *nn, int *out, int x, int z, int w, int h, int y, int yh, float confidence);

/**
 * End biome generation is based on simplex noise and varies only at a 1:16
 * chunk scale which can be generated with mapEndBiome(). The function mapEnd()
 * is a variation which also scales this up on a regular grid to 1:4. The final
 * access at a 1:1 scale is the standard voronoi layer.
 */
void setEndSeed(EndNoise *en, int64_t seed);
int mapEndBiome(const EndNoise *en, int *out, int x, int z, int w, int h);
int mapEnd(const EndNoise *en, int *out, int x, int z, int w, int h);

//==============================================================================
// Seed Helpers
//==============================================================================

/**
 * The seed pipeline:
 *
 * getLayerSalt(n)                -> layerSalt (ls)
 * layerSalt (ls), worldSeed (ws) -> startSalt (st), startSeed (ss)
 * startSeed (ss), coords (x,z)   -> chunkSeed (cs)
 *
 * The chunkSeed alone is enough to generate the first PRNG integer with:
 *   mcFirstInt(cs, mod)
 * subsequent PRNG integers are generated by stepping the chunkSeed forwards,
 * salted with startSalt:
 *   cs_next = mcStepSeed(cs, st)
 */

static inline int64_t mcStepSeed(int64_t s, int64_t salt)
{
    return s * (s * 6364136223846793005LL + 1442695040888963407LL) + salt;
}

static inline int mcFirstInt(int64_t s, int mod)
{
    int ret = (int)((s >> 24) % mod);
    if (ret < 0)
        ret += mod;
    return ret;
}

static inline int mcFirstIsZero(int64_t s, int mod)
{
    return (int)((s >> 24) % mod) == 0;
}

static inline int64_t getChunkSeed(int64_t ss, int x, int z)
{
    int64_t cs = ss + x;
    cs = mcStepSeed(cs, z);
    cs = mcStepSeed(cs, x);
    cs = mcStepSeed(cs, z);
    return cs;
}

static inline int64_t getLayerSalt(int64_t salt)
{
    int64_t ls = mcStepSeed(salt, salt);
    ls = mcStepSeed(ls, salt);
    ls = mcStepSeed(ls, salt);
    return ls;
}

static inline int64_t getStartSalt(int64_t ws, int64_t ls)
{
    int64_t st = ws;
    st = mcStepSeed(st, ls);
    st = mcStepSeed(st, ls);
    st = mcStepSeed(st, ls);
    return st;
}

static inline int64_t getStartSeed(int64_t ws, int64_t ls)
{
    int64_t ss = ws;
    ss = getStartSalt(ss, ls);
    ss = mcStepSeed(ss, 0);
    return ss;
}


//==============================================================================
// BiomeID Helpers
//==============================================================================

int biomeExists(int mc, int id);
int isOverworld(int mc, int id);
int getMutated(int mc, int id);
int getCategory(int mc, int id);
int areSimilar(int mc, int id1, int id2);
int isMesa(int id);
int isShallowOcean(int id);
int isDeepOcean(int id);
int isOceanic(int id);
int isSnowy(int id);


//==============================================================================
// Layers
//==============================================================================

//                             old names
mapfunc_t mapContinent;     // mapIsland
mapfunc_t mapZoomFuzzy;
mapfunc_t mapZoom;
mapfunc_t mapLand;          // mapAddIsland
mapfunc_t mapLand16;
mapfunc_t mapIsland;        // mapRemoveTooMuchOcean
mapfunc_t mapSnow;          // mapAddSnow
mapfunc_t mapSnow16;
mapfunc_t mapCool;          // mapCoolWarm
mapfunc_t mapHeat;          // mapHeatIce
mapfunc_t mapSpecial;
mapfunc_t mapMushroom;      // mapAddMushroomIsland
mapfunc_t mapDeepOcean;
mapfunc_t mapBiome;
mapfunc_t mapBamboo;        // mapAddBamboo
mapfunc_t mapNoise;         // mapRiverInit
mapfunc_t mapBiomeEdge;
mapfunc_t mapHills;
mapfunc_t mapRiver;
mapfunc_t mapSmooth;
mapfunc_t mapSunflower;     // mapRareBiome
mapfunc_t mapShore;
mapfunc_t mapSwampRiver;
mapfunc_t mapRiverMix;
mapfunc_t mapOceanTemp;
mapfunc_t mapOceanMix;

// final layer 1:1
mapfunc_t mapVoronoi;       // mapVoronoiZoom
mapfunc_t mapVoronoi114;

// With 1.15 voronoi changed in preparation for 3D biome generation.
// Biome generation now stops at scale 1:4 OceanMix and voronoi is just an
// access algorithm, mapping the 1:1 scale onto its 1:4 correspondent.
// It is seeded by the first 8-bytes of the SHA-256 hash of the world seed.
int64_t getVoronoiSHA(int64_t worldSeed) __attribute__((const));
void voronoiAccess3D(int64_t sha, int x, int y, int z, int *x4, int *y4, int *z4);


#ifdef __cplusplus
}

//=================================================================================
//HERE START THE NODE ADDON PART
//=================================================================================


//mcStepSeed (s,salt) has a return value
static napi_value mcStepSeedMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int s = args[0];
  int salt = args[1];
  int RETURN = mcStepSeed(s,salt);
  return RETURN;
}

//mcFirstInt (s,mod) has a return value
static napi_value mcFirstIntMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int s = args[0];
  int mod = args[1];
  int RETURN = mcFirstInt(s,mod);
  return RETURN;
}

//mcFirstIsZero (s,mod) has a return value
static napi_value mcFirstIsZeroMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int s = args[0];
  int mod = args[1];
  int RETURN = mcFirstIsZero(s,mod);
  return RETURN;
}

//getChunkSeed (ss,x,z) has a return value
static napi_value getChunkSeedMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 3;
  napi_value args[3];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int ss = args[0];
  int x = args[1];
  int z = args[2];
  int RETURN = getChunkSeed(ss,x,z);
  return RETURN;
}

//getLayerSalt (seed) has a return value
static napi_value getLayerSaltMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int salt = args[0];
  int RETURN = getLayerSalt(salt);
  return RETURN;
}

//getStartSalt (ws,ls) has a return value
static napi_value getStartSaltMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int ws = args[0];
  int ls = args[1];
  int RETURN = getStartSalt(ws,ls);
  return RETURN;
}

//getStartSeed (ws,ls) has a return value
static napi_value getStartSeedMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int ws = args[0];
  int ls = args[1];
  int RETURN = getStartSeed(ws,ls);
  return RETURN;
}
#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

//Initializer
static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("mcStepSeed", mcStepSeedMethod);
  status = napi_define_properties(env, exports, 1, &desc);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("mcFirstInt", mcFirstIntMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("mcFirstIsZero", mcFirstIsZeroMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("getChunkSeed", getChunkSeedMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("getLayerSalt", getLayerSaltMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("getStartSalt", getStartSaltMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("getStartSeed", getStartSeedMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  return exports;
}

//EXPORT
NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

#endif

#endif /* LAYER_H_ */
