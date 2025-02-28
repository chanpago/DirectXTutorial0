#ifndef _GAMECLASS_H_
#define _GAMECLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "inputclass.h"
#include "bitmapclass.h"
#include "textureshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GameClass
////////////////////////////////////////////////////////////////////////////////
class GameClass
{
public:
    GameClass();
    ~GameClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

private:
    bool HandleInput();
    void UpdatePositions();

private:
    BitmapClass* playerBitmap;
    BitmapClass* enemyBitmap1;
    BitmapClass* enemyBitmap2;

    int screenWidth, screenHeight;
    int playerX, playerY;
    int enemy1X, enemy1Y;
    int enemy2X, enemy2Y;

    LPDIRECTINPUT8 directInput;
    LPDIRECTINPUTDEVICE8 keyboard;
    unsigned char keyboardState[256];
};

#endif