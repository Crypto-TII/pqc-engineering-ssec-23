//
//  Utility functions: 64-bits built-in functions
//

#ifndef SSEC_UTILITIES_H
#define SSEC_UTILITIES_H

#include <stdio.h>
#include <stdint.h>

#if defined(DEBUG)
#define NUMBER_OF_BENCHMARK_EXPERIMENTS 1
#define NUMBER_OF_TEST_EXPERIMENTS 1
#else
#define NUMBER_OF_BENCHMARK_EXPERIMENTS 500
#define NUMBER_OF_TEST_EXPERIMENTS 500
#endif

void multiplication_u64(uint64_t *output_low, uint64_t *output_high, uint64_t input_a, uint64_t input_b);

void conditional_move_u64(uint64_t *output, uint8_t input_decision, uint64_t input_a, uint64_t input_b);

uint64_t constant_time_is_nonzero_u64(uint64_t x);
uint64_t constant_time_is_zero_u64(uint64_t x);
uint64_t constant_time_is_lessthan_u64(uint64_t x, uint64_t y);

uint8_t constant_time_compare(const uint8_t *input_a, const uint8_t *input_b, uint64_t input_length);

void constant_time_conditional_mov(uint8_t *output, const uint8_t *input, uint64_t input_length, uint8_t input_selector);

void multiprecision_shift_to_right(uint64_t *input_a_output_shifted_a, uint64_t input_words_length);
void multiprecision_shift_to_left(uint64_t *input_a_output_shifted_a, uint64_t input_words_length);

#define addition_with_carry_u64(output, output_carry, input_carry, input_a, input_b)    \
    { uint64_t tmp = (input_a) + (uint64_t)(input_carry);                               \
    (output) = (input_b) + tmp;                                                         \
    (output_carry) = (constant_time_is_lessthan_u64(tmp, (uint64_t)(input_carry)) | constant_time_is_lessthan_u64((output), tmp)); }

#define subtraction_with_borrow_u64(output, output_borrow, input_borrow, input_a, input_b)                          \
    { uint64_t tmp = (input_a) - (input_b);                                                                         \
    uint64_t borrowReg = (constant_time_is_lessthan_u64((input_a), (input_b)) | ((input_borrow) & constant_time_is_zero_u64(tmp)));   \
    (output) = tmp - (uint64_t)(input_borrow);                                                                      \
    (output_borrow) = borrowReg; }

#define constant_time_shift_to_right_u64(shiftOut, highIn, lowIn, shift, DigitSize)  \
    (shiftOut) = ((lowIn) >> (shift)) ^ ((highIn) << ((DigitSize) - (shift)));

#define constant_time_shift_to_left_u64(shiftOut, highIn, lowIn, shift, DigitSize)  \
    (shiftOut) = ((highIn) << (shift)) ^ ((lowIn) >> ((DigitSize) - (shift)));

void multiprecision_addition(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length);
void multiprecision_subtraction(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length);

uint8_t multiprecision_is_smaller(const uint64_t *input1, const uint64_t *input2, uint64_t input_length);


static void cmov(uint32_t *r, const uint32_t a, uint32_t b) {
    // decision bit b has to be either 0 or 1
	uint32_t t;
	b = -b; /* Now b is either 0 or 0xffffffff */
	t = (*r ^ a) & b;
	*r ^= t;
}

static uint32_t isequal(uint32_t a, uint32_t b) {
	// check if a and b are equal in constant time
	uint32_t r = 0;
	unsigned char *ta = (unsigned char *)&a;
	unsigned char *tb = (unsigned char *)&b;
	r = (ta[0] ^ tb[0]) | (ta[1] ^ tb[1]) | (ta[2] ^ tb[2]) |  (ta[3] ^ tb[3]);
	r = (-r);
	r = r >> 31;
	return (uint32_t)(1-r);
}

static uint32_t lookup(size_t pos, uint32_t const table[], size_t N) {
    // get priv[pos] in constant time
	int b;
	uint32_t r = table[0];
	for(size_t i = 1; i < N; i++)
	{
		b = isequal(i, pos);
		cmov(&r, table[i], b);
	}
	return r;
}

static int32_t issmaller(int32_t x, int32_t y) {
    // constant-time comparison: -1 if x < y, 0 otherwise.
	int32_t xy = x ^ y;
	int32_t c = x - y;
	c ^= xy & (c ^ x);
	return (c >> 31);
}

