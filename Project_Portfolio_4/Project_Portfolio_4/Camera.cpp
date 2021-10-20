//
//#include "framework.h"
//#include "Project_Portfolio_4.h"
//#include "Math_Library.h"
//#include <d3d11.h>
//#pragma comment (lib, "d3d11.lib")
//#include <Windows.h>
//
//#include <DirectXMath.h>
//using namespace DirectX;
//XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//
//XMMATRIX camRotationMatrix;
//XMMATRIX groundWorld;
//
//float moveLeftRight = 0.0f;
//float moveBackForward = 0.0f;
//
//float camYaw = 0.0f;
//float camPitch = 0.0f;
////
////void UpdateCamera()
////{
////    camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
////    XMVECTOR camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
////    camTarget = XMVector3Normalize(camTarget);
////
////    XMMATRIX RotateYTempMatrix;
////    RotateYTempMatrix = XMMatrixRotationY(camYaw);
////
////    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
////    XMVECTOR camUp;
////    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
////    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
////
////    XMVECTOR camPosition;
////    camPosition += moveLeftRight * camRight;
////    camPosition += moveBackForward * camForward;
////
////    moveLeftRight = 0.0f;
////    moveBackForward = 0.0f;
////
////    camTarget = camPosition + camTarget;
////
////    XMMATRIX camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
////}
//void Camera(XMMATRIX world_matrix)
//{
//    float x =0, y =0, z=0;
//    //Use Get Cursor Pos for cursor input
//    POINT point;
//    if (GetCursorPos(&point) == TRUE)
//    {
//        //Matrix rotation
//        world_matrix = XMMatrixRotationY(point.x);
//    }
//
//    //Matrix translation
//    if (GetAsyncKeyState(VK_LEFT) & 0x01)
//    {
//        //go left
//        //world goes right
//        x++;
//        world_matrix = XMMatrixTranslation(x, 0, 0);
//    }
//    if (GetAsyncKeyState(VK_RIGHT) & 0x01)
//    {
//        //go right
//        //world goes left
//        x--;
//        world_matrix = XMMatrixTranslation(x, 0, 0);
//    }
//    if (GetAsyncKeyState(VK_UP) & 0x01)
//    {
//        //go forwards
//        //z goes down
//        z--;
//        world_matrix = XMMatrixTranslation(0, 0, z);
//    }
//    if (GetAsyncKeyState(VK_DOWN) & 0x01)
//    {
//        //go backwards
//        //z goes up
//        z++;
//        world_matrix = XMMatrixTranslation(0, 0, z);
//    }
//}