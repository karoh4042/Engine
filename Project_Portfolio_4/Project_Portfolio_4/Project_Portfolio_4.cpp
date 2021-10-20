// Project_Portfolio_4.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Project_Portfolio_4.h"
#include "Math_Library.h"
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "VertexShader.csh"
#include "PixelShader.csh"
#include "MeshVertexShader.csh"
#include "MeshPixelShader.csh"
#include "InstancedShader.csh"
#include "TransparencyShader.csh"

#include "DDSTextureLoader.h"
#include "Assets/test pyramid.h"
#include "Assets/talon.h"
#include "Assets/bluegem.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

void Camera(XMMATRIX& world_matrix);
XMVECTOR GetPosition(XMMATRIX matrix);
void readFile(const char* filePath);


//for init
ID3D11Device* myDev = nullptr;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

//for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
float aspectRatio = 1;
ID3D11BlendState* myBlend;
ID3D11BlendState* transparentBlend;
D3D11_BLEND_DESC blendDesc;

struct myVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT4 Color;
    
};

struct CubeVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Texcoord;
    XMFLOAT3 Normal;
    //XMFLOAT3 uvw;
};

unsigned int numVerts = 0;
ID3D11Buffer* vertexBuffer;
ID3D11InputLayout* vertexLayout;
ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;

ID3D11Buffer* constantBuffer;//shaders
ID3D11Buffer* instancedConstantBuffer;

D3D11_RASTERIZER_DESC skybox;
ID3D11RasterizerState* skyboxState_True;
ID3D11RasterizerState* skyboxState_False;

//mesh data
ID3D11Buffer* vertexBufferMesh; //vertex buffer
ID3D11Buffer* indexBufferMesh;//index buffer
ID3D11Buffer* cubeIndexBufferMesh;//index buffer for cube
ID3D11Buffer* cubeVertexBufferMesh;//vertex buffer for cube

//second cube
ID3D11Buffer* CVBufferMesh; //vertex buffer
ID3D11Buffer* CIBufferMesh;//index buffer

//gem buffers
ID3D11Buffer* GemVertexBufferMesh; //vertex buffer
ID3D11Buffer* GemIndexBufferMesh;//index buffer

//mesh vertex shader
ID3D11VertexShader* meshVertexShader;
ID3D11PixelShader* meshPixelShader;
ID3D11PixelShader* transparentPixelShader;

ID3D11VertexShader* instancedShader;
ID3D11InputLayout* meshVertexLayout;
//gem input layout
ID3D11InputLayout* GemVertexLayout;

//texture
ID3D11ShaderResourceView* srv;
ID3D11ShaderResourceView* srv2;
ID3D11ShaderResourceView* cubeMapResource;
ID3D11SamplerState* sampler;

std::vector< XMFLOAT3 > vertices;
std::vector< XMFLOAT2 > uvs;
std::vector< XMFLOAT3 > normals; // Won't be used at the moment.


//math
struct wvp
{
    XMFLOAT4X4 world_matrix;//sstorage type
    XMFLOAT4X4 view_matrix;
    XMFLOAT4X4 projection_matrix;
};

wvp myMatrices;

