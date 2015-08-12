//  STREET VIEW ver0.1 :: HANDONG
//
//  Created by Seongho Hong / Seung Hee Ko / Mihyun Wendy Yang / on 2015. 7. 16..
//  Copyright (c) 2015년 Seongho Hong. All rights reserved.
//

#include "findPic.h"

int main(){

	PTS_INFO dirMap[MAX_PICTURE];
	int num_pic = 0;
	char directionMapFileName[MAX_TEXT] = "";
	

	pGraph g = newGraph(V);
	GNode n;
	makeDirectionMap(dirMap, num_pic, directionMapFileName, g); // 관계표 만들기

	//변수 초기화
	int index = 0;
	int indexNum = -1;
	int choice = 0;
	int user_compass = 0;
	int startIdx = -1, endIdx = -1;

	//인덱스 받고 첫화면
	while ((num_pic < indexNum) || (indexNum < 0) || (choice < 0)){
		system("cls");
		printGraph(g);
		showDirMap(dirMap, num_pic);
		cout << "사진을 보시려면 0, 관계표를 수정하려면 1, 최단경로를 구하고 싶으시면 2를 입력해 주세요." << endl;
		cout << "input : ";
		cin >> choice;

		
		if ((num_pic < indexNum) || (indexNum < -2)){
			cout << "Error : out of range !" << endl;
			cout << "You have to input 0 < input < " << num_pic - 1 << endl;
			return -1;
		}

		else if (choice == 1){
			modifyDirectionMap(dirMap, directionMapFileName, num_pic, g);   //관계표 수정
		}
		else if (choice == 2){
			cout << "출발점과 도착점의 인덱스 번호를 입력해주세요." << endl;
			cin >> startIdx;
			cin >> endIdx;
			shortestPath(startIdx, endIdx, g);
			}
		else {
			cout << "인덱스 번호를 0 ~ " << (num_pic - 1) << " 중에 입력해주세요" << endl;
			cin >> indexNum;

		}
	}

	Mat streetImage = imread(getFileName(dirMap[indexNum], 0)); //사진 불러오기 
	Mat revisedImage = slantCorrection(streetImage);

	MOUSECHANGE mouse;
	namedWindow(win_name, WINDOW_NORMAL);
	setMouseCallback(win_name, callBackFunc, &mouse); // 마우스 클릭 이벤트 설정
	drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass);// 화살표 생성
	mouse.user_dir = user_compass;
	imshow(win_name, revisedImage);
	resizeWindow(win_name, 640, 480);
	mouse.frameSize = revisedImage.size();

	while ((mouse.change == 0) || (dirMap[indexNum].direc[mouse.direction] == -1)){
		waitKey(20);
	}

	Mat temp = streetImage.clone();

	while (1){
		if ((mouse.change == 1) && (dirMap[indexNum].direc[mouse.direction] != -1)){      // 가고자 하는 방향에 사진이 있을 때만
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].direc[mouse.direction];   //indexNUm에 그 사진 index 넣어주기
			user_compass = mouse.direction;
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // 사진 불러오기
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // 화살표 생성
			mouse.user_dir = user_compass;
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();
		}
		else if ((mouse.change == 4) && (dirMap[indexNum].updown[0] != -1))  {
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].updown[0];   //indexNUm에 그 사진 index 넣어주기
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // 사진 불러오기
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // 화살표 생성
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();

		}
		else if ((mouse.change == 5) && (dirMap[indexNum].updown[1] != -1)){
			system("cls");
			showDirMap(dirMap, num_pic);
			indexNum = dirMap[indexNum].updown[1];   //indexNUm에 그 사진 index 넣어주기
			streetImage = imread(getFileName(dirMap[indexNum], user_compass), 1); // 사진 불러오기
			revisedImage = slantCorrection(streetImage);
			drawArrowAndGps(revisedImage, dirMap[indexNum], user_compass); // 화살표 생성
			imshow(win_name, revisedImage);
			mouse.frameSize = revisedImage.size();

		}

		revisedImage = slantCorrection(streetImage);
		slideImage(revisedImage, dirMap[indexNum], mouse, user_compass, indexNum); // 왼쪽 오른쪽 UI

		mouse.change = 0; // 0으로 바꿔주어 다시 마우스 클릭 됐을 때를 알 수 있도록
		waitKey(200);
	}

	//종료
	destroyWindow(win_name);
	return 0;
}

///////////////////////////////////
//////스트리트 뷰 화면 컨트롤//////
///////////////////////////////////

// jpg 파일 이름 받아오기
// compass에는 사용자가 바라보고 있는 방향 정보를 받아옴
// 기본값으로 동쪽으로 설정 0 
String getFileName(PTS_INFO map, int compass){
	// 0 북 1 서 2 남 3 동
	char buf[MAX_TEXT];
	sprintf(buf, imageFileName, map.id, compass);
	//sprintf(buf, "%d.jpg", fileIndex,compass);

	String dst = buf;
	cout << buf << endl;
	return dst;
}

