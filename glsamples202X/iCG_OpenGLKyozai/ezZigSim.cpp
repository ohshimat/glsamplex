// RecivZIGSIM.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

//★受信するPCのアドレスをZIGSIMで設定すること
#include "calc.h"

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define _USE_MATH_DEFINES
#include <math.h>

#include "ezZigSim.h"

//--------◆ソケット
static SOCKET sock;

//--------◆スレッド
static CRITICAL_SECTION cs;
static HANDLE hThread;

//>>>>>>>>★
static ezZigSim* zigsim = ezZigSim::get();

//#define DEBUG

static float RadToDeg(float rad)
{
	return rad * (180.0f / M_PI);
}

static float NormalizeYaw(float* yaw)
{
	while (*yaw > 180.0f)  *yaw -= 360.0f;
	while (*yaw < -180.0f) *yaw += 360.0f;
	return *yaw;
}

static void QtToEuler(quat_t* qt, euler_t* el)
{
	// 正規化（必要に応じて）
	float norm = sqrtf(qt->w * qt->w + qt->x * qt->x + qt->y * qt->y + qt->z * qt->z);
	qt->w /= norm;
	qt->x /= norm;
	qt->y /= norm;
	qt->z /= norm;

	// Roll (X軸回転)画面右側方向＋
	float sinr_cosp = 2.0f * (qt->w * qt->x + qt->y * qt->z);
	float cosr_cosp = 1.0f - 2.0f * (qt->x * qt->x + qt->y * qt->y);
	el->roll = atan2f(sinr_cosp, cosr_cosp);

	// Pitch (Y軸回転)画面上側方向＋
	float sinp = 2.0f * (qt->w * qt->y - qt->z * qt->x);
	if (fabsf(sinp) >= 1)
		el->pitch = copysignf(M_PI / 2.0f, sinp); // クランプ
	else
		el->pitch = asinf(sinp);

	// Yaw (Z軸回転)画面に垂直上方向＋
	float siny_cosp = 2.0f * (qt->w * qt->z + qt->x * qt->y);
	float cosy_cosp = 1.0f - 2.0f * (qt->y * qt->y + qt->z * qt->z);
	el->yaw = atan2f(siny_cosp, cosy_cosp);

	// 度に変換（OpenGLで使いやすくするため）
	el->yaw =    RadToDeg(el->yaw);
	el->pitch = - RadToDeg(el->pitch);
	el->roll =  - RadToDeg(el->roll);
	// このコードは Z（Yaw）→Y（Pitch）→X（Roll） の順で回転を適用する、
	// OpenGLでよく使われる形式に対応しています。
	// atan2f や asinf を使って、ジンバルロックをある程度回避しています。
	// クォータニオンは正規化してから使うのが安全です。
}
// データを抽出する簡易関数（文字列操作のみ）
//-------------------------------------------------------------------------
static void extractData(const char* json, const char* item) {
	char token[128]{};
	const char* gpsStart = strstr(json, item);

	if (gpsStart) {
		const char* bracketStart = strchr(gpsStart, ':');
		const char* bracketEnd = strchr(gpsStart, ',');
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			char gpsData[256];
			int len = bracketEnd - bracketStart + 1;
			strncpy(gpsData, bracketStart, len);
			gpsData[len] = '\0';
			printf("%s: %s\n", item, gpsData);
		}
	}
	else {
		printf("No %s data found.\n", item);
	}
}
//-------------------------------------------------------------------------
static void extract(const char* json, const char* item) {
	char token[128]{};
	const char* gpsStart = strstr(json, item);

	if (gpsStart) {
		const char* bracketStart = strchr(gpsStart, '{');
		const char* bracketEnd = strchr(gpsStart, '}');
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			char gpsData[256];
			int len = bracketEnd - bracketStart + 1;
			strncpy(gpsData, bracketStart, len);
			gpsData[len] = '\0';
			printf("%s: %s\n", item, gpsData);
		}
	}
	else {
		printf("No %s data found.\n", item);
	}
}
//---------------------------------------------◆Touch
static bool extractTouch(const char* json) {
	const char* start = strstr(json, "\"touch\"");
	if (start) {

		const char* bracketStart = strchr(start, '[');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, ']');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';
#ifdef DEBUG
			printf("%s: %s.\n", "touch", buffer);//◆デバッグ
#endif
		}
		//--------
		int count = 0;
		const char* p = buffer;
		while ( *p != '\0') {
			//--------
			char subbuf[256]{};
			bracketStart = strchr(p, '{');
			if (bracketStart) bracketStart++;
			bracketEnd = strchr(p, '}');
			if (bracketEnd) bracketEnd--;
			//--------
			if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
				int len = bracketEnd - bracketStart + 1;
				strncpy(subbuf, bracketStart, len);
				subbuf[len] = '\0';

				//printf(" ---- %s/\n", subbuf);//◆デバッグ

				p = bracketEnd + 3;
				//printf(" p = %s////\n", p);
			}

			char* y = strstr(subbuf, "\"y\"");
			char* x = strstr(subbuf, "\"x\"");
			const char* radius = strstr(subbuf, "\"radius\"");

			char* sep = subbuf;
			while ((sep = strchr(sep, ',')) != nullptr) {
				*sep = '\0';
				sep++;
			}
			/*
			printf("%s\n", x+4);//strlen("\"x\"")+1
			printf("%s\n", y+4);//strlen("\"y\"")+1
			printf("%s\n", radius + 9);//strlen("\"radius\"") + 1
			*/
			if( x ) zigsim->buffer.touch[count].x = strtof(y + 4, nullptr);
			if( y ) zigsim->buffer.touch[count].y = strtof(x + 4, nullptr);
			if( radius ) zigsim->buffer.touch[count].z = strtof(radius + 9, nullptr);
