// I am using c++ to build this, because
// I think bit wise operation should better doing in C++.
// low level.

#include <iostream>
#include <bitset>
#include <iomanip>

#define M_SIZE 16
#define INT_BITS 32

bool no_steps = false;

typedef unsigned char Block512;

//////// Initialize data
// Use binary integer part of the sines of integers (Radians) as constants:
// for i from 0 to 63 do
//     K[i] = floor(232 × abs (sin(i + 1)))
// end for
// (Or just use the following precomputed table):
unsigned int K[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

// S specifies the per-round shift amounts
int S[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

// per-round modula addition M index
int R[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
    5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
    0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9};

// Initialze vector
int I_A = 0x67452301;
int I_B = 0xefcdab89;
int I_C = 0x98badcfe;
int I_D = 0x10325476;

// MD5 Functions
unsigned int F(unsigned int B, unsigned int C, unsigned int D)
{
    return (B & C | ~B & D);
}

unsigned int G(unsigned int B, unsigned int C, unsigned int D)
{
    return (B & D | C & ~D);
}

unsigned int H(unsigned int B, unsigned int C, unsigned int D)
{
    return (B ^ C ^ D);
}

unsigned int I(unsigned int B, unsigned int C, unsigned int D)
{
    return (C ^ (B | ~D));
}

// Custom function
void print_binary(const char *str, int len)
{
    for (int i = 0; i < len; i++)
    {
        char a = str[i];
        std::bitset<8> x(a);
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int left_rotate(unsigned int n, int d)
{

    /* In n<<d, last d bits are 0. To
     put first 3 bits of n at
    last, do bitwise or of n<<d
    with n >>(INT_BITS - d) */
    return (n << d) | (n >> (INT_BITS - d));
}

Block512 *encode_blocks(const char *str, int *number_of_blocks, int *padding_bits)
{
    int padding_size = *padding_bits;

    size_t len = strlen(str);
    int msg_bits = len * 8;
    *number_of_blocks = 1 + (msg_bits + 64) / 512;

    padding_size = int(msg_bits % 512) - 448;
    if (padding_size < 0)
        padding_size = abs(padding_size);
    else
        padding_size = 512 - padding_size;

    Block512 *blocks;
    if (msg_bits < 448)
        blocks = (Block512 *)calloc((padding_size / 8) + len + 8, sizeof(unsigned char));
    else
        blocks = (Block512 *)calloc(64 * *number_of_blocks, sizeof(unsigned char));

    // copy number of bit of the str to the end of the block
    *((uint64_t *)(blocks + (padding_size / 8) + len)) = msg_bits;

    // // copy str to block
    memcpy(blocks, str, sizeof(unsigned char) * len);

    // // add the bit 1 to block at the end of the str.
    Block512 *temp_block = blocks + len;
    *temp_block = 128;

    *padding_bits = padding_size;
    return blocks;
}

unsigned int *md5(const char *plaint_text)
{
    // function pointer array F, G, H, I
    // unsigned int F(unsigned int B, unsigned int C, unsigned int D);
    unsigned int (*funcs[])(unsigned int, unsigned int, unsigned int) = {&F, &G, &H, &I};

    // initial vector A, B, C, D
    unsigned int *MD5 = new unsigned int[4];
    MD5[0] = I_A;
    MD5[1] = I_B;
    MD5[2] = I_C;
    MD5[3] = I_D;

    // Encode blocks
    int padding_bits = 0;
    int number_of_blocks = 0;
    Block512 *blocks = encode_blocks(plaint_text, &number_of_blocks, &padding_bits);

    // print complete block
    if (no_steps == false)
    {
        std::cout << "##### COMPLETE BLOCK DATA #####" << std::endl;
        print_binary((const char *)blocks, 64 * number_of_blocks);
        std::cout << std::endl;
    }

    // STEP - 2 SUFFLE
    for (int k = 0; k < number_of_blocks; k++)
    {
        unsigned int prev_MD5[4] = {MD5[0], MD5[1], MD5[2], MD5[3]};

        // split into 16 words
        // print M0 - M15
        unsigned int *M = new unsigned int[M_SIZE];

        for (int i = 0; i < 16; i++)
        {
            M[i] = ((unsigned int *)blocks)[16 * k + i];
            if (no_steps == false)
            {
                std::cout << "M" << std::setfill('0') << std::setw(2) << i << std::setw(2) << " - ";
                print_binary((const char *)(M + i), 4);
            }
        }
        if (no_steps == false)
            std::cout << std::endl;

        for (int i = 0; i < 4; i++)
        {
            if (no_steps == false)
                std::cout << "########### ROUND " << i << " ##############" << std::endl;
            for (int j = 0; j < 16; j++)
            {
                if (no_steps == false)
                {
                    for (int i = 0; i < 4; i++)
                        std::cout << std::hex << MD5[i] << std::endl;
                    std::cout << std::endl;
                }

                unsigned int func_partial_result;
                unsigned int modular_addition_result;
                unsigned int new_A;

                func_partial_result = funcs[i](MD5[1], MD5[2], MD5[3]);

                // modular addition
                modular_addition_result = (MD5[0] + func_partial_result);

                // modular adiition partial msg
                new_A = (M[R[16 * i + j]] + modular_addition_result);

                // modular adiition constance
                new_A = (K[16 * i + j] + new_A);

                // left rotate
                new_A = left_rotate(new_A, S[16 * i + j]);

                // modular addition B
                new_A = (MD5[1] + new_A);

                // replace with new_A and right rotate MD5
                unsigned int temp[4] = {MD5[0], MD5[1], MD5[2], MD5[3]};
                MD5[1] = new_A;
                MD5[2] = temp[1];
                MD5[3] = temp[2];
                MD5[0] = temp[3];
            }
        }
        MD5[0] += prev_MD5[0];
        MD5[1] += prev_MD5[1];
        MD5[2] += prev_MD5[2];
        MD5[3] += prev_MD5[3];
    }

    return MD5;
}

int main(int argc, char *argv[])
{
    int argv_len = 0;
    char *plaint_text = argv[1];

    if (argc < 2)
    {
        std::cout << "Argument CANNOT be null!" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 3 && strcmp(argv[2], "--nosteps") == 0)
        no_steps = true;

    unsigned int *MD5 = md5(plaint_text);

    // print result
    unsigned int little_endian_MD5[] = {MD5[0], MD5[1], MD5[2], MD5[3]};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            memcpy((unsigned char *)(little_endian_MD5 + i) + j, (unsigned char *)(MD5 + i) - j + 3, sizeof(unsigned char));

        std::cout << std::hex << std::setfill('0') << std::setw(8) << little_endian_MD5[i] << std::setw(0) << std::dec;
    }

    return EXIT_SUCCESS;
}