// index들의 관계 보여주기
void showRelation(int(*map)[MAX_DIRECTION]){

	cout << "########### STREET VIEW ver1.0 ##########" << endl;
	cout << "#index\t북\t서\t남\t동\t#" << endl;

	for (int i = 0; i < MAX_PICTURE; i++){
		cout << "# " << i << '\t';
		for (int j = 0; j < MAX_DIRECTION; j++){
			cout << map[i][j] << '\t';
		}
		cout << "#" << endl;
	}
	cout << "#########################################" << endl;
}

// 좌표 받은 것의 관계 보여주기
void showDirMap(PTS_INFO* in_dstMap, int in_num_pic){
	cout << "############### STREET VIEW ver0.1 ##############" << endl;
	cout << "#index\t북\t서\t남\t동\t층\t#" << endl;

	for (int i = 0; i < in_num_pic; i++){
		cout << "# " << i << '\t';
		for (int j = 0; j < MAX_DIRECTION; j++){
			cout << in_dstMap[i].direc[j] << '\t';
		}
		cout << in_dstMap[i].floor << "\t";

		cout << "#" << endl;
	}
	cout << "#################################################" << endl;
}

// txt 읽기
void readText(String fileName, PTS_INFO* &io_dstMap){
	ifstream file(fileName);
	if (!file.is_open()){
		cout << "file is not exist" << endl;
		return;
	}
	char buffer[MAX_TEXT];
	char* token;
	int i = 0;
	while (!file.eof()){

		file.getline(buffer, 100);
		token = strtok(buffer, "\"id");

		for (int j = 0; token != NULL; j++){
			if (j == 0){
				io_dstMap[i].id = atoi(token);
				cout << token << endl;
			}
			else if (j == 2){ // 3번째
				io_dstMap[i].y = atof(token);
				cout << i << " : y 좌표 " << token << endl;
			}
			else if (j == 3){ // 4번째
				io_dstMap[i].x = atof(token);
				cout << i << " : x 좌표 " << token << endl;
			}
			else if (j == 4){ // 5번째
				io_dstMap[i].floor = atoi(token);
				cout << i << " : floor " << io_dstMap[i].floor << endl;
			}
			else if (j == 5) { //6번째
				io_dstMap[i].capturedCompass = atoi(token);
				cout << i << " : 촬영 될 때 나침반 각도" << token << endl;
			}

			if (j == 3){
				token = strtok(NULL, "\";AF");
			}
			else{
				token = strtok(NULL, "\";");
			}


		}
		i++;
		cout << endl;
	}

	file.close();
}

// 마우스 이벤트
void callBackFunc(int event, int x, int y, int flags, void* userdata){

	MOUSECHANGE* mouse = (MOUSECHANGE*)userdata;
	int rows = mouse->frameSize.height;
	int cols = mouse->frameSize.width;

	if (event == EVENT_LBUTTONDOWN){
		//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		// if 화살표 위를 클릭하면 동, 서, 남, 북
		//cout << "cols가로" << cols/2 << endl;
		//cout << "rows세로" << rows / 2 << endl;

		if (((x> cols / 2) && (x < cols / 2 + 100)) && (y>rows / 2 - 25) && (y < rows / 2 + 25)){
			mouse->direction = (mouse->user_dir + 3) % 4; //동
			mouse->change = 1;
		}

		else if (((x>cols / 2 - 100) && (x < cols / 2)) && (y>rows / 2 - 25) && (y < rows / 2 + 25)){
			mouse->direction = (mouse->user_dir + 1) % 4; //서
			mouse->change = 1;
		}

		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows / 2) && (y < rows / 2 + 50)){
			mouse->direction = (mouse->user_dir + 2) % 4; //남
			mouse->change = 1;
			//cout << "남" << endl;
		}
		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows / 2 - 50) && (y < rows / 2)){
			mouse->direction = (mouse->user_dir) % 4; //북
			mouse->change = 1;
			//cout << "북" << endl;
		}

		// 왼쪽 오른쪽 여백
		if ((x > 0) && (x < cols * 1 / 15) && (y>0) && (y < rows)){
			//cout << "hi" << endl;
			mouse->change = 2;
		}
		else if ((x> cols * 14 / 15) && (x < cols) && (y>0) && (y < rows)){
			//cout << "there" << endl;
			mouse->change = 3;
		}

		// 위층 아래층 이동
		if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>0) && (y < rows * 1 / 10)){
			mouse->change = 4;
			//cout << "up" << endl;
		}
		else if (((x>cols / 2 - 25) && (x < cols / 2 + 25)) && (y>rows * 9 / 10) && (y < rows)){
			mouse->change = 5;
			//cout << "down" << endl;
		}
	}
	else if (event == EVENT_RBUTTONDOWN){
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN){
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE){
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;


	}
}

