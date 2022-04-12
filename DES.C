#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 상수 정의 */
#define BLOCK_SIZE	 8		// DES 블록 사이즈

/* 타입 정의 */
typedef unsigned char BYTE;
typedef unsigned int UINT;

/* 함수 선언 */

void EP(UINT r, BYTE* out);										// 확장 치환 함수
UINT Permutation(UINT in);										// 치환 함수
UINT S_box_Transfer(BYTE* in);									// S-box 변환 함수
UINT f(UINT in, BYTE* rkey);									// f 함수
void BtoW(BYTE *in, UINT *x, UINT *y);							// byte를 word로 바꾸는 함수
void WtoB(UINT l, UINT r, BYTE *out);
void IP(BYTE *in, BYTE *out);									// 초기 치환 함수
void In_IP(BYTE *in, BYTE *out);								// 역 초기 치환 함수

																/* 전역 변수 */

																// 초기 치환 테이블
BYTE ip[64] = { 58, 50, 42, 34, 26, 18, 10, 2,
60, 52, 44, 36 ,28, 20, 12, 4,
62, 54, 46, 38, 30, 22, 14, 6,
64, 56, 48, 40, 32, 24, 16, 8,
57, 49, 41, 33, 25, 17,  9, 1,
59, 51, 43, 35, 27, 19, 11, 3,
61, 53, 45, 37, 29, 21, 13, 5,
63, 55, 47, 39, 31, 23, 15, 7 };

// 역 초기 치환 테이블
BYTE ip_1[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
39, 7, 47, 15, 55, 23, 63, 31,
38, 6, 46, 14, 54, 22, 62, 30,
37, 5, 45, 13, 53, 21, 61, 29,
36, 4, 44, 12, 52, 20, 60, 28,
35, 3, 43, 11, 51, 19, 59, 27,
34, 2, 42, 10, 50, 18, 58, 26,
33, 1, 41,  9, 49, 17, 57, 25 };
// 확장 치환 테이블
BYTE E[48] = { 32,  1,  2,  3,  4,  5,  4,  5,
6,  7,  8,  9,  8,  9, 10, 11,
12, 13, 12, 13, 14, 15, 16, 17,
16, 17, 18, 19, 20, 21, 20, 21,
22, 23, 24, 25, 24, 25, 26, 27,
28, 29, 28, 29, 30, 31, 32, 1 };

// 치환 테이블
BYTE P[32] = { 16,  7, 20, 21, 29, 12, 28, 17,
1, 15, 23, 26,  5, 18, 31, 10,
2,  8, 24, 14, 32, 27,  3,  9,
19, 13, 30,  6, 22, 11,  4, 25 };