struct wvp_instance
{
    XMFLOAT4X4 world_matrix[5];//sstorage type
    XMFLOAT4X4 view_matrix;
    XMFLOAT4X4 projection_matrix;
};
wvp_instance instancedMatrices;
#define MAX_LOADSTRING 100 

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECTPORTFOLIO4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECTPORTFOLIO4));

    MSG msg;

    XMMATRIX temp = XMMatrixIdentity();
    temp = XMMatrixTranslation(0, 0, 2);
    XMMATRIX temp2 = XMMatrixRotationY(0.1f);
    temp = XMMatrixMultiply(temp2, temp);
    XMStoreFloat4x4(&myMatrices.world_matrix, temp);
    XMMATRIX camera = XMMatrixInverse(nullptr, XMMatrixLookAtLH({ 0, 2, -10 }, { 0, 0, 0 }, { 0, 1, 0 }));
    XMMATRIX  matrixOne = XMMatrixIdentity();
    XMMATRIX  matrixTwo = XMMatrixIdentity();
    XMMATRIX  matrixThree = XMMatrixIdentity();
    XMMATRIX  matrixFour = XMMatrixIdentity();
    XMMATRIX  matrixFive = XMMatrixIdentity();

    XMMATRIX matrixArray[] = { matrixOne, matrixTwo, matrixThree, matrixFour, matrixFive };
    for (int i = 0; i < 5; i++)
    {
        float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 10.0f;
        float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 10.0f;
        float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 10.0f;
        matrixArray[i] = XMMatrixMultiply(XMMatrixTranslation(x, y, z), XMMatrixScaling(5, 5, 5));
    }
    //XMStoreFloat4x4(&myMatrices.view_matrix, temp);
    //set skybox xube to camera pos every frame.
    
    //for drawing inside out, set up a rasterizer description
    //change FRONTCOUNTERCLOCKWISE,  set to true
   // so make 2 createRasterizerState
    //draw skybox, then call again 


    // Main message loop:
    while (true)
    {
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.wParam == WM_QUIT)
        {
            break;
        }

        //rendering here; uses device Context
        float color[] = { 0, 0, 0, 0 };
        myCon->ClearRenderTargetView(myRtv, color);

        //setup the pipeline

        //outputmerger
        ID3D11RenderTargetView* tempRTV[] = { myRtv };
        myCon->OMSetRenderTargets(1, tempRTV, nullptr);
        //rasterizer
        myCon->RSSetViewports(1, &myPort);
        //input assembler
        myCon->IASetInputLayout(vertexLayout);
        UINT strides[] = { sizeof(CubeVertex) };
        UINT offsets[] = { 0 };
        ID3D11Buffer* tempVB[] = { vertexBuffer };
        myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
        myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //Vertex shader stage
        myCon->VSSetShader(vertexShader, 0, 0);
        //Pixel Shader
        myCon->PSSetShader(pixelShader, 0, 0);
        
        //3d stuff
        //Matrix
        static float rot = 0; rot += 0.1f;
       
        //temp = XMMatrixTranslation(0, 0, 2);
        XMMATRIX temp2 = XMMatrixRotationY(rot);
        temp = XMMatrixMultiply(temp2, temp);

        //view matrix

        Camera(camera);

        XMStoreFloat4x4(&myMatrices.view_matrix, XMMatrixInverse(nullptr, camera));
        //projection matrix
        temp = XMMatrixPerspectiveFovLH(3.14 / 2.0f, aspectRatio, 0.1f, 1000);
        XMStoreFloat4x4(&myMatrices.projection_matrix, temp);

        XMVECTOR camera_pos = GetPosition(camera);
        XMMATRIX world = XMMatrixIdentity();
        world.r[3] = camera_pos;
        XMStoreFloat4x4(&myMatrices.world_matrix, world);


        //upload to video card
        D3D11_MAPPED_SUBRESOURCE  gpuBuffer;
        myCon->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);

        *((wvp*)(gpuBuffer.pData)) = myMatrices;
        //memcpy(gpuBuffer.pData, &myMatrices, sizeof(wvp));
        myCon->Unmap(constantBuffer, 0);

        //apply math to vertex shader
        //connect consant buffer to pipeline
        ID3D11Buffer* constants[] = { constantBuffer };
        myCon->VSSetConstantBuffers(0, 1, constants);
        myCon->PSSetConstantBuffers(0, 1, constants);


        myCon->Map(instancedConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);

        *((wvp_instance*)(gpuBuffer.pData)) = instancedMatrices;
        //memcpy(gpuBuffer.pData, &myMatrices, sizeof(wvp));
        myCon->Unmap(instancedConstantBuffer, 0);

        ID3D11Buffer* instancedConstants[] = { instancedConstantBuffer };
        myCon->VSSetConstantBuffers(1, 1, instancedConstants);
        myCon->PSSetConstantBuffers(1, 1, instancedConstants);

        //set skybox xube to camera pos every frame.
        
  //for drawing inside out, set up a rasterizer description
       
  //change FRONTCOUNTERCLOCKWISE,  set to true
 // so make 2 createRasterizerState
  //draw skybox, then call again 
        //draw cube
        myCon->IASetIndexBuffer(cubeIndexBufferMesh, DXGI_FORMAT_R32_UINT, 0);
        myCon->IASetInputLayout(meshVertexLayout);
        //front counter clockwise - true means 
       myCon->RSSetState(skyboxState_True);
       ID3D11ShaderResourceView* srv1[1] = { cubeMapResource };
       myCon->PSSetShaderResources(0, 1, srv1);
        myCon->DrawIndexed(36, 0, 0);
        //myCon->Draw(numVerts, 0);
         myCon->RSSetState(skyboxState_False);

         //draw plane
         temp = XMMatrixIdentity();
         temp = XMMatrixTranslation(0, -5, 0);
         temp2 = XMMatrixRotationY(0.1f);
         temp = XMMatrixMultiply(temp2, temp);
         temp = XMMatrixScaling(5, 0.1f, 5);
         XMStoreFloat4x4(&myMatrices.world_matrix, temp);
         myCon->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);

         *((wvp*)(gpuBuffer.pData)) = myMatrices;
         //memcpy(gpuBuffer.pData, &myMatrices, sizeof(wvp));
         myCon->Unmap(constantBuffer, 0);

         UINT cubeStrides[] = { sizeof(CubeVertex) };
         UINT cubeOffsets[] = { 0 };
         ID3D11Buffer* CMesh[] = { CVBufferMesh };
         myCon->IASetVertexBuffers(0, 1, CMesh, cubeStrides, cubeOffsets);
         myCon->IASetIndexBuffer(CIBufferMesh, DXGI_FORMAT_R32_UINT, 0);
         myCon->VSSetShader(meshVertexShader, 0, 0);
         //Pixel Shader
         myCon->PSSetShader(meshPixelShader, 0, 0);
         myCon->IASetInputLayout(meshVertexLayout);
         ID3D11ShaderResourceView* srv3[1] = { srv2 };
         myCon->PSSetShaderResources(0, 1, srv3);
         myCon->PSSetSamplers(0, 1, &sampler);
         //draw plane
         myCon->DrawIndexed(36, 0, 0);

         //draw gem
         temp = XMMatrixIdentity();
         temp = XMMatrixTranslation(0, 1, 0);
         temp2 = XMMatrixRotationY(0.1f);
         temp = XMMatrixMultiply(temp2, temp);

         XMStoreFloat4x4(&myMatrices.world_matrix, temp);
         myCon->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
         *((wvp*)(gpuBuffer.pData)) = myMatrices;
         myCon->Unmap(constantBuffer, 0);

         //set pipeline pyramid
         UINT GemMeshStrides[] = { sizeof(_OBJ_VERT_) };
         UINT GemMeshOffsets[] = { 0 };
         ID3D11Buffer* GemMeshVB[] = { GemVertexBufferMesh };
         /*myCon->VSSetConstantBuffers(0, 1, &instancedConstantBuffer);
         myCon->PSSetConstantBuffers(0, 1, &instancedConstantBuffer);*/
         myCon->IASetVertexBuffers(0, 1, GemMeshVB, GemMeshStrides, GemMeshOffsets);
         myCon->IASetIndexBuffer(GemIndexBufferMesh, DXGI_FORMAT_R32_UINT, 0);
         myCon->VSSetShader(meshVertexShader, 0, 0);
         //Pixel Shader
         ID3D11PixelShader* tempShader = nullptr;
         //myCon->OMSetBlendState(transparentBlend, 0, 0);
         bool transparentState = false;
         if (GetAsyncKeyState(VK_SPACE) & 0x01)
         {
             transparentState = !transparentState;
         }
         if (transparentState == true)
         {
            tempShader = transparentPixelShader;
         }
         else
         {
             tempShader = meshPixelShader;
         }
         myCon->IASetInputLayout(GemVertexLayout);
         ID3D11ShaderResourceView* Gem_srvs[2] = { srv, srv2 };
         myCon->PSSetShaderResources(0, 2, Gem_srvs);
         myCon->PSSetSamplers(0, 1, &sampler);
         //draw pyramid
        // myCon->DrawIndexed(1368, 0, 0);
         myCon->PSSetShader(tempShader, 0, 0);
         myCon->DrawIndexed(234, 0, 0);
        // myCon->OMSetBlendState(myBlend, 0, 0);
        
         for (int i = 0; i < 5; i++)
         {
             XMStoreFloat4x4(&instancedMatrices.world_matrix[i], matrixArray[i]);
         }
         /*temp = XMMatrixIdentity();
         temp = XMMatrixTranslation(0, 1, 0);
         temp2 = XMMatrixRotationY(0.1f);
         temp = XMMatrixMultiply(temp2, temp);
         temp = XMMatrixScaling(5, 5, 5);*/
         
         myCon->Map(instancedConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
         instancedMatrices.view_matrix = myMatrices.view_matrix;
         instancedMatrices.projection_matrix = myMatrices.projection_matrix;
         *((wvp_instance*)(gpuBuffer.pData)) = instancedMatrices;
         myCon->Unmap(instancedConstantBuffer, 0);

        //set pipeline pyramid
        UINT meshStrides[] = { sizeof(_OBJ_VERT_) };
        UINT meshOffsets[] = { 0 };
        ID3D11Buffer* meshVB[] = { vertexBufferMesh };
        myCon->VSSetConstantBuffers(0, 1, &instancedConstantBuffer);
        myCon->PSSetConstantBuffers(0, 1, &instancedConstantBuffer);
        myCon->IASetVertexBuffers(0, 1, meshVB, meshStrides, meshOffsets);
        myCon->IASetIndexBuffer(indexBufferMesh, DXGI_FORMAT_R32_UINT, 0);
        myCon->VSSetShader(instancedShader, 0, 0);
        //Pixel Shader
       myCon->PSSetShader(meshPixelShader, 0, 0);
        myCon->IASetInputLayout(meshVertexLayout);
        ID3D11ShaderResourceView* srvs[2] = { srv, srv2 };
       myCon->PSSetShaderResources(0, 2, srvs);
       myCon->PSSetSamplers(0, 1, &sampler);
        //draw pyramid
       // myCon->DrawIndexed(1368, 0, 0);
       myCon->DrawIndexedInstanced(1368, 5, 0, 0, 0);
       
       


        mySwap->Present(1, 0);
    }

    //release all our d3d11 interfaces
    myCon->Release();
    mySwap->Release();
    myRtv->Release();
    vertexShader->Release();
    pixelShader->Release();
    vertexLayout->Release();
    myDev->Release();
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTPORTFOLIO4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECTPORTFOLIO4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, 1920, 1080, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //see how big window is
   RECT myWinR;
   GetClientRect(hWnd, &myWinR);

   //attach d3d11 to our window
   D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
   DXGI_SWAP_CHAIN_DESC swap;
   ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
   swap.BufferCount = 1;
   swap.OutputWindow = hWnd;
   swap.Windowed = true;
   swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
   swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   //makes sure you dont overwrite the window borders
   swap.BufferDesc.Width = myWinR.right - myWinR.left;
   swap.BufferDesc.Height = myWinR.bottom - myWinR.top;
   swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swap.SampleDesc.Count = 1;
   HRESULT hr;
   hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &dx11, 1, D3D11_SDK_VERSION, 
       &swap, &mySwap, &myDev,0, &myCon);

   ID3D11Resource* backBuffer = 0;
   hr = mySwap->GetBuffer(0, _uuidof(backBuffer), (void**)&backBuffer);
   
   if (backBuffer)
   {
        myDev->CreateRenderTargetView(backBuffer, NULL, &myRtv);
   }

   backBuffer->Release();

   myPort.Width = swap.BufferDesc.Width;
   myPort.Height = swap.BufferDesc.Height;
   myPort.TopLeftX = myPort.TopLeftY = 0;
   myPort.MinDepth = 0;
   myPort.MaxDepth = 1;


   // POS - UV - NORM
   CubeVertex cube[] =
   {
       { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(0.0f, 1.0f, 0.0f)  },//XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),     XMFLOAT3(0.0f,1.0f, 0.0f) }, //XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) ,   XMFLOAT3(0.0f, 1.0f, 0.0f)},// XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),     XMFLOAT3(0.0f, 1.0f, 0.0f)},//  XMFLOAT3(1.0f, 1.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f)},//  XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(0.0f, -1.0f, 0.0f)} ,//  XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(0.0f, -1.0f, 0.0f) },//  XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f)} ,//  XMFLOAT3(1.0f, 1.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(-1.0f, 0.0f, 0.0f)} , // XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, // XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) ,  XMFLOAT3(-1.0f, 0.0f, 0.0f)}, // XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) ,   XMFLOAT3(-1.0f, 0.0f, 0.0f)},//  XMFLOAT3(1.0f, 1.0f, 1.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(1.0f, 0.0f, 0.0f)},  //XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}, // XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) ,  XMFLOAT3(1.0f, 0.0f, 0.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 0.0f, 0.0f) }, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(0.0f, 0.0f, 1.0f) }, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) ,   XMFLOAT3(0.0f, 0.0f, 1.0f)}, //  XMFLOAT3(1.0f, 1.0f, 1.0f)},
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(0.0f, 0.0f, 1.0f)} //  XMFLOAT3(1.0f, 1.0f, 1.0f)},

   };

   const unsigned int cube_indices[] =
   {
       3,1,0,
       2,1,3,

       6,4,5,
       7,4,6,

       11,9,8,
       10,9,11,

       14,12,13,
       15,12,14,

       19,17,16,
       18,17,19,

       22,20,21,
       23,20,22
   };
   numVerts = ARRAYSIZE(cube);

   //skybox specific stuff
   skybox.FillMode = D3D11_FILL_SOLID;
   skybox.CullMode = D3D11_CULL_BACK;
   skybox.FrontCounterClockwise = TRUE;
   skybox.DepthBias = 0;
   skybox.SlopeScaledDepthBias = 0.0f;
   skybox.DepthBiasClamp = 0.0f;
   skybox.DepthClipEnable = TRUE;
   skybox.ScissorEnable = FALSE;
   skybox.MultisampleEnable = FALSE;
   skybox.AntialiasedLineEnable = FALSE;
   hr = myDev->CreateRasterizerState(&skybox, &skyboxState_True);

   skybox.FillMode = D3D11_FILL_SOLID;
   skybox.CullMode = D3D11_CULL_BACK;
   skybox.FrontCounterClockwise = FALSE;
   skybox.DepthBias = 0;
   skybox.SlopeScaledDepthBias = 0.0f;
   skybox.DepthBiasClamp = 0.0f;
   skybox.DepthClipEnable = TRUE;
   skybox.ScissorEnable = FALSE;
   skybox.MultisampleEnable = FALSE;
   skybox.AntialiasedLineEnable = FALSE;
   hr = myDev->CreateRasterizerState(&skybox, &skyboxState_False);
   
   //blendstate (default)
   blendDesc.AlphaToCoverageEnable = FALSE;
   blendDesc.IndependentBlendEnable = FALSE;
   blendDesc.RenderTarget[0].BlendEnable = FALSE;
   blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
   blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
   blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
   blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
   blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
   blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
   blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
   hr = myDev->CreateBlendState(&blendDesc, &myBlend);

   //second blendstate
   blendDesc.AlphaToCoverageEnable = TRUE;
   blendDesc.IndependentBlendEnable = FALSE;
   blendDesc.RenderTarget[0].BlendEnable = FALSE;
   blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
   blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
   blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
   blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
   blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
   blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
   blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
   hr = myDev->CreateBlendState(&blendDesc, &transparentBlend);

       //load it on the card
   D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));
   
   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(myVertex) * numVerts;
   bDesc.CPUAccessFlags = 0;
   bDesc.MiscFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DEFAULT;

   subData.pSysMem = cube;

   hr = myDev->CreateBuffer(&bDesc, &subData, &vertexBuffer);  

   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(cube_indices);
   subData.pSysMem = cube_indices;
   hr = myDev->CreateBuffer(&bDesc, &subData, &cubeIndexBufferMesh);

   /*D3D11_BUFFER_DESC bDesc;
   D3D11_SUBRESOURCE_DATA subData;
   ZeroMemory(&bDesc, sizeof(bDesc));
   ZeroMemory(&subData, sizeof(subData));*/

   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(myVertex) * numVerts;
   bDesc.CPUAccessFlags = 0;
   bDesc.MiscFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DEFAULT;

   subData.pSysMem = cube;

   hr = myDev->CreateBuffer(&bDesc, &subData, &CVBufferMesh);

   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(cube_indices);
   subData.pSysMem = cube_indices;
   hr = myDev->CreateBuffer(&bDesc, &subData, &CIBufferMesh);


   //write and compile & load our shaders   
   //create vertex shader
   hr = myDev->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vertexShader);
   //create pixel shader
   hr = myDev->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pixelShader);
   
   //describe it to d3d11
   //define the input layout
   //cube
   D3D11_INPUT_ELEMENT_DESC layout[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

   };
  UINT numElements = ARRAYSIZE(layout);
   myDev->CreateInputLayout(layout, numElements, VertexShader, sizeof(VertexShader), &vertexLayout);


   //create constant buffer
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(wvp);
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


   bDesc.MiscFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   myDev->CreateBuffer(&bDesc, nullptr, &constantBuffer);
   //create instanced constant buffer
   ZeroMemory(&bDesc, sizeof(bDesc));

   bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bDesc.ByteWidth = sizeof(wvp_instance);
   bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


   bDesc.MiscFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_DYNAMIC;

   myDev->CreateBuffer(&bDesc, nullptr, &instancedConstantBuffer);

   //load texture
   // Load the Texture
   std::string skybox = "SunsetSkybox.dds";

   //load complex mesh
   //vertex buffer mesh
   bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   bDesc.ByteWidth = sizeof(talon_data);
   bDesc.CPUAccessFlags = 0;
   bDesc.MiscFlags = 0;
   bDesc.StructureByteStride = 0;
   bDesc.Usage = D3D11_USAGE_IMMUTABLE;

   subData.pSysMem = talon_data;
   hr = myDev->CreateBuffer(&bDesc, &subData, &vertexBufferMesh);
   //index buffer mesh
   bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bDesc.ByteWidth = sizeof(talon_indicies);
   subData.pSysMem = talon_indicies;
   hr = myDev->CreateBuffer(&bDesc, &subData, &indexBufferMesh); 



   //load our new mesh shader
   hr = myDev->CreateVertexShader(MeshVertexShader, sizeof(MeshVertexShader), nullptr, &meshVertexShader);
   hr = myDev->CreatePixelShader(MeshPixelShader, sizeof(MeshPixelShader), nullptr, &meshPixelShader);

   hr = myDev->CreateVertexShader(InstancedShader, sizeof(InstancedShader), nullptr, &instancedShader);
   hr = myDev->CreatePixelShader(TransparencyShader, sizeof(TransparencyShader), nullptr, &transparentPixelShader);
   // make matching input layout
   //pyramid
   D3D11_INPUT_ELEMENT_DESC meshLayout[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       
       

   };
    hr = myDev->CreateInputLayout(meshLayout, 3, MeshVertexShader, sizeof(MeshVertexShader), &meshVertexLayout);

    //load texture
    if (myDev)
    {
        hr = CreateDDSTextureFromFile(myDev, L"bleachedWood_seamless.dds", nullptr, &srv);
    }
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;
     hr = myDev->CreateSamplerState(&samplerDesc, &sampler);

    if (myDev)
    {
        hr = CreateDDSTextureFromFile(myDev, L"seafloor.dds", nullptr, &srv2);
    }
    //std::string skybox = "SunsetSkybox.dds";

    D3D11_TEXTURE2D_DESC cubeMap;
    cubeMap.Width = 1;
    cubeMap.Height = 1;
    cubeMap.MipLevels = 1;
    cubeMap.ArraySize = 6;
    cubeMap.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    cubeMap.SampleDesc = { 1, 1 };
    cubeMap.Usage = D3D11_USAGE_IMMUTABLE;
    cubeMap.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    cubeMap.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cubeMap.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    if (myDev)
    {
        hr = CreateDDSTextureFromFile(myDev, L"SunsetSkybox.dds", nullptr, &cubeMapResource);
    }

    //gem data
    //vertex buffer mesh
    bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bDesc.ByteWidth = sizeof(bluegem_data);
    bDesc.CPUAccessFlags = 0;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;
    bDesc.Usage = D3D11_USAGE_IMMUTABLE;

    subData.pSysMem = bluegem_data;
    hr = myDev->CreateBuffer(&bDesc, &subData, &GemVertexBufferMesh);
    //index buffer mesh
    bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bDesc.ByteWidth = sizeof(bluegem_indicies);
    subData.pSysMem = bluegem_indicies;
    hr = myDev->CreateBuffer(&bDesc, &subData, &GemIndexBufferMesh);
    
    D3D11_INPUT_ELEMENT_DESC gem_input_layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
         { "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    };

    readFile("C:/Users/Rohit/Desktop/Kakarala_Rohit_PP4/Project_Portfolio_4/Project_Portfolio_4/Assets/cube.obj");

    hr = myDev->CreateInputLayout(meshLayout, 3, MeshVertexShader, sizeof(MeshVertexShader), &GemVertexLayout);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Camera(XMMATRIX& view_matrix)
{
   

        XMMATRIX camera = XMMatrixIdentity();
        //Use Get Cursor Pos for cursor input
        POINT point;
        GetCursorPos(&point);
        static POINT last_point = point;


        float dX = point.x - last_point.x;
        float dY = point.y - last_point.y;
        XMMATRIX xRot = XMMatrixIdentity();
        XMMATRIX yRot = XMMatrixIdentity();

        XMVECTOR  origin = { 0, 0, 0, 1 };


        /*
            if a is pressed we want to turn left
                create a matrix and make it equal rotation of delta y
                create a matrix and make it equal rotation of delta x
                multiply y rotation globally
                multiply x rotation locally
        */
        float fTurnRate = 0.004f;
        float fTranslateRate = 0.22f;

        if (GetAsyncKeyState(MK_RBUTTON))
        {

        //left right look
        yRot = XMMatrixRotationY(dX * fTurnRate);
        XMVECTOR original_pos = view_matrix.r[3];
        view_matrix.r[3] = origin;
        view_matrix = XMMatrixMultiply(view_matrix, yRot);
        //set matrix pos
        view_matrix.r[3] = original_pos;

        //up down look
        xRot = XMMatrixRotationX(dY * fTurnRate);
        //original_pos = view_matrix.r[3];
        //view_matrix.r[3] = origin;
        view_matrix = XMMatrixMultiply(xRot, view_matrix);
        //view_matrix.r[3] = original_pos;

    }
        last_point = point;

    //Matrix translation
        if (GetAsyncKeyState(VK_LEFT) & 0x01 || GetAsyncKeyState(0x41) & 0x01)
        {
            //go left
            //world goes right
            camera = XMMatrixTranslation(-fTranslateRate, 0, 0);
            view_matrix = XMMatrixMultiply(camera, view_matrix);
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x01 || GetAsyncKeyState(0x44) & 0x01)
        {
            //go right
            //world goes left
            camera = XMMatrixTranslation(+fTranslateRate, 0, 0);
            view_matrix = XMMatrixMultiply(camera, view_matrix);
        }
        if (GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(0x57) & 0x01)
        {
            //go forwards
            camera = XMMatrixTranslation(0, 0, fTranslateRate);
            view_matrix = XMMatrixMultiply(camera, view_matrix);
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x01 || GetAsyncKeyState(0x53) & 0x01)
        {
            //go backwards
            camera = XMMatrixTranslation(0, 0, -fTranslateRate);
            view_matrix = XMMatrixMultiply(camera, view_matrix);
        }
        

}
    XMVECTOR GetPosition(XMMATRIX matrix)
    {
        return matrix.r[3];
    }

    void readFile(const char* filePath)
    {
        //Loads OBJ file from path
        std::ifstream file;
        file.open(filePath);
        if (!file.good())
        {
            std::cout << "Can't open texture file " << filePath << std::endl;
        }

    }
  