//화살표 그려주기
void drawArrowAndGps(Mat img, PTS_INFO map, int & in_user_compass){

	// user compass에서 0번 카메라의 나침반을 가져왔다고 생각하면


	if (map.direc[in_user_compass % 4] != -1){ // 북
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2, img.rows / 2 - 50), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 1) % 4] != -1){ // 서
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2 - 100, img.rows / 2), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 2) % 4] != -1){ // 남
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2, img.rows / 2 + 50), Scalar(255, 255, 255), 5);
	}
	if (map.direc[(in_user_compass + 3) % 4] != -1){ // 동
		arrowedLine(img, Point(img.cols / 2, img.rows / 2), Point(img.cols / 2 + 100, img.rows / 2), Scalar(255, 255, 255), 5);
	}

	if (map.updown[0] != -1){ // 위
		arrowedLine(img, Point(img.cols / 2, img.rows * 1 / 10), Point(img.cols / 2, 0), Scalar(0, 255, 255), 5);
		//putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);

	}
	if (map.updown[1] != -1){ // 아래
		arrowedLine(img, Point(img.cols / 2, img.rows * 9 / 10), Point(img.cols / 2, img.rows), Scalar(0, 255, 255), 5);
		//putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);

	}

	char xText[MAX_TEXT];
	char yText[MAX_TEXT];
	char floorText[MAX_TEXT];

	sprintf(xText, "X :%f", map.x);
	sprintf(yText, "Y :%f", map.y);
	sprintf(floorText, "Floor : %d", map.floor);

	putText(img, floorText, Point(img.cols * 4 / 5, img.rows * 7 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
	putText(img, xText, Point(img.cols * 4 / 5, img.rows * 8 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
	putText(img, yText, Point(img.cols * 4 / 5, img.rows * 9 / 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2);
}


// 좌우 화면 전환
// 마우스 이벤트, 사용자의 시점을 고려
// 이미지, 관계정보, 마우스 처리, 사용자 시점, 인덱스
void slideImage(Mat & io_streetImage, PTS_INFO map, MOUSECHANGE & io_mouse, int & io_user_compass, int in_indexNum){

	Mat revisedImage;
	io_streetImage.copyTo(revisedImage);

	if (io_mouse.change == 2)
	{
		if (io_user_compass == E){
			io_user_compass = N;
			io_streetImage = imread(getFileName(map, N), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);
		}
		else if (io_user_compass == N){
			io_user_compass = W;
			io_streetImage = imread(getFileName(map, W), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == W){
			io_user_compass = S;
			io_streetImage = imread(getFileName(map, S), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == S){
			io_user_compass = E;
			io_streetImage = imread(getFileName(map, E), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);
		}
		io_mouse.user_dir = io_user_compass;
		io_mouse.frameSize = io_streetImage.size();
	}


	else if (io_mouse.change == 3)
	{
		if (io_user_compass == E){
			io_user_compass = S;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == S){
			io_user_compass = W;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);
		}
		else if (io_user_compass == W){
			io_user_compass = N;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);

		}
		else if (io_user_compass == N){
			io_user_compass = E;
			io_streetImage = imread(getFileName(map, io_user_compass), 1);
			revisedImage = slantCorrection(io_streetImage);
			drawArrowAndGps(revisedImage, map, io_user_compass); // 화살표 생성
			imshow(win_name, revisedImage);

		}
		io_mouse.user_dir = io_user_compass;

		io_mouse.frameSize = io_streetImage.size();

	}
}



/////////////////////////////////////
//////좌표로 부터 관계표 만들기//////
/////////////////////////////////////

//두 점의 거리 계산
double F_dist(double x1, double y1, double x2, double y2){
	return sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));
}

//두 점의 각도 계산
double F_ang(double x1, double y1, double x2, double y2){
	return (atan2((double)(y2 - y1), (double)(x2 - x1))*180.0f / 3.14159265);
}

//관계표 만들기 (관계도, 점의 개수, 관계도 txt 파일명)
void makeDirectionMap(PTS_INFO* io_dstMap, int &out_num_pic, char * io_directionMapFileName, pGraph g) {

	int i = 0, n = 0, j = 0;
	int fileExist;
	//char directionMapFileName[MAX_TEXT] = ""; // char 초기화
	cout << "관계도를 저장할 텍스트 파일명을 입력해 주세요 (ex.OH123.txt) : ";
	cin >> io_directionMapFileName; // 입력받음
	fileExist = isDirectionMapExist(io_directionMapFileName); // 파일 존재 여부 확인

	cout << endl << "점의 갯수를 입력하시오 \n" << endl;
	cin >> n;
	out_num_pic = n;

	if (fileExist == 0){
		double ang_result[MAX_PICTURE][MAX_PICTURE], dis_result[MAX_PICTURE][MAX_PICTURE];

		// 2차원 배열을 사용. 초기화
		for (i = 0; i < n; i++){
			io_dstMap[i].x = io_dstMap[i].y = 0.0; // x,y 좌표 초기화
			strcpy(io_dstMap[i].tagging, ""); // 태깅정보 초기화

			for (j = 0; j < 4; j++){
				io_dstMap[i].length[j] = MAX_DIST;
				io_dstMap[i].direc[j] = -1;
			}

			for (j = 0; j < 2; j++){
				io_dstMap[i].updown_length[j] = MAX_DIST;
				io_dstMap[i].updown[j] = -1;
			}


			for (j = 0; j < n; j++)
				ang_result[i][j] = dis_result[i][j] = 0.0;
		}

		// 텍스트 파일에 있는 점의 좌표를 불러옴
		readText(txtFileName, io_dstMap);

		for (i = 0; i < n; i++){
			for (j = 0; j < n; j++){
				if (i == j || io_dstMap[i].floor != io_dstMap[j].floor) continue;

				ang_result[i][j] = F_ang(io_dstMap[i].x, io_dstMap[i].y, io_dstMap[j].x, io_dstMap[j].y);
				dis_result[i][j] = F_dist(io_dstMap[i].x, io_dstMap[i].y, io_dstMap[j].x, io_dstMap[j].y);

				//printf("i:%d j:%d %f %f ang %f\n", i, j, io_dstMap[i].x, io_dstMap[i].y, ang_result[i][j]);

				if (-45 <= ang_result[i][j] && ang_result[i][j] < 45){
					//동쪽 0과 1. 0과 3
					if ((io_dstMap[i].length[3] >= dis_result[i][j])){
						io_dstMap[i].direc[3] = j;
						//io_dstMap[j].direc[1] = i;
						io_dstMap[i].length[3] = dis_result[i][j];
					}
				}

				else if (45 <= ang_result[i][j] && ang_result[i][j] < 135){
					//북쪽
					if (io_dstMap[i].length[0] >= dis_result[i][j]){
						io_dstMap[i].direc[0] = j;
						//io_dstMap[j].direc[2] = i;
						io_dstMap[i].length[0] = dis_result[i][j];
						//printf("%d에서 %d까지 거리 : %f \n",i,j, io_dstMap[i].length[0]);
					}
				}

				else if (135 <= ang_result[i][j] || ang_result[i][j] < -135){
					//서쪽
					if (io_dstMap[i].length[1] >= dis_result[i][j]){
						io_dstMap[i].direc[1] = j;
						//io_dstMap[j].direc[3] = i;
						io_dstMap[i].length[1] = dis_result[i][j];
					}
				}

				else if (-135 <= ang_result[i][j] && ang_result[i][j] < -45){
					//남쪽
					if (io_dstMap[i].length[2] >= dis_result[i][j]){
						io_dstMap[i].direc[2] = j;
						//io_dstMap[j].direc[0] = i;
						io_dstMap[i].length[2] = dis_result[i][j];
					}
				}
			}

			for (int k = 0; k < 4; k++){
				if (io_dstMap[i].length[k] > 0.0001) io_dstMap[i].direc[k] = -1;
			}

		}



		makeTxtFile(io_dstMap, io_directionMapFileName, out_num_pic, g);//텍스트 파일로 저장
	}
	else{
		readTextDirectionMap(io_directionMapFileName, io_dstMap, g);
	}
}
//관계표 텍스트 파일로 만들기 ( 관계도, 관계도 txt 파일명 )
void makeTxtFile(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g){

	//ofstream outTxt(in_directionMapFileName);
	FILE *outTxt;
	outTxt = fopen(in_directionMapFileName, "w");
	// id / 북서남동 / 거리 / x, y / 층 / 태깅정보

	
	for (int i = 0; i<in_pts_num; i++){
		fprintf(outTxt, "%d\t", i);
		fprintf(outTxt, "%d\t%d\t%d\t%d\t", io_dstMap[i].direc[0], io_dstMap[i].direc[1], io_dstMap[i].direc[2], io_dstMap[i].direc[3]);  // 북서남동
		fprintf(outTxt, "%d\t%d\t", io_dstMap[i].updown[0], io_dstMap[i].updown[1]);  // 위 아래
		fprintf(outTxt, "%f\t%f\t%f\t%f\t", io_dstMap[i].length[0], io_dstMap[i].length[1], io_dstMap[i].length[2], io_dstMap[i].length[3]); // 북서남동 길이
		fprintf(outTxt, "%f\t%f\t", io_dstMap[i].updown_length[0], io_dstMap[i].updown_length[1]); // 위 아래 길이
		fprintf(outTxt, "%f\t%f\t", io_dstMap[i].x, io_dstMap[i].y); // x,y 좌표
		fprintf(outTxt, "%d\t", io_dstMap[i].floor); // 층
		addEdgeFromTo(g, i, io_dstMap[i].direc[0], io_dstMap[i].length[0]);
		addEdgeFromTo(g, i, io_dstMap[i].direc[1], io_dstMap[i].length[1]);
		addEdgeFromTo(g, i, io_dstMap[i].direc[2], io_dstMap[i].length[2]);
		addEdgeFromTo(g, i, io_dstMap[i].direc[3], io_dstMap[i].length[3]);
		if (io_dstMap[i].tagging == NULL)
			fprintf(outTxt, "%s\t", io_dstMap[i].tagging); // 태깅정보
		fprintf(outTxt, "\n");
	}

	//outTxt.close();
	fclose(outTxt);

	cout << txtFileName << " 성공적으로 저장되었습니다 :D" << endl;
	cout << "( 각 요소들은 tab으로 구분되어 있음 )" << endl << endl;
}

//관계표 수정 ( 관계도, 관계도 txt 파일명 )
void modifyDirectionMap(PTS_INFO* io_dstMap, char * in_directionMapFileName, int in_pts_num, pGraph g){
	char choice = 'y';
	while (choice == 'y'){
		int in_index = -1, in_direction = -1, in_pointIndex = -2;

		cout << "수정할 인덱스, 방향, 가리킬 인덱스를 입력해주세요 : " << endl;
		cin >> in_index >> in_direction >> in_pointIndex;

		if (in_pointIndex == -1){
			io_dstMap[in_index].direc[in_direction] = in_pointIndex;
			io_dstMap[in_index].length[in_direction] = 10000000.;

			system("cls");
			showDirMap(io_dstMap, in_pts_num);

			cout << "index " << in_index << "의 " << in_direction << "방향이 " << in_pointIndex << "로 설정되었습니다" << endl;

		}
		else if (in_direction <= 3 && in_direction >= 0){
			io_dstMap[in_index].direc[in_direction] = in_pointIndex;
			io_dstMap[in_pointIndex].direc[(in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION] = in_index; // 대응하는지점의 반대방향

			io_dstMap[in_index].length[in_direction] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
			io_dstMap[in_pointIndex].length[(in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
		
			system("cls");
			showDirMap(io_dstMap, in_pts_num);

			cout << "index " << in_index << "의 " << in_direction << "방향이 " << in_pointIndex << "로 설정되었습니다" << endl;
			cout << "index " << in_pointIndex << "의 " << (in_direction + MAX_DIRECTION / 2) % MAX_DIRECTION << "방향이 " << in_index << "로 설정되었습니다" << endl;

		}
		else if (in_direction == 4){
			if (in_index == 11 || in_index == 13){
				io_dstMap[in_index].updown[0] = in_pointIndex;
				io_dstMap[in_index].updown_length[0] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
				
				system("cls");
				showDirMap(io_dstMap, in_pts_num);

				cout << "index " << in_index << "의 " << "위쪽 방향이 " << in_pointIndex << "로 설정되었습니다" << endl;

				}
			else{
				io_dstMap[in_index].updown[0] = in_pointIndex;
				io_dstMap[in_pointIndex].updown[1] = in_index; // 대응하는지점의 반대방향

				io_dstMap[in_index].updown_length[0] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
				io_dstMap[in_pointIndex].updown_length[1] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);

				system("cls");
				showDirMap(io_dstMap, in_pts_num);

				cout << "index " << in_index << "의 " << "위쪽 방향이 " << in_pointIndex << "로 설정되었습니다" << endl;
				cout << "index " << in_pointIndex << "의 " << "아래쪽 방향이 " << in_index << "로 설정되었습니다" << endl;

			}

		}
		else if (in_direction == 5){
			if (in_index == 46){
				io_dstMap[in_index].updown[1] = in_pointIndex;
				io_dstMap[in_index].updown_length[1] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
				system("cls");
				showDirMap(io_dstMap, in_pts_num);

				cout << "index " << in_index << "의 " << "아래쪽 방향이 " << in_pointIndex << "로 설정되었습니다" << endl;

			}
			else{
				io_dstMap[in_index].updown[1] = in_pointIndex;
				io_dstMap[in_pointIndex].updown[0] = in_index; // 대응하는지점의 반대방향

				io_dstMap[in_index].updown_length[1] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);
				io_dstMap[in_pointIndex].updown_length[0] = F_dist(io_dstMap[in_index].x, io_dstMap[in_index].y, io_dstMap[in_pointIndex].x, io_dstMap[in_pointIndex].y);

				system("cls");
				showDirMap(io_dstMap, in_pts_num);

				cout << "index " << in_index << "의 " << "아래쪽 방향이 " << in_pointIndex << "로 설정되었습니다" << endl;
				cout << "index " << in_pointIndex << "의 " << "위쪽 방향이 " << in_index << "로 설정되었습니다" << endl;
			}
		}


		
		
		cout << "또 수정하시겠습니까? (y/n) : " << endl;
		cin >> choice;

	}

	cout << in_directionMapFileName << "에 저장하시겠습니까? (y/n) : " << endl;
	cin >> choice;
	if (choice = 'y'){
		makeTxtFile(io_dstMap, in_directionMapFileName, in_pts_num, g);
	}
	cout << "관계도 수정 완료!" << endl;
}

// 유저가 입력한 건물의 관계도(지도)가 존재하는지 확인
int isDirectionMapExist(char* file)
{
	strcat(strPath, file);
	int nResult = access(strPath, 0);
	if (nResult == 0)
	{
		printf("해당 건물의 지도(관계도)가 존재합니다.");
		return 1;
	}
	else if (nResult == -1)
	{
		printf("해당 건물의 지도(관계도)가 존재하지 않습니다.");
		return 0;
	}
}

// 관계표 txt 읽어오기
void readTextDirectionMap(char * in_directionMapFileName, PTS_INFO* &io_dstMap, pGraph g){
	ifstream file(in_directionMapFileName);

	char buffer[MAX_TEXT];
	char* token;
	int i = 0;
	while (!file.eof()){

		file.getline(buffer, 250);
		token = strtok(buffer, "\t");

		for (int j = 0; token != NULL; j++){
			if (j == 0){ // id
				io_dstMap[i].id = atoi(token);
			}
			else if (j == 1){ // 북
				io_dstMap[i].direc[0] = atoi(token);
			}
			else if (j == 2){ // 서
				io_dstMap[i].direc[1] = atoi(token);
			}
			else if (j == 3){ // 남
				io_dstMap[i].direc[2] = atoi(token);
			}
			else if (j == 4){ // 동
				io_dstMap[i].direc[3] = atoi(token);
			}

			else if (j == 5){ // 위
				io_dstMap[i].updown[0] = atoi(token);
			}
			else if (j == 6){ // 아래
				io_dstMap[i].updown[1] = atoi(token);
			}

			else if (j == 7){ // 북 길이
				io_dstMap[i].length[0] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].direc[0], io_dstMap[i].length[0]);
				//addEdge(g, i, io_dstMap[i].direc[0], io_dstMap[i].length[0]);
			}
			else if (j == 8){ // 서 길이
				io_dstMap[i].length[1] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].direc[1], io_dstMap[i].length[1]);
				//addEdge(g, i, io_dstMap[i].direc[1], io_dstMap[i].length[1]);
			}
			else if (j == 9){ // 남 길이
				io_dstMap[i].length[2] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].direc[2], io_dstMap[i].length[2]);
				//addEdge(g, i, io_dstMap[i].direc[2], io_dstMap[i].length[2]);
			}
			else if (j == 10){ // 동 길이
				io_dstMap[i].length[3] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].direc[3], io_dstMap[i].length[3]);
				//addEdge(g, i, io_dstMap[i].direc[3], io_dstMap[i].length[3]);
			}

			else if (j == 11){ // 위 길이
				io_dstMap[i].updown_length[0] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].updown[0], io_dstMap[i].updown_length[0]);
			}
			else if (j == 12){ // 아래 길이
				io_dstMap[i].updown_length[1] = atof(token);
				addEdgeFromTo(g, i, io_dstMap[i].updown[1], io_dstMap[i].updown_length[1]);
			}

			else if (j == 13){ // x 좌표
				io_dstMap[i].x = atof(token);
			}
			else if (j == 14){ // y 좌표
				io_dstMap[i].y = atof(token);
			}
			else if (j == 15){ // 층
				io_dstMap[i].floor = atoi(token);
			}
			else if (j == 16){ // 층
				strcpy(io_dstMap[i].tagging, token);
			}
			token = strtok(NULL, "\t");
		}

		i++;
		cout << endl;
	}

	file.close();
}