// S-BOX 테이블
BYTE s_box[8][4][16] =
{
	{ 14,  4, 13, 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9, 0,  7,
	0, 15,  7, 4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5, 3,  8,
	4,  1, 14, 8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10, 5,  0,
	15, 12,  8, 2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0, 6, 13 },

	{ 15,  1,  8, 14,  6, 11,  3,  4,  9, 7,  2, 13, 12, 0,  5, 10,
	3, 13,  4,  7, 15,  2,  8, 14, 12, 0,  1, 10,  6, 9, 11,  5,
	0, 14,  7, 11, 10,  4, 13,  1,  5, 8, 12,  6,  9, 3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11, 6,  7, 12,  0, 5, 14,  9 },

	{ 10,  0,  9, 14, 6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9, 3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9, 8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	1, 10, 13,  0, 6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 },

	{ 7, 13, 14, 3,  0,  6,  9, 10,  1, 2, 8,  5, 11, 12,  4, 15,
	13,  8, 11, 5,  6, 15,  0,  3,  4, 7, 2, 12,  1, 10, 14,  9,
	10,  6,  9, 0, 12, 11,  7, 13, 15, 1, 3, 14,  5,  2,  8,  4,
	3, 15,  0, 6, 10,  1, 13,  8,  9, 4, 5, 11, 12,  7,  2, 14 },

	{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13, 0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3, 9,  8,  6,
	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6, 3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10, 4,  5,  3 },

	{ 12,  1, 10, 15, 9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2, 7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	9, 14, 15,  5, 2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	4,  3,  2, 12, 9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },

	{ 4, 11,  2, 14, 15, 0,  8, 13,  3, 12, 9,  7,  5, 10, 6,  1,
	13,  0, 11,  7,  4, 9,  1, 10, 14,  3, 5, 12,  2, 15, 8,  6,
	1,  4, 11, 13, 12, 3,  7, 14, 10, 15, 6,  8,  0,  5, 9,  2,
	6, 11, 13,  8,  1, 4, 10,  7,  9,  5, 0, 15, 14,  2, 3, 12 },

	{ 13,  2,  8, 4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	1, 15, 13, 8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	7, 11,  4, 1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	2,  1, 14, 7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
};

///////////////////////////////////////////////////////////////////////////////////////////////

void main()
{
	int i;
	BYTE input[BLOCK_SIZE] = {'2','0', '1', '8', '1', '2', '2', '2'};
	BYTE data[BLOCK_SIZE] = {0,};
	BYTE output[BLOCK_SIZE]={ 0, };
	BYTE round_key[6] = {'r','n', 'd', 'k', 'e', 'y'};
	UINT R = 0, L = 0;

	IP(input, data);
	BtoW(data, &L, &R);
	L = L ^ f(R, round_key);
	WtoB(L, R, data);
	In_IP(data, output);

	printf("\nInput:");
	for (i = 0; i < BLOCK_SIZE; i++)printf("%x", input[i]);
	printf("\nOutPut: ");
	for (i = 0; i < BLOCK_SIZE; i++)printf("%x", output[i]);
	printf("\n");

	IP(output, data);
	BtoW(data, &L, &R);
	L = L ^ f(R, round_key);
	WtoB(L, R, data);
	In_IP(data, input);

	printf("\nOutPut: ");
	for (i = 0; i < BLOCK_SIZE; i++)printf("%x", output[i]);
	printf("\n");
	printf("Input:");
	for (i = 0; i < BLOCK_SIZE; i++)printf("%x", input[i]);




}

// 초기 치환
void IP(BYTE *in, BYTE *out)
{
	int i;
	BYTE index, bit, mask = 0x80;

	for (i = 0; i<64; i++)
	{
		// 해당 bit의 위치를 계산
		index = (ip[i] - 1) / 8;
		bit = (ip[i] - 1) % 8;

		// 해당 bit의 값을 상위 bit부터 대입
		if (in[index] & (mask >> bit))
			out[i / 8] |= mask >> (i % 8);
	}
}

// 역 초기 치환
void In_IP(BYTE *in, BYTE *out)
{
	int i;
	BYTE index, bit, mask = 0x80;

	for (i = 0; i<64; i++)
	{
		// 해당 bit의 위치를 계산
		index = (ip_1[i] - 1) / 8;
		bit = (ip_1[i] - 1) % 8;

		// 해당 bit의 값을 상위 bit부터 대입
		if (in[index] & (mask >> bit))
			out[i / 8] |= mask >> (i % 8);
	}
}
// f 함수
UINT f(UINT r, BYTE* rkey)
{
	int i;
	BYTE data[6] = { 0, };
	UINT out;

	EP(r, data);	// 확장 치환

					// 라운드 키와 XOR
	for (i = 0; i<6; i++)
		data[i] = data[i] ^ rkey[i];

	out = Permutation(S_box_Transfer(data));		// S-box 변환 결과를 치환

	return out;
}

// 확장 치환 함수
void EP(UINT r, BYTE* out)
{
	int i;
	UINT mask = 0x80000000;

	// 확장 치환 (32 bit -> 48 bit)
	for (i = 0; i<48; i++)
	{
		// 해당 bit의 값을 상위 bit부터 대입
		if (r & (mask >> (E[i] - 1)))
		{
			out[i / 8] |= (BYTE)(0x80 >> (i % 8));
		}
	}
}

// 치환 함수
UINT Permutation(UINT in)
{
	int i;
	UINT out = 0, mask = 0x80000000;

	// P-box
	for (i = 0; i<32; i++)
	{
		// 해당 bit의 값을 상위 bit부터 대입
		if (in & (mask >> (P[i] - 1)))
			out |= (mask >> i);
	}

	return out;
}

// S-box 변환
UINT S_box_Transfer(BYTE* in)
{
	int i, row, column, shift = 28;
	UINT temp = 0, result = 0, mask = 0x00000080;

	for (i = 0; i<48; i++)
	{
		// 1bit 씩 값을 추출하여 temp에 저장
		if (in[i / 8] & (BYTE)(mask >> (i % 8)))
			temp |= 0x20 >> (i % 6);

		// 6 bit가 되었을 경우
		if ((i + 1) % 6 == 0)
		{
			row = ((temp & 0x20) >> 4) + (temp & 0x01);		// 6번째 bit와 첫번째 bit를 추출하여 S-box의 행의 값을 계산
			column = (temp & 0x1E) >> 1;					// 2번째 bit부터 5번째 bit를 추출하여 S-box의 열의 값을 계산

															// S-box를 적용한 결과 값 4 bit를 32 bit인 s_result에 상위 비트부터 채움
			result += ((UINT)s_box[i / 6][row][column] << shift);

			shift -= 4;		// 4 bit 씩 채우므로 시프트 횟수를 4씩 줄여줌
			temp = 0;		// 다음 6 bit를 계산하기 위해 초기화 해줌
		}
	}

	return result;
}

// 8bit(byte) 단위의 데이터를 32bit(word) 단위의 데이터로 변환
void BtoW(BYTE *in, UINT *x, UINT *y)
{
	int i;

	for (i = 0; i<8; i++)
	{
		if (i < 4)
			*x |= (UINT)in[i] << (24 - (i * 8));
		else
			*y |= (UINT)in[i] << (56 - (i * 8));
	}
}

// 32bit(word) 단위의 데이터를 8bit(byte) 단위의 데이터로 변환
void WtoB(UINT l, UINT r, BYTE *out)
{
	int i;
	UINT mask = 0xFF000000;

	for (i = 0; i<8; i++)
	{
		if (i < 4)
			out[i] = (l & (mask >> i * 8)) >> (24 - (i * 8));
		else
			out[i] = (r & (mask >> (i - 4) * 8)) >> (56 - (i * 8));
	}
}
