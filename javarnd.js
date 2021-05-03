let currentseed;


function setSeed(seed,value){
    currentseed = (value ^ 0x5deece66d) & ((1 << 48) - 1);
}

function next(seed,bits){
    currentseed = (currentseed * 0x5deece66d + 0xb) & ((1<< 48) - 1);
    return (int) (currentseed >> (48 - bits));
}
