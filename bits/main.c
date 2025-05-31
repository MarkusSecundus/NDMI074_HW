#include<stdio.h>
#include<stdbool.h>


#define NUMBITS (sizeof(unsigned) * 8) // guaranteed to be a power of 2
_Static_assert(NUMBITS == 32, "broken assumption that `unsigned int` is 32bit!");


//rotate right
unsigned rotr(unsigned i, unsigned rotation_amount){
    rotation_amount &= (NUMBITS-1); // modulo 32
    return (i >> rotation_amount) | (i<<(NUMBITS - rotation_amount));
}

//rotate left
unsigned rotl(unsigned i, unsigned rotation_amount){
    rotation_amount &= (NUMBITS-1); // modulo 32
    return (i << rotation_amount) | (i>>(NUMBITS - rotation_amount));
}


unsigned multiply_by_power_of_two_v1(unsigned i, unsigned log2ofTheMultiplier){
    return (unsigned)(((unsigned long long)i) << log2ofTheMultiplier);
}


static unsigned helper_extract_most_significant_bit(unsigned u){
    unsigned mask = 0xFFFF0000&u ? 0xFFFF0000 : 0x0000FFFF; //could be written without branching e.g. like this: `((!!(0xFFFF0000&u))*0xFFFF0000) | ((!(0xFFFF0000&u))*0x0000FFFF)`, but on ARM64 this will get compiled to some kind of conditional load instruction anyway
    u &= mask;
    unsigned mask2 = 0xFF00FF00&u ? 0xFF00FF00 : 0x00FF00FF;
    u &= mask2;
    unsigned mask3 = 0xF0F0F0F0&u ? 0xF0F0F0F0 : 0x0F0F0F0F;
    u &= mask3;
    unsigned mask4 = 0xcccccccc&u ? 0xcccccccc : 0x33333333;
    u &= mask4;
    unsigned mask5 = 0xaaaaaaaa&u ? 0xaaaaaaaa : 0x55555555;
    u &= mask5;
    return u;
}
static unsigned helper_extract_most_significant_bit_and_get_log2(unsigned u, unsigned *out_log2){
    unsigned log = 0;
    unsigned mask = 0xFFFF0000&u ? (log += 16, 0xFFFF0000) : 0x0000FFFF;
    u &= mask;
    unsigned mask2 = 0xFF00FF00&u ? (log += 8, 0xFF00FF00) : 0x00FF00FF;
    u &= mask2;
    unsigned mask3 = 0xF0F0F0F0&u ? (log += 4, 0xF0F0F0F0) : 0x0F0F0F0F;
    u &= mask3;
    unsigned mask4 = 0xcccccccc&u ? (log += 2, 0xcccccccc) : 0x33333333;
    u &= mask4;
    unsigned mask5 = 0xaaaaaaaa&u ? (log += 1, 0xaaaaaaaa) : 0x55555555;
    u &= mask5;
    if(out_log2) *out_log2 = log;
    return u;
}


//not sure which of these versions is the one that we are supposed to implement
unsigned multiply_by_power_of_two_v2(unsigned i, unsigned multiplier){
    unsigned log;
    helper_extract_most_significant_bit_and_get_log2(multiplier, &log); //other option would be `log = popcnt(multiplier-1)`
    return multiply_by_power_of_two_v1(i, log);
}


unsigned multiply_by_20(unsigned i){
    return (i << 4) + (i<<2); // i*16 + i*4
}



unsigned div_round_up(unsigned i, unsigned divider){
    return (unsigned)(((unsigned long long)i+divider-1)/divider);  //compute in 64 number because otherwise we can overflow when adding the divider
}
unsigned div_round_half_up(unsigned i, unsigned divider){
    return (unsigned)(((unsigned long long)i+(divider>>1))/divider); //compute in 64 number because otherwise we can overflow when adding the divider
}

bool is_powerof_2(unsigned i){
    return (i & (i-1)) == 0;
}


unsigned mirror_bytes(unsigned i){
    i = (i>>16) | (i<<16);
    i = ((i&0x00FF00FF)<<8) | ((i&0xFF00FF00)>>8);
    return i;
}

unsigned mirror_bits(unsigned i){
    i = (i>>16) | (i<<16);
    i = ((i&0xFF00FF00)>>8) | ((i&0x00FF00FF)<<8);
    i = ((i&0xF0F0F0F0)>>4) | ((i&0x0F0F0F0F)<<4);
    i = ((i&0xcccccccc)>>2) | ((i&0x33333333)<<2); //00110011.. and 11001100..
    i = ((i&0xaaaaaaaa)>>1) | ((i&0x55555555)<<1); //01010101.. and 10101010..
    return i;
}

unsigned long long round_to_greater_powerof2_sharp(unsigned u){
    return ((unsigned long long)helper_extract_most_significant_bit(u))<<1;
}
unsigned long long round_to_greater_powerof2_nonsharp(unsigned u){
    //only supports numbers up to `1<<31`, returns `0` for greater numbers
    if(u == 0) return 0;
    return round_to_greater_powerof2_sharp(u-1);
}






#define ARRAYSIZE(...) (sizeof(__VA_ARGS__)/sizeof(*(__VA_ARGS__)))
int main(){

    //for(unsigned t=0;t<30;++t){
    //    const unsigned divider = 8;
    //    printf("%d / %d == %d (%.2f)\n", t, divider, div_round_half_up(t, divider), t*1.0f/divider);
    //}
    //return 0;


    //unsigned arr[] = {-1, 0, 1, 30, 32, 477, 9999, 64000, 78888, 987654, 987654321, 1<<31, (1<<31) + 9};
    //for(int t=0;t<ARRAYSIZE(arr);++t){
    //    unsigned log;
    //    unsigned long long msb = helper_extract_most_significant_bit_and_get_log2(arr[t], &log);
    //    printf("log2(%u) = %llu (%llu; 1<<%u)\n", arr[t], round_to_greater_powerof2_nonsharp(arr[t]), ((unsigned long long)1)<<log, log);
    //}

    return 0;
}