void hough_transform(Mat& im, Mat& orig, Mat gray, double* skew)
{
	double max_r = sqrt(pow(.5*im.cols, 2) + pow(.5*im.rows, 2));
	int angleBins = 180;
	Mat acc = Mat::zeros(Size(2 * max_r, angleBins), CV_32SC1);
	int cenx = im.cols / 2;
	int ceny = im.rows / 2;
	Mat vert, hori;
	orig.copyTo(vert);
	orig.copyTo(hori);

	*skew = 0.;

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{
			if (im.at<uchar>(y, x) == 255)		// edge 검출
			{
				for (int t = 0; t < angleBins; t++)
				{
					double r = (x - cenx)*cos((double)t / angleBins*CV_PI) + (y - ceny)*sin((double)t / angleBins*CV_PI);
					r += max_r;
					acc.at<int>(t, int(r))++;
				}
			}
		}
	}
	Mat thresh;
	normalize(acc, acc, 255, 0, NORM_MINMAX);
	convertScaleAbs(acc, acc);

	Point maxLoc, minLoc;
	minMaxLoc(acc, 0, 0, &minLoc, &maxLoc);	// 가장 뚜렷한 직선값

	double theta = (double)maxLoc.y / angleBins*CV_PI;	// 영상 센터 좌표의 y축을 기준으로 한 각도

	double rho = -maxLoc.x + max_r;		// 원점~영상 센터 좌표까지의 거리

	Mat revisedImg, w_thres, b_thres, add, canny, rgb_canny, dst;
	vector<Vec4i> lines;
	Point center;

	double vertGrad[arraySize] = { 0.0 }, horiGrad[arraySize] = { 0.0 };
	double vertFrag[arraySize] = { 0.0 }, horiFrag[arraySize] = { 0.0 };
	int vertCount = 0, horiCount = 0;
	double vertGAVG = 0., vertFAVG = 0., horiGAVG = 0., horiFAVG = 0.;
	double vertSkewangle = 0., horiSkewangle = 0.;


	// 선 굵기가 얇아서 짧게 나온다.
	threshold(gray, w_thres, 120, 255, 0);
	inRange(gray, Scalar::all(0), Scalar::all(20), b_thres);

	add = b_thres + w_thres;
	Canny(add, canny, 50, 150, 3);

	cvtColor(canny, rgb_canny, CV_GRAY2BGR);

	HoughLinesP(canny, lines, 1, CV_PI / 180, 30, 10, 30);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4d I = lines[i];
		center = Point(I[2], I[3]) - Point(I[0], I[1]);
		double rad = atan2(center.x, center.y);
		double degree = (rad * 180) / CV_PI;
		double a, b;

		if (degree >0 && degree < 10){	// 수직(y축)에 가까운 선 검출
			
			b = I[1] - I[0] * degree;
			vertGrad[vertCount] = degree;
			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree>170 && degree < 180){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree >-10 && degree < 0) vertGrad[vertCount] = degree;
			
			vertFrag[vertCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree > 80 && degree < 90) // 수평 (x축)에 가까운 선 검출
		{
			
			b = I[1] - I[0] * degree;
			horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
		else if (degree >90 && degree < 110){
			degree = degree - 180;
			b = I[1] - I[0] * degree;
			if (degree > -90 && degree < -80) horiGrad[horiCount] = degree;
			horiFrag[horiCount++] = b;
			line(rgb_canny, Point(I[0], I[1]), Point(I[2], I[3]), Scalar(0, 0, 255), 1);

		}
	}


	for (int j = 0; j < vertCount; j++)
	{
		vertGAVG += vertGrad[j];
		vertFAVG += vertFrag[j];
	}
	for (int j = 0; j < horiCount; j++)
	{
		horiGAVG += horiGrad[j];
		horiFAVG += horiFrag[j];
	}
	if (vertCount != 0.){
		vertGAVG /= vertCount;
		vertFAVG /= vertCount;

	}
	if (horiCount != 0.){
		horiGAVG /= horiCount;
		horiFAVG /= horiCount;
	}

	if (abs(sin(theta)) < 0.000001)//check vertical    <0.000001
	{
		cout << abs(sin(theta)) << endl;
		cout << abs(cos(theta)) << endl;
		double m = -cos(theta) / sin(theta);
		double b = rho / sin(theta) + im.rows / 2. - m*im.cols / 2.;
		Point2d p1 = Point2d(rho + im.cols / 2, 0);
		Point2d p2 = Point2d(rho + im.cols / 2, im.rows);
		line(orig, p1, p2, Scalar(0, 0, 255), 1);
		double skewangle;
		skewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);
		*skew = skewangle;
		
	}
	

	if ((vertGAVG > -10 && vertGAVG < 0) || (vertGAVG> 0 && vertGAVG < 10)){

		Point2d p1 = Point2d(0, vertFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*vertGAVG + vertFAVG);
		line(vert, p1, p2, Scalar(0, 0, 255), 1);

		vertSkewangle = p1.y - p2.y>0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);	// 세로선
		*skew = -vertGAVG;
	}

	else{
		*skew = 0;
	}

	if (((horiGAVG > 40 && horiGAVG < 90) || (horiGAVG >-90 && horiGAVG < -40)) || ((horiGAVG > 0 && horiGAVG < 20) || (horiGAVG >-20 && horiGAVG < 0))) // 수평 (x축)에 가까운 선 검출
	{

		if (horiGAVG > 0) horiGAVG = 90 - horiGAVG;
		else horiGAVG = 90 + horiGAVG;
		Point2d p1 = Point2d(0, horiFAVG);
		Point2d p2 = Point2d(im.cols, im.cols*horiGAVG + horiFAVG);
		line(hori, p1, p2, Scalar(0, 0, 255), 1);
		horiSkewangle = p1.x - p2.x > 0 ? (atan2(p1.y - p2.y, p1.x - p2.x)*180. / CV_PI) : (atan2(p2.y - p1.y, p2.x - p1.x)*180. / CV_PI);	// 세로선
		
		horiSkewangle = 90 - horiSkewangle;
		horiSkewangle = abs(horiSkewangle) < horiGAVG ? horiSkewangle : horiGAVG;
		*skew = horiSkewangle;
		

	}
	else{
		*skew = 0;
	}
	



	if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <10. && horiSkewangle >2.5)) {
		if (vertGAVG > -2. && vertGAVG < 2.)
			*skew = -vertGAVG;
		else if (abs(vertGAVG - horiSkewangle) > 2.0)
			*skew = abs(vertGAVG) < abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
		else
			*skew = abs(vertGAVG) > abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	}
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle <2.5 && horiSkewangle >0.)){
		if (abs(vertGAVG) > abs(vertSkewangle))
			*skew = *skew = abs(vertSkewangle) < abs(horiSkewangle) ? -vertSkewangle : horiSkewangle;
		else
			*skew = abs(vertGAVG) < abs(horiSkewangle) ? -vertGAVG : horiSkewangle;
	}
	else if (horiSkewangle != 0. && vertGAVG != 0. && (horiSkewangle > 10.)) *skew = abs(vertGAVG) > abs(horiGAVG) ? -vertGAVG : horiSkewangle;
	else if (horiSkewangle == 0)	*skew = -vertGAVG;
	else if (vertGAVG == 0) *skew = horiSkewangle;
	else if (horiSkewangle == 0 && vertGAVG == 0)	*skew = 0.;

	if ((vertSkewangle < 3. && vertSkewangle >0.) && (horiSkewangle <3. && horiSkewangle >0.))
		*skew = vertSkewangle > horiSkewangle ? -vertSkewangle : horiSkewangle;

	if (vertGAVG > 3.5) *skew = 0.;
	if (vertGAVG != 0 && horiSkewangle != 0 && abs(vertGAVG - horiSkewangle) < 1.3 && abs(vertGAVG - horiSkewangle) > 1.0){
		if (abs(vertGAVG) < abs(horiSkewangle))	* skew = horiSkewangle;
		else *skew = vertGAVG;
	}
	if (vertGAVG != 0 && horiSkewangle != 0 && abs(vertGAVG - horiSkewangle) < 1.0)	*skew = abs(vertGAVG - horiSkewangle);
	
}

