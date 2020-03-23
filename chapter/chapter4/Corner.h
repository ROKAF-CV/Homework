#pragma once
#include <algorithm>
#include "../chapter3/Edge.h"

#define NUM_MAX 1e9
class Corner :public Edge{
public:
	Corner();
	queue<pair<int, int>> Moravec(const Mat &img, double thresh);

	Mat Harris(const Mat &img);
	queue<pair<int, int>> localization(const Mat &img, double thresh);
private:
	int mask[3][3] = { {1,1,1},
						{1,1,1},
						{1,1,1} };
	float gaussian[3][3] = { {0.0751,0.1238,0.0751},{0.1238,0.2042,0.1238},{0.0751,0.1238,0.0751} };
	int dx[4] = { 0,0,1,-1 }; //하, 상,좌,우
	int dy[4] = { 1,-1,0,0 };
	int nx[3] = { 0,1,-1 };
	int ny[3] = { 0,1,-1 };

	//Moravec
	//특징 가능성 값 찾기
	//cx,cy 중심좌표 
	//x,y S(x,y)
	int feature(const Mat &img, int x, int y, int cx, int cy);

	bool isRange(int y, int x);
};