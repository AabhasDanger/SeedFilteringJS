let currentseed;


function setSeed(seed,value){
    currentseed = seed
    currentseed = (value ^ 0x5deece66d) & ((1 << 48) - 1);
}

function next(seed,bits){
    currentseed = (seed * 0x5deece66d + 0xb) & ((1<< 48) - 1);
    return (seed >> (48 - bits));
}

function nextInt(seed,n){
    let bits, val;
    const m = n - 1;

    if((m & n) == 0) return ((n * next(seed, 31)) >> 31);

    do {
        bits = next(seed, 31);
        val = bits % n;
    }
    while (bits - val + m < 0);
    return val;
}

function nextLong(seed){
    return (next(seed, 32) << 32) + next(seed, 32);
}

function nextFloat(seed){
    return next(seed, 24) / (1 << 24);
}

function nextDouble(seed){
    return ((next(seed, 26) << 27) + next(seed, 27)) /  (1 << 53);
}