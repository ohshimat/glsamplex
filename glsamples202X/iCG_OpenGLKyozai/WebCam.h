#pragma once
#include <stdlib.h>

//-------- VIDEO INPUT FUNCTIONS --------
int ezWebCam_Open(int cam, int wx, int wy, double fps = 30.0, int device=-1, int mstype=-1);
int ezWebCam_OpenName( int cam, const char *devicename, int wx, int wy, double fps = 30.0, int mstype = 1);
void ezWebCam_GetImage( int cam, unsigned char *buffer );
int ezWebCam_Close(int cam);
unsigned char *ezWebCam_image( int cam );
void ezWebCam_setBuffer( int camID, unsigned char *buffer );
unsigned char *ezWebCam_getBuffer( int camID );
bool ezWebCam_isCaptured( int camID );
void ezWebCam_GetFormat(  char *devicename = NULL  );

class WebCam {
public:
	WebCam(){ _buffer = nullptr; };
	~WebCam(){};
	void config( int wx, int wy, float fps ){
		_wx = wx;
		_wy = wy;
		_fps = fps;

	}
	void open( int cam, unsigned char *buf = nullptr ){
		_cam = cam;
		ezWebCam_Open( _cam, _wx, _wy, _fps );
		_setBuffer( buf );
	}
	void open( int cam, const char *name, unsigned char *buf = nullptr ){
		_cam = cam;
		ezWebCam_OpenName( _cam, name, _wx, _wy, _fps );
		_setBuffer( buf );
	}
	unsigned char *read( unsigned char *buffer )
	{
		ezWebCam_GetImage( _cam, buffer );
		return buffer;
	}

	void close( void ){
		ezWebCam_Close( _cam );
	}
	bool isLatest( void ){
		return ezWebCam_isCaptured( _cam );
	}
	int width( void ){ return _wx; }
	int height( void ){ return _wy; }
	float fps( void ){ return _fps; }
	unsigned char *buffer( void ){ return ezWebCam_image( _cam ); }

private:
	void _setBuffer( unsigned char *buf = nullptr ){
		if (buf == nullptr) {
			//_buffer = (unsigned char *)calloc(_wx * _wy, 4);
			//ezWebCam_setBuffer(_cam, _buffer);/////////////
			_buffer = ezWebCam_getBuffer(_cam);
		}
		else{
			ezWebCam_setBuffer( _cam, buf );
			_buffer = buf;
		}
	}
	int _cam;
	int _wx, _wy;
	float _fps;
	unsigned char *_buffer;
};