#ifdef DEBUG
			//◆デバッグ
			printf("%d: %.3f, %.3f, %.3f\n", count,
				zigsim->buffer.touch[count].x,
				zigsim->buffer.touch[count].y,
				zigsim->buffer.touch[count].z
				);
#endif
			count++;
		}
		zigsim->buffer.n_touch = count;
		return true;
	}
	else {
		//printf("No %s data found.\n", "touch");
		return false;
	}
}
//---------------------------------------------◆Gyro
static bool extractGyro(const char* json) {
	const char* start = strstr(json, "\"gyro\"");
	if (start) {

		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			printf("%s: %s.\n", "gyro", buffer);//◆デバッグ
		}
		//-------
		char* y = strstr(buffer, "\"y\"");
		char* x = strstr(buffer, "\"x\"");
		char* z = strstr(buffer, "\"z\"");
		/*
		char* sep = buffer;
		while ((sep = strchr(sep, ',')) != nullptr) {
			*sep = '\0';
			sep++;
		}
		*/
		//◆デバッグ
		//printf("%s\n", x+4);//strlen("\"x\"")+1
		//printf("%s\n", y+4);//strlen("\"y\"")+1
		//printf("%s\n", z+4);//strlen("\"z\"") + 1

		if (x) zigsim->buffer.gyro.roll  = -strtof(x + 4, nullptr);
		if (y) zigsim->buffer.gyro.pitch = -strtof(y + 4, nullptr);
		if (z) zigsim->buffer.gyro.yaw   = strtof(z + 4, nullptr);

		//◆デバッグ
		//printf("GYRO: %s %7.3f, %7.3f, %7.3f\n", z+4,
		//	zigsim->buffer.gyro.pitch,
		//	zigsim->buffer.gyro.yaw,
		//	zigsim->buffer.gyro.roll
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gyro");
		return false;
	}
}
//---------------------------------------------◆accel
static bool extractAccel(const char* json) {
	const char* start = strstr(json, "\"accel\"");
	if (start) {

		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			//printf("%s: %s.\n", "accel", buffer);//◆デバッグ
		}
		//-------
		char* y = strstr(buffer, "\"y\"");
		char* x = strstr(buffer, "\"x\"");
		char* z = strstr(buffer, "\"z\"");
		/*
		char* sep = buffer;
		while ((sep = strchr(sep, ',')) != nullptr) {
			*sep = '\0';
			sep++;
		}
		*/
		//◆デバッグ
		//printf("%s\n", x+4);//strlen("\"x\"")+1
		//printf("%s\n", y+4);//strlen("\"y\"")+1
		//printf("%s\n", z+4);//strlen("\"z\"") + 1

		if (x) zigsim->buffer.accel.z = -strtof(x + 4, nullptr);
		if (y) zigsim->buffer.accel.x = -strtof(y + 4, nullptr);
		if (z) zigsim->buffer.accel.y = strtof(z + 4, nullptr);

		//◆デバッグ
		//printf("ACCEL: %7.3f, %7.3f, %7.3f\n",
		//	zigsim->buffer.accel.x,
		//	zigsim->buffer.accel.y,
		//	zigsim->buffer.accel.z
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "accel");
		return false;
	}
}//---------------------------------------------◆gravity
static bool extractGravity(const char* json) {
	const char* start = strstr(json, "\"gravity\"");
	if (start) {

		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			//printf("%s: %s.\n", "gravity", buffer);//◆デバッグ
		}
		//-------
		char* y = strstr(buffer, "\"y\"");
		char* x = strstr(buffer, "\"x\"");
		char* z = strstr(buffer, "\"z\"");
		/*
		char* sep = buffer;
		while ((sep = strchr(sep, ',')) != nullptr) {
			*sep = '\0';
			sep++;
		}
		*/
		//◆デバッグ
		//printf("%s\n", x+4);//strlen("\"x\"")+1
		//printf("%s\n", y+4);//strlen("\"y\"")+1
		//printf("%s\n", z+4);//strlen("\"z\"") + 1

		if (x) zigsim->buffer.gravity.z = -strtof(x + 4, nullptr);
		if (y) zigsim->buffer.gravity.x = -strtof(y + 4, nullptr);
		if (z) zigsim->buffer.gravity.y = strtof(z + 4, nullptr);

		//◆デバッグ
		//printf("ACCEL: %7.3f, %7.3f, %7.3f\n",
		//	zigsim->buffer.gravity.x,
		//	zigsim->buffer.gravity.y,
		//	zigsim->buffer.gravity.z
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gravity");
		return false;
	}
}
//---------------------------------------------◆Compass
static bool extractCompass(const char* json) {
	const char* start = strstr(json, "\"compass\"");
	if (start) {
		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			//printf("%s: %s.\n", "compass", buffer);//◆デバッグ
		}
		//-------
		char* azimuth = strstr(buffer, "\"compass\"");
		char* faceup = strstr(buffer, "\"faceup\"");
		char* comma = strchr(buffer, ',');
		*comma = '\0';

		//printf("%s\n", azimuth+10);//strlen("\"compass\"")+1 	//◆デバッグ

		if (azimuth) zigsim->buffer.compass = strtof(azimuth + 10, nullptr);
		NormalizeYaw(&(zigsim->buffer.compass));
		/*
		{
			static bool first = true;
			if (first) {
				zigsim->buffer.north = zigsim->buffer.compass;
				zigsim->buffer.compass = 0.0;
				first = false;
			}
			else {
				zigsim->buffer.compass -= zigsim->buffer.north;
				NormalizeYaw(&(zigsim->buffer.compass));
			}
		}
		*/

		if (faceup) zigsim->buffer.faceup = strtof(faceup + 9, nullptr);

		//◆デバッグ
		//printf("GYRO: %s %7.3f\n", azimuth + 10,
		//	zigsim->buffer.compass,
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gyro");
		return false;
	}
}
//---------------------------------------------◆GPS
static bool extractGPS(const char* json) {
	const char* start = strstr(json, "\"gps\"");
	if (start) {
		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			//printf("%s: %s.\n", "gps", buffer);//◆デバッグ
		}
		//-------
		char* latitude = strstr(buffer, "\"latitude\"");
		char* longitude = strstr(buffer, "\"longitude\"");
		char* comma = strchr(buffer, ',');
		*comma = '\0';

		//printf("%s\n", azimuth+10);//strlen("\"gps\"")+1 	//◆デバッグ

		if (latitude ) zigsim->buffer.latitude =  strtof(latitude  + 11, nullptr);
		if (longitude) zigsim->buffer.longitude = strtof(longitude + 12, nullptr);

		//◆デバッグ
		//printf("GPS: %s %7.3f\n",  + 10,
			//zigsim->buffer.longitude,
			//zigsim->buffer.latitude
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gyro");
		return false;
	}
}
//---------------------------------------------◆PRESSURE
static bool extractPressure(const char* json) {
	const char* start = strstr(json, "\"pressure\"");
	if (start) {
		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			printf("%s: %s.\n", "pressure", buffer);//◆デバッグ
		}
		//-------
		char* pressure = strstr(buffer, "\"pressure\"");
		char* altitude = strstr(buffer, "\"altitude\"");
		char* comma = strchr(buffer, ',');
		*comma = '\0';

		if (pressure) zigsim->buffer.pressure = strtof(pressure + 11, nullptr);
		if (altitude) zigsim->buffer.altitude = strtof(altitude + 11, nullptr);

		//◆デバッグ
		//printf("PRESSURE: %s %7.3f\n", azimuth + 10,
		//	zigsim->buffer.compass,
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gyro");
		return false;
	}
}
//---------------------------------------------◆Quarternion
static bool extractQuaternion(const char* json) {
	const char* start = strstr(json, "\"quaternion\"");
	if (start) {
		const char* bracketStart = strchr(start, '{');
		if (bracketStart) bracketStart++;
		const char* bracketEnd = strchr(start, '}');
		if (bracketEnd) bracketEnd--;
		//--------
		char buffer[512]{};
		if (bracketStart && bracketEnd && bracketEnd > bracketStart) {
			int len = bracketEnd - bracketStart + 1;
			strncpy(buffer, bracketStart, len);
			buffer[len] = '\0';

			//printf("%s: %s.\n", "quaternion", buffer);//◆デバッグ
		}
		//-------
		char* x = strstr(buffer, "\"x\"");
		char* y = strstr(buffer, "\"y\"");
		char* z = strstr(buffer, "\"z\"");
		char* w = strstr(buffer, "\"w\"");
		char* comma = strchr(buffer, ',');
		*comma = '\0';

		//printf("%s\n", azimuth+10);//strlen("\"quaternion\"")+1 	//◆デバッグ

		if (x) zigsim->buffer.qt.x = strtof(x + 4, nullptr);
		if (y) zigsim->buffer.qt.y = strtof(y + 4, nullptr);
		if (z) zigsim->buffer.qt.z = strtof(z + 4, nullptr);
		if (w) zigsim->buffer.qt.w = strtof(w + 4, nullptr);

		QtToEuler(&zigsim->buffer.qt, &zigsim->buffer.angle);

		{
			static bool first = true;
			if (!first) {
				zigsim->buffer.zero = zigsim->buffer.angle.yaw;
				zigsim->buffer.angle.yaw = 0;
				first = false;
			}
			else {
				zigsim->buffer.angle.yaw -= zigsim->buffer.zero;
				NormalizeYaw(&(zigsim->buffer.angle.yaw));
			}
		}

		//◆デバッグ
		//printf("GYRO: %s %7.3f\n", azimuth + 10,
		//	zigsim->buffer.compass,
		//);
		return true;
	}
	else {
		//printf("No %s data found.\n", "gyro");
		return false;
	}
}
//-------------------------------------◆データ受信
static int getZigSim( SOCKET sock ) {
	//-------- データ受信
	struct sockaddr_in client;
	int clientLen = sizeof(client);
	char buffer[1024];

	int recvLen = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client, &clientLen);
	if (recvLen == SOCKET_ERROR) {
		printf("recvfrom failed.\n");
		return -1;
	}
	buffer[recvLen] = '\0'; // 文字列終端