static void to_trit_string(uint32_t *output, const uint8_t *input, size_t length) {

    uint32_t table[243] = {
        0x00000, 0x00001, 0x00002,
        0x00010, 0x00011, 0x00012,
        0x00020, 0x00021, 0x00022,
        0x00100, 0x00101, 0x00102,
        0x00110, 0x00111, 0x00112,
        0x00120, 0x00121, 0x00122,
        0x00200, 0x00201, 0x00202,
        0x00210, 0x00211, 0x00212,
        0x00220, 0x00221, 0x00222,
        0x01000, 0x01001, 0x01002,
        0x01010, 0x01011, 0x01012,
        0x01020, 0x01021, 0x01022,
        0x01100, 0x01101, 0x01102,
        0x01110, 0x01111, 0x01112,
        0x01120, 0x01121, 0x01122,
        0x01200, 0x01201, 0x01202,
        0x01210, 0x01211, 0x01212,
        0x01220, 0x01221, 0x01222,
        0x02000, 0x02001, 0x02002,
        0x02010, 0x02011, 0x02012,
        0x02020, 0x02021, 0x02022,
        0x02100, 0x02101, 0x02102,
        0x02110, 0x02111, 0x02112,
        0x02120, 0x02121, 0x02122,
        0x02200, 0x02201, 0x02202,
        0x02210, 0x02211, 0x02212,
        0x02220, 0x02221, 0x02222,
        0x10000, 0x10001, 0x10002,
        0x10010, 0x10011, 0x10012,
        0x10020, 0x10021, 0x10022,
        0x10100, 0x10101, 0x10102,
        0x10110, 0x10111, 0x10112,
        0x10120, 0x10121, 0x10122,
        0x10200, 0x10201, 0x10202,
        0x10210, 0x10211, 0x10212,
        0x10220, 0x10221, 0x10222,
        0x11000, 0x11001, 0x11002,
        0x11010, 0x11011, 0x11012,
        0x11020, 0x11021, 0x11022,
        0x11100, 0x11101, 0x11102,
        0x11110, 0x11111, 0x11112,
        0x11120, 0x11121, 0x11122,
        0x11200, 0x11201, 0x11202,
        0x11210, 0x11211, 0x11212,
        0x11220, 0x11221, 0x11222,
        0x12000, 0x12001, 0x12002,
        0x12010, 0x12011, 0x12012,
        0x12020, 0x12021, 0x12022,
        0x12100, 0x12101, 0x12102,
        0x12110, 0x12111, 0x12112,
        0x12120, 0x12121, 0x12122,
        0x12200, 0x12201, 0x12202,
        0x12210, 0x12211, 0x12212,
        0x12220, 0x12221, 0x12222,
        0x20000, 0x20001, 0x20002,
        0x20010, 0x20011, 0x20012,
        0x20020, 0x20021, 0x20022,
        0x20100, 0x20101, 0x20102,
        0x20110, 0x20111, 0x20112,
        0x20120, 0x20121, 0x20122,
        0x20200, 0x20201, 0x20202,
        0x20210, 0x20211, 0x20212,
        0x20220, 0x20221, 0x20222,
        0x21000, 0x21001, 0x21002,
        0x21010, 0x21011, 0x21012,
        0x21020, 0x21021, 0x21022,
        0x21100, 0x21101, 0x21102,
        0x21110, 0x21111, 0x21112,
        0x21120, 0x21121, 0x21122,
        0x21200, 0x21201, 0x21202,
        0x21210, 0x21211, 0x21212,
        0x21220, 0x21221, 0x21222,
        0x22000, 0x22001, 0x22002,
        0x22010, 0x22011, 0x22012,
        0x22020, 0x22021, 0x22022,
        0x22100, 0x22101, 0x22102,
        0x22110, 0x22111, 0x22112,
        0x22120, 0x22121, 0x22122,
        0x22200, 0x22201, 0x22202,
        0x22210, 0x22211, 0x22212,
        0x22220, 0x22221, 0x22222
    };

    for(size_t i = 0; i < length; i++) {
        output[i] = lookup(input[i], table, 243);
    }
}

#endif // SSEC_UTILITIES_H
