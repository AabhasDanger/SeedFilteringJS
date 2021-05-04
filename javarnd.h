#ifndef JAVARND_H_
#define JAVARND_H_
#include <assert.h>
#include <node_api.h>
#include <stdint.h>


/********************** C copy of the Java Random methods **********************
 */

static inline void setSeed(int64_t *seed, int64_t value)
{
    *seed = (value ^ 0x5deece66d) & ((1LL << 48) - 1);
}

static inline int next(int64_t *seed, const int bits)
{
    *seed = (*seed * 0x5deece66d + 0xb) & ((1LL << 48) - 1);
    return (int) (*seed >> (48 - bits));
}

static inline int nextInt(int64_t *seed, const int n)
{
    int bits, val;
    const int m = n - 1;

    if((m & n) == 0) return (int) ((n * (int64_t)next(seed, 31)) >> 31);

    do {
        bits = next(seed, 31);
        val = bits % n;
    }
    while (bits - val + m < 0);
    return val;
}

static inline int64_t nextLong(int64_t *seed)
{
    return ((int64_t) next(seed, 32) << 32) + next(seed, 32);
}

static inline float nextFloat(int64_t *seed)
{
    return next(seed, 24) / (float) (1 << 24);
}

static inline double nextDouble(int64_t *seed)
{
    return (((int64_t) next(seed, 26) << 27) + next(seed, 27)) / (double) (1LL << 53);
}

/* A macro to generate the ideal assembly for X = nextInt(S, 24)
 * This is a macro and not an inline function, as many compilers can make use
 * of the additional optimisation passes for the surrounding code.
 */
#define JAVA_NEXT_INT24(S,X)                \
    do {                                    \
        int64_t a = (1ULL << 48) - 1;       \
        int64_t c = 0x5deece66dLL * (S);    \
        c += 11; a &= c;                    \
        (S) = a;                            \
        a >>= 17;                           \
        c = 0xaaaaaaab * a;                 \
        c >>= 36;                           \
        (X) = (int)a - (int)(c << 3) * 3;   \
    } while (0)


/* skipNextN
 * ---------
 * Jumps forwards in the random number sequence by simulating 'n' calls to next.
 */
static inline void skipNextN(int64_t *seed, uint64_t n)
{
    int64_t m = 1;
    int64_t a = 0;
    int64_t im = 0x5deece66dLL;
    int64_t ia = 0xbLL;
    uint64_t k;

    for (k = n; k; k >>= 1)
    {
        if (k & 1)
        {
            m *= im;
            a = im * a + ia;
        }
        ia = (im + 1) * ia;
        im *= im;
    }

    *seed = *seed * m + a;
    *seed &= 0xffffffffffffLL;
}

/* invSeed48
 * ---------
 * Returns the previous 48-bit seed which will generate 'nseed'.
 * The upper 16 bits are ignored, both here and in the generator.
 */
static inline __attribute__((const))
int64_t invSeed48(int64_t nseed)
{
    const int64_t x = 0x5deece66d;
    const int64_t xinv = 0xdfe05bcb1365LL;
    const int64_t y = 0xbLL;
    const int64_t m48 = 0xffffffffffffLL;

    int64_t a = nseed >> 32;
    int64_t b = nseed & 0xffffffffLL;
    if (b & 0x80000000LL) a++;

    int64_t q = ((b << 16) - y - (a << 16)*x) & m48;
    int64_t k;
    for (k = 0; k <= 5; k++)
    {
        int64_t d = (x - (q + (k << 48))) % x;
        d = (d + x) % x; // force the modulo and keep it positive
        if (d < 65536)
        {
            int64_t c = ((q + d) * xinv) & m48;
            if (c < 65536)
            {
                return ((((a << 16) + c) - y) * xinv) & m48;
            }
        }
    }
    return -1;
}


/* Find the modular inverse: (1/x) | mod m.
 * Assumes x and m are positive and co-prime.
 */
static inline __attribute__((const))
int64_t mulInv(int64_t x, int64_t m)
{
    int64_t t, q, a, b, n;
    if (m <= 1)
        return 0; // no solution

    n = m;
    a = 0; b = 1;

    while (x > 1)
    {
        if (m == 0)
            return 0; // x and m are co-prime
        q = x / m;
        t = m; m = x % m;     x = t;
        t = a; a = b - q * a; b = t;
    }

    if (b < 0)
        b += n;
    return b;
}

//===================================================================
//THIS IS THE START OF THE JS CONVERSION
//===================================================================

//setSeed(seed,value) doesn't have a return Value
static napi_value setSeedMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int value = args[1];
  setSeed(seed,value);
  return NULL;
}

//next(seed,bits) have a return Value
static napi_value nextMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int bits = args[1];
  int RETURN = setSeed(seed,bits);
  return RETURN;
}

//nextInt(seed,n) have a return Value
static napi_value nextIntMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int n = args[1];
  int RETURN = next(seed,n);
  return RETURN;
}

//nextLong(seed) have a return Value
static napi_value nextLongMethod(napi_env env, napi_callback_info info){
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int RETURN = nextLong(seed);
  return RETURN;
}

//nextFloat(seed) have a return Value
static napi_value nextFloatMethod(napi_env env, napi_callback_info info){
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int RETURN = nextFloat(seed);
  return RETURN;
}

//nextDouble(seed) have a return Value
static napi_value nextDoubleMethod(napi_env env, napi_callback_info info){
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int RETURN = nextDouble(seed);
  return RETURN;
}

//skipNextN(seed,n) doesn't have a return Value
static napi_value skipNextNMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int n = args[1];
  skipNextN(seed,n);
  return NULL;
}

//invSeed48(seed) have a return Value
static napi_value invSeed48Method(napi_env env, napi_callback_info info){
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int seed = args[0];
  int RETURN = invSeed48(seed);
  return RETURN;
}

//mulInv(x,m) have a return Value
static napi_value mulInvMethod(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int x = args[0];
  int m = args[1];
  int RETURN = mulInv(x,m);
  return RETURN;
}

//JAVA_NEXT_INT24(S,X)
static napi_value JAVA_NEXT_INT24Method(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env,info,&argc, args, NULL,NULL);
  int S = args[0];
  int X = args[1];
  int RETURN = JAVA_NEXT_INT24(S,X);
  return RETURN;
}

//Initializer
static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("setSeed", setSeedMethod);
  status = napi_define_properties(env, exports, 1, &desc);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("next", nextMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("nextInt", nextIntMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("nextLong", nextLongMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("nextFloat", nextFloatMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("nextDouble", nextDoubleMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("skipNextN", skipNextNMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("invSeed48", invSeed48Method);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("mulInv", mulInvMethod);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("JAVA_NEXT_INT24", JAVA_NEXT_INT24Method);
  status = napi_define_properties(env, exports, 1, &desc2);
  assert(status == napi_ok);
  return exports;
}

//EXPORT
NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

#endif /* JAVARND_H_ */