#ifdef DEBUG //<<<<◆デバッグ
	printf("Received: %s\n", buffer);
	//putchar('.');
#else

	extractTouch(buffer);
	extractGyro(buffer);
	extractCompass(buffer);

	extractQuaternion(buffer);
	extractAccel(buffer);
	extractGravity(buffer);
	extractGPS(buffer);
	extractPressure(buffer);

	//extract(buffer, "\"gps\"");
	//extract(buffer, "\"gyro\"");
#endif

	return 0;
}
//-------------------------------------------------◆子スレッド関数
static DWORD WINAPI ThreadFunc(LPVOID lpParam) {
	//◆CRITICAL_SECTION cs;
	while (1) {
		EnterCriticalSection(&cs);
		getZigSim(sock);
		LeaveCriticalSection(&cs);
		Sleep(10);
	}
	return 0;
}
//-----------------------------------------------◆子スレッド起動
static int InvokeThread(void)
{
	//-------- ソケット作成
	{
		//-------- Winsock初期化
		{
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				printf("WSAStartup failed.\n");
				return 1;
			}
		}
		//-------- ソケット作成
		{
			sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock == INVALID_SOCKET) {
				printf("Socket creation failed.\n");
				WSACleanup();
				return 1;
			}
		}
		//-------- サーバー情報設定
		{
			struct sockaddr_in server;

			server.sin_family = AF_INET;
			server.sin_addr.s_addr = INADDR_ANY;
			server.sin_port = htons(50000); // ★ZIGSIMの送信ポートと一致させること

			// バインド
			if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
				printf("Bind failed.\n");
				closesocket(sock);
				WSACleanup();
				return 1;
			}
		}
		printf("Listening for UDP packets on port 50000...\n");
	}
	//-------- マルチスレッド起動
	{
		DWORD threadId;
		InitializeCriticalSection(&cs);
		HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadId);
		if (hThread == NULL) {
			printf("スレッド作成に失敗しました。\n");
			return 1;
		}
	}
	Sleep(1000);
}
//-------------------------◆クリーンアップ
static void TermThread()
{
	//--------◆ソケットクローズ
	{
		closesocket(sock);
		WSACleanup();
	}
	//--------◆スレッドクローズ
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		DeleteCriticalSection(&cs);
	}
	return;
}
//##############################################################
//===================================================◆init
void ezZigSim::init(void)
{
	InvokeThread();
}
//===================================================◆update
void ezZigSim::update( bool debug)
{
	//EnterCriticalSection(&cs);
	memcpy((void*)&data, (void*)&buffer, sizeof(ezZigSimDataT));
	//LeaveCriticalSection(&cs);

	if (debug) {
		
		for (int i = 0; i < zigsim->data.n_touch; i++) {
			printf("TOUCH[%d] %7.3f, %7.3f, %7.3f\n", i,
				zigsim->data.touch[i].x,
				zigsim->data.touch[i].y,
				zigsim->data.touch[i].z
			);
		}
		
		printf("COMPASS: %3.1f > %7.3f\n", zigsim->data.faceup, zigsim->data.compass);
		
		printf("GYRO: %7.3f, %7.3f, %7.3f\n",
			zigsim->data.gyro.roll,
			zigsim->data.gyro.pitch,
			zigsim->data.gyro.yaw
		);
		printf("QUATERNION: %7.3f, %7.3f, %7.3f, %7.3f\n",
			zigsim->data.qt.x,
			zigsim->data.qt.y,
			zigsim->data.qt.z,
			zigsim->data.qt.w
		);
		printf("EULER: %7.3f, %7.3f, %7.3f\n",
			zigsim->data.angle.roll,
			zigsim->data.angle.pitch,
			zigsim->data.angle.yaw
		);
		printf("ACCEL: %7.3f, %7.3f, %7.3f\n",
			zigsim->data.accel.x,
			zigsim->data.accel.y,
			zigsim->data.accel.z
		);
		printf("GRAVITY: %7.3f, %7.3f, %7.3f\n",
			zigsim->data.gravity.x,
			zigsim->data.gravity.y,
			zigsim->data.gravity.z
		);
		printf("PRESSURE: %7.3f, %7.3f\n",
			zigsim->data.pressure,
			zigsim->data.altitude
		);
		printf("GPS: %7.3f, %7.3f\n",
			zigsim->data.latitude,
			zigsim->data.longitude
		);
	}
}
//===================================================◆term
void ezZigSim::term(void)
{
	TermThread();
}
/*
//##############################################################
int main() {


	//InvokeThread(); 
	zigsim->init();//◆スレッド開始

	//◆メインスレッド
	while( 1 ){

		printf("----\n");

		zigsim->update( true );

		Sleep(100);
	}

	//TermThread(); 
	zigsim->term();//◆スレッド終了

	return 0;
}
*/
// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します