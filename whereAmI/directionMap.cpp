//  directionMap
//
//  Created by Mihyun Wendy Yang on 2015. 7. 16..
//  Copyright (c) 2015년 Mihyun Wendy Yang. All rights reserved.
//

// 지금은 일단 복사해서 하나로 합치는데
// 클래스 개념을 사용해서 둘을 분리하면 더 깔끔할 것 같다

#include <stdio.h>
#include <math.h>

#define MAX_DIST 10000000000
#define MAX_DOT 100


double F_dist(double x1, double y1, double x2, double y2);
double F_ang(double x1, double y1, double x2, double y2);



//입력받은 점의 인덱스와 좌표값

struct pts_info{
	int index[4];
	double direc[4]; //0=동, 1=서, 2=남, 3=북
	double x, y;
};


int main(int argc, const char * argv[]) {
	
	int i = 0, n = 0, j = 0;
	printf("점의 갯수를 입력하시오 \n");
	scanf("%d", &n);
	
	double ang_result[MAX_DOT][MAX_DOT], dis_result[MAX_DOT][MAX_DOT];
	struct pts_info pts[MAX_DOT];
	
	for (i = 0; i<n; i++){

		pts[i].x = pts[i].y = 0.0;

		for (j = 0; j<4; j++){
			pts[i].direc[j] = MAX_DIST;
			pts[i].index[j] = -1;
		}

		for (j = 0; j<n; j++)
			ang_result[i][j] = dis_result[i][j] = 0.0;

	}



	//점의 좌표 입력받기
	for (i = 0; i<n; i++){

		printf("index %d번째의 x값과 y값을 입력하세요\n", i);
		scanf("%lf", &pts[i].x);
		scanf("%lf", &pts[i].y);

	}
	
	for (i = 0; i<n; i++){
		for (j = 0; j<n; j++){
			if (i == j) continue;

			ang_result[i][j] = F_ang(pts[i].x, pts[i].y, pts[j].x, pts[j].y);
			dis_result[i][j] = F_dist(pts[i].x, pts[i].y, pts[j].x, pts[j].y);

			printf("i:%d j:%d %f %f ang %f\n", i, j, pts[i].x, pts[i].y, ang_result[i][j]);
			
			if (-45 <= ang_result[i][j] && ang_result[i][j]<45){
				//동쪽 0과 1. 0과 3
				if (pts[i].direc[0] >= dis_result[i][j]){
					pts[i].index[0] = j;
					pts[i].direc[0] = dis_result[i][j];
				}
			}

			else if (45 <= ang_result[i][j] && ang_result[i][j]<135){
				//북쪽
				if (pts[i].direc[3] >= dis_result[i][j]){
					pts[i].index[3] = j;
					pts[i].direc[3] = dis_result[i][j];
				}
			}

			else if (135 <= ang_result[i][j] || ang_result[i][j]<-135){
				//서쪽
				if (pts[i].direc[1] >= dis_result[i][j]){
					pts[i].index[1] = j;
					pts[i].direc[1] = dis_result[i][j];
				}
			}

			else if (-135 <= ang_result[i][j] && ang_result[i][j]<-45){
				//남쪽
				if (pts[i].direc[2] >= dis_result[i][j]){
					pts[i].index[2] = j;
					pts[i].direc[2] = dis_result[i][j];
				}

			}

		}
	}

	for (i = 0; i<n; i++){
			printf("점 %d의 동쪽에는 점: %d, 서쪽에는 점: %d, 남쪽에는 점: %d, 북쪽에는점: %d가 있습니다 \n",
			i, pts[i].index[0], pts[i].index[1], pts[i].index[2], pts[i].index[3]);
	}

	return 0;

}



//두 점의 거리 계산

double F_dist(double x1, double y1, double x2, double y2){

	return sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));

}


//두 점의 각도 계산

/*double F_ang(double x1,double y1, double x2, double y2)

{

double dx,dy,da;



dx=x2-x1;

dy=y2-y1;



if (!dx) dx=1e-6;



da=atan(dy/dx);



if (dx<0) da+=3.14159265;



da= da*180/3.14159265;

return da;

}*/


double F_ang(double x1, double y1, double x2, double y2){

	return (atan2((double)(y2 - y1), (double)(x2 - x1))*180.0f / 3.14159265);

}