Mat preprocess1(Mat& im)
{
	Mat ret = Mat::zeros(im.size(), CV_32SC1);

	for (int x = 1; x < im.cols - 1; x++)
	{
		for (int y = 1; y < im.rows - 1; y++)
		{

			int gy = (im.at<uchar>(y - 1, x + 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y, x + 1) - im.at<uchar>(y, x - 1))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y + 1, x - 1));
			int gx = (im.at<uchar>(y + 1, x - 1) - im.at<uchar>(y - 1, x - 1))
				+ 2 * (im.at<uchar>(y + 1, x) - im.at<uchar>(y - 1, x))
				+ (im.at<uchar>(y + 1, x + 1) - im.at<uchar>(y - 1, x + 1));
			int g2 = (gy*gy + gx*gx);
			ret.at<int>(y, x) = g2;
		}
	}
	normalize(ret, ret, 255, 0, NORM_MINMAX);
	ret.convertTo(ret, CV_8UC1);
	threshold(ret, ret, 50, 255, THRESH_BINARY);
	return ret;
}

Mat preprocess2(Mat& im)
{
	// 1) assume white on black and does local thresholding
	// 2) only allow voting top is white and buttom is black(buttom text line)
	Mat thresh;
	//thresh=255-im;
	thresh = im.clone();
	adaptiveThreshold(thresh, thresh, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, -2);
	Mat ret = Mat::zeros(im.size(), CV_8UC1);
	for (int x = 1; x < thresh.cols - 1; x++)
	{
		for (int y = 1; y < thresh.rows - 1; y++)
		{
			bool toprowblack = thresh.at<uchar>(y - 1, x) == 0 || thresh.at<uchar>(y - 1, x - 1) == 0 || thresh.at<uchar>(y - 1, x + 1) == 0;
			bool belowrowblack = thresh.at<uchar>(y + 1, x) == 0 || thresh.at<uchar>(y + 1, x - 1) == 0 || thresh.at<uchar>(y + 1, x + 1) == 0;

			uchar pix = thresh.at<uchar>(y, x);
			if ((!toprowblack && pix == 255 && belowrowblack))
			{
				ret.at<uchar>(y, x) = 255;
			}
		}
	}
	return ret;
}
Mat rot(Mat& im, double thetaRad)
{
	cv::Mat rotated;
	double rskew = thetaRad* CV_PI / 180;
	double nw = abs(sin(thetaRad))*im.rows + abs(cos(thetaRad))*im.cols;
	double nh = abs(cos(thetaRad))*im.rows + abs(sin(thetaRad))*im.cols;
	cv::Mat rot_mat = cv::getRotationMatrix2D(Point2d(nw*.5, nh*.5), thetaRad * 180 / CV_PI, 1);
	Mat pos = Mat::zeros(Size(1, 3), CV_64FC1);
	pos.at<double>(0) = (nw - im.cols)*.5;
	pos.at<double>(1) = (nh - im.rows)*.5;
	Mat res = rot_mat*pos;
	rot_mat.at<double>(0, 2) += res.at<double>(0);
	rot_mat.at<double>(1, 2) += res.at<double>(1);
	cv::warpAffine(im, rotated, rot_mat, Size(nw, nh), cv::INTER_LANCZOS4);
	return rotated;
}

Mat slantCorrection(Mat& src)
{

	Mat im;
	src.copyTo(im);
	Mat gray;
	cvtColor(im, gray, CV_BGR2GRAY);

	Mat preprocessed = preprocess2(gray);
	double skew;
	hough_transform(preprocessed, im, gray, &skew);
	Mat rotated = rot(im, skew* CV_PI / 180);

	return rotated;
}

void shortestPath(int startIdx, int endIdx, pGraph g){
	
	cout << startIdx << "부터 " << endIdx << "까지의 거리를 계산하겠습니다. " << endl;
	pGraph h = newGraph(V);
	pq p = newPQ(V);
	pq S = newPQ(V);
	double distance = 0.;

	for (int i = 0; i < g->V; i++)
		insert(p, i);

	distance = dijkstra(g, p, S, endIdx, startIdx, path);
	cout << startIdx << "부터 " << endIdx << "까지의 거리는 " << distance << " 입니다" << endl;
	
	system("pause");
}