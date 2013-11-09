/*----------------------------------------------------------
	Direct3D 11�T���v��
		�EMicrosoft DirectX SDK (February 2010)
		�EVisual Studio 2010 Express
		�EWindows 7/Vista
		�E�V�F�[�_���f��4.0
		�Ή�

	D3D11Sample04.cpp
		�u�g�[���J�[�u�v
--------------------------------------------------------------*/

#define STRICT					// �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��

#include <windows.h>
#include <mmsystem.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>

#include "resource.h"

// �K�v�ȃ��C�u�����������N����
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )

#pragma comment( lib, "winmm.lib" )

/*-------------------------------------------
	�O���[�o���ϐ�(�A�v���P�[�V�����֘A)
--------------------------------------------*/
HINSTANCE	g_hInstance		= NULL;	// �C���X�^���X �n���h��
HWND		g_hWindow		= NULL;	// �E�C���h�E �n���h��

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample04";
WCHAR		g_szWndClass[]	= L"D3D11S04";

// �N�����̕`��̈�T�C�Y
SIZE		g_sizeWindow	= { 640, 480 };		// �E�C���h�E�̃N���C�A���g�̈�

/*-------------------------------------------
	�O���[�o���ϐ�(Direct3D�֘A)
--------------------------------------------*/

//�@�\���x���̔z��
D3D_FEATURE_LEVEL g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; 
UINT              g_FeatureLevels    = 3;   // �z��̗v�f��
D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��

// �C���^�[�t�F�C�X
IDXGIFactory*           g_pFactory = NULL;			// DXGI 
IDXGISwapChain*         g_pSwapChain = NULL;        // �X���b�v�E�`�F�C��

ID3D11Device*           g_pD3DDevice = NULL;        // �f�o�C�X
ID3D11DeviceContext*    g_pImmediateContext = NULL; // �f�o�C�X�E�R���e�L�X�g

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // �`��^�[�Q�b�g�E�r���[
D3D11_VIEWPORT          g_ViewPort[1];				// �r���[�|�[�g

ID3D11Texture2D*          g_pDepthStencil = NULL;		// �[�x/�X�e���V��
ID3D11DepthStencilView*   g_pDepthStencilView = NULL;	// �[�x/�X�e���V���E�r���[

ID3D11InputLayout*        g_pInputLayout = NULL;            // ���̓��C�A�E�g�E�I�u�W�F�N�g
ID3D11Buffer*             g_pVerBuffer[2] = { NULL, NULL }; // ���_�o�b�t�@�̃C���^�[�t�F�C�X
ID3D11Buffer*             g_pIdxBuffer = NULL;              // �C���f�b�N�X�E�o�b�t�@�̃C���^�[�t�F�C�X

ID3D11VertexShader*       g_pVertexShader = NULL;   // ���_�V�F�[�_
ID3D11GeometryShader*     g_pGeometryShader = NULL; // �W�I���g���E�V�F�[�_
ID3D11PixelShader*        g_pPixelShader = NULL;    // �s�N�Z���E�V�F�[�_
ID3D11Buffer*             g_pCBuffer[3] = { NULL, NULL, NULL }; // �萔�o�b�t�@

ID3D11BlendState*         g_pBlendState = NULL;			// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11RasterizerState*	  g_pRasterizerState = NULL;	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11DepthStencilState*  g_pDepthStencilState = NULL;	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g

// �V�F�[�_�̃R���p�C�� �I�v�V����
#if defined(DEBUG) || defined(_DEBUG)
UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
					| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

// �[�x�o�b�t�@�̃��[�h
bool g_bDepthMode = true;

// �X�^���o�C���[�h
bool g_bStandbyMode = false;

// �`��^�[�Q�b�g���N���A����l(RGBA)
float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

XMFLOAT3 g_vLightPos(3.0f, 3.0f, -3.0f);   // �������W(���[���h���W�n)

// ���_�o�b�t�@�@�ɔ[�߂�f�[�^�`��
struct XYZBuffer
{
	XMFLOAT3 Position;   // ���W�l
};

// ���_�o�b�t�@�A�ɔ[�߂�f�[�^�`��
struct ColBuffer
{
	XMFLOAT3 Color;     // �F
};

// �萔�o�b�t�@�̃f�[�^��`�@�@�ύX���Ȃ��f�[�^
struct cbNeverChanges {
	XMFLOAT4X4 Projection;   // �����ϊ��s��
};

// �萔�o�b�t�@�̃f�[�^��`�A�@�ύX�p�x�̒Ⴂ�f�[�^
struct cbChangesEveryFrame {
	XMFLOAT4X4  View;  // �r���[�ϊ��s��
	XMFLOAT3    Light; // �������W(���[���h���W�n)
	FLOAT       dummy; // �_�~�[
};

// �萔�o�b�t�@�̃f�[�^��`�B�@�ύX�p�x�̍����f�[�^
struct cbChangesEveryObject {
	XMFLOAT4X4 World;      // ���[���h�ϊ��s��
};

// �萔�o�b�t�@�̃f�[�^
struct cbNeverChanges       g_cbNeverChanges;       // �����ϊ��s��
struct cbChangesEveryFrame  g_cbChangesEveryFrame;  // �r���[�ϊ��s��@�������W
struct cbChangesEveryObject g_cbChangesEveryObject; // ���[���h�ϊ��s��

/*-------------------------------------------
	�֐���`
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);
HRESULT InitBackBuffer(void);

/*-------------------------------------------
	�A�v���P�[�V����������
--------------------------------------------*/
HRESULT InitApp(HINSTANCE hInst)
{
	// �A�v���P�[�V�����̃C���X�^���X �n���h����ۑ�
	g_hInstance = hInst;

	// �E�C���h�E �N���X�̓o�^
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(hInst, (LPCTSTR)IDI_ICON1);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= g_szWndClass;

	if (!RegisterClass(&wc))
		return DXTRACE_ERR(L"InitApp", GetLastError());

	// ���C�� �E�C���h�E�쐬
	RECT rect;
	rect.top	= 0;
	rect.left	= 0;
	rect.right	= g_sizeWindow.cx;
	rect.bottom	= g_sizeWindow.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	g_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return DXTRACE_ERR(L"InitApp", GetLastError());

	// �E�C���h�E�\��
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/*-------------------------------------------
	Direct3D������
--------------------------------------------*/
HRESULT InitDirect3D(void)
{
	// �E�C���h�E�̃N���C�A���g �G���A
	RECT rc;
	GetClientRect(g_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// �f�o�C�X�ƃX���b�v �`�F�C���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));	// �\���̂̒l��������
	sd.BufferCount       = 3;		// �o�b�N �o�b�t�@��
	sd.BufferDesc.Width  = width;	// �o�b�N �o�b�t�@�̕�
	sd.BufferDesc.Height = height;	// �o�b�N �o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;  // ���t���b�V�� ���[�g(���q)
	sd.BufferDesc.RefreshRate.Denominator = 1; // ���t���b�V�� ���[�g(����)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N �o�b�t�@�̎g�p�@
	sd.OutputWindow = g_hWindow;	// �֘A�t����E�C���h�E
	sd.SampleDesc.Count = 1;		// �}���` �T���v���̐�
	sd.SampleDesc.Quality = 0;		// �}���` �T���v���̃N�I���e�B
	sd.Windowed = TRUE;				// �E�C���h�E ���[�h
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�����؂�ւ�

#if defined(DEBUG) || defined(_DEBUG)
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

	// �n�[�h�E�F�A�E�f�o�C�X���쐬
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
    if(FAILED(hr)) {
        // WARP�f�o�C�X���쐬
        hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
        if(FAILED(hr)) {
            // ���t�@�����X�E�f�o�C�X���쐬
            hr = D3D11CreateDeviceAndSwapChain(
                NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags,
                g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
                &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
                &g_pImmediateContext);
            if(FAILED(hr)) {
                return DXTRACE_ERR(L"InitDirect3D D3D11CreateDeviceAndSwapChain", hr);
            }
        }
    }

	// **********************************************************
	// ���_�o�b�t�@�@�̒�`
	D3D11_BUFFER_DESC xyzBufferDesc;
	xyzBufferDesc.Usage          = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	xyzBufferDesc.ByteWidth      = sizeof(XYZBuffer) * 8;    // 8���_
	xyzBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@
	xyzBufferDesc.CPUAccessFlags = 0;
	xyzBufferDesc.MiscFlags      = 0;
	xyzBufferDesc.StructureByteStride = 0;

	// ���_�o�b�t�@�@�̃T�u���\�[�X�̏����l(���_���W)
	struct XYZBuffer posVertex[] = {
		XMFLOAT3(-1.0f,1.0f,-1.0f), XMFLOAT3(1.0f,1.0f,-1.0f),
		XMFLOAT3(1.0f,-1.0f,-1.0f), XMFLOAT3(-1.0f,-1.0f,-1.0f),
		XMFLOAT3(-1.0f,1.0f,1.0f), XMFLOAT3(1.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,-1.0f,1.0f), XMFLOAT3(-1.0f,-1.0f,1.0f)
	};

	// ���_�o�b�t�@�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA xyzSubData;
	xyzSubData.pSysMem          = posVertex;  // �o�b�t�@�E�f�[�^�̏����l
	xyzSubData.SysMemPitch      = 0;
	xyzSubData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�@�̍쐬
	hr = g_pD3DDevice->CreateBuffer(&xyzBufferDesc, &xyzSubData, &g_pVerBuffer[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// ���_�o�b�t�@�A�̒�`
	D3D11_BUFFER_DESC colBufferDesc;
	colBufferDesc.Usage          = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	colBufferDesc.ByteWidth      = sizeof(ColBuffer) * 8;    // 8���_
	colBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@
	colBufferDesc.CPUAccessFlags = 0;
	colBufferDesc.MiscFlags      = 0;
	colBufferDesc.StructureByteStride = 0;

	// ���_�o�b�t�@�A�̃T�u���\�[�X�̏����l(���_�F)
	struct ColBuffer colVertex[] = {
		XMFLOAT3(0.0f,0.0f,0.0f), XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f), XMFLOAT3(0.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f), XMFLOAT3(1.0f,0.0f,1.0f),
		XMFLOAT3(1.0f,1.0f,0.0f), XMFLOAT3(1.0f,1.0f,1.0f)
	};

	// ���_�o�b�t�@�A�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA colSubData;
	colSubData.pSysMem          = colVertex;  // �o�b�t�@�̏����l
	colSubData.SysMemPitch      = 0;
	colSubData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�A�̃T�u���\�[�X�̍쐬
	hr = g_pD3DDevice->CreateBuffer(&colBufferDesc, &colSubData, &g_pVerBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// �C���f�b�N�X�E�o�b�t�@�̒�`
	D3D11_BUFFER_DESC idxBufferDesc;
	idxBufferDesc.Usage          = D3D11_USAGE_DEFAULT;     // �f�t�H���g�g�p�@
	idxBufferDesc.ByteWidth      = sizeof(UINT) * 36;       // 12�~3���_
	idxBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER; // �C���f�b�N�X�E�o�b�t�@
	idxBufferDesc.CPUAccessFlags = 0;
	idxBufferDesc.MiscFlags      = 0;
	idxBufferDesc.StructureByteStride = 0;

	// �C���f�b�N�X�E�o�b�t�@�̃T�u���\�[�X�̏����l(���_ID)
	UINT idxVertexID[] = {
		0,1,3,  1,2,3,  1,5,2,  5,6,2,  5,4,6,  4,7,6,
		4,5,0,  5,1,0,  4,0,7,  0,3,7,  3,2,7,  2,6,7
	};

	// �C���f�b�N�X�E�o�b�t�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA idxSubData;
	idxSubData.pSysMem          = idxVertexID;  // �o�b�t�@�E�f�[�^�̏����l
	idxSubData.SysMemPitch      = 0;
	idxSubData.SysMemSlicePitch = 0;

	// �C���f�b�N�X�E�o�b�t�@�̍쐬
	hr = g_pD3DDevice->CreateBuffer(&idxBufferDesc, &idxSubData, &g_pIdxBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// ���_�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"D3D11Sample05.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"VS",          // �uVS�֐��v���V�F�[�_������s�����
			"vs_4_0",      // ���_�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// ���_�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pVertexShader); // ���_�V�F�[�_���󂯎��ϐ�
//	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// ���͗v�f
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
	hr = g_pD3DDevice->CreateInputLayout(
			layout,                            // ��`�̔z��
			_countof(layout),                  // ��`�̗v�f��
			pBlobVS->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
			&g_pInputLayout);                  // �󂯎��ϐ��̃|�C���^
	SAFE_RELEASE(pBlobVS);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// �W�I���g���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobGS = NULL;
	hr = D3DX11CompileFromFile(
			L"D3D11Sample05.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"GS",          // �uGS�֐��v���V�F�[�_������s�����
			"gs_4_0",      // �W�I���g���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobGS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// �W�I���g���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobGS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pGeometryShader); // �W�I���g���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobGS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
			L"D3D11Sample05.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"PS",          // �uPS�֐��v���V�F�[�_������s�����
			"ps_4_0",      // �s�N�Z���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobPS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// �s�N�Z���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobPS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pPixelShader); // �s�N�Z���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobPS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// �萔�o�b�t�@�̒�`
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // ���I(�_�C�i�~�b�N)�g�p�@
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // �萔�o�b�t�@
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU���珑������
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;

	// �萔�o�b�t�@�@�̍쐬
	cBufferDesc.ByteWidth      = sizeof(cbNeverChanges); // �o�b�t�@�E�T�C�Y
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// �萔�o�b�t�@�A�̍쐬
	cBufferDesc.ByteWidth      = sizeof(cbChangesEveryFrame); // �o�b�t�@�E�T�C�Y
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// �萔�o�b�t�@�B�̍쐬
	cBufferDesc.ByteWidth      = sizeof(cbChangesEveryObject); // �o�b�t�@�E�T�C�Y
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[2]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable  = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable           = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3DDevice->CreateBlendState(&BlendState, &g_pBlendState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBlendState", hr);

	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // ���ʂɕ`�悷��
	RSDesc.CullMode = D3D11_CULL_NONE;    // ���ʂ�`�悷��
	RSDesc.FrontCounterClockwise = FALSE; // ���v��肪�\��
	RSDesc.DepthBias             = 0;
	RSDesc.DepthBiasClamp        = 0;
	RSDesc.SlopeScaledDepthBias  = 0;
	RSDesc.DepthClipEnable       = TRUE;
	RSDesc.ScissorEnable         = FALSE;
	RSDesc.MultisampleEnable     = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = g_pD3DDevice->CreateRasterizerState(&RSDesc, &g_pRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	// **********************************************************
	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL; // ��������
	DepthStencil.DepthFunc        = D3D11_COMPARISON_LESS; // ��O�̕��̂�`��
	DepthStencil.StencilEnable    = FALSE; // �X�e���V���E�e�X�g�Ȃ�
	DepthStencil.StencilReadMask  = 0;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0;     // �X�e���V���������݃}�X�N�B
			// �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_NEVER; // ��Ɏ��s
			// �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // ��ɐ���
	hr = g_pD3DDevice->CreateDepthStencilState(&DepthStencil,
											   &g_pDepthStencilState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateDepthStencilState", hr);

	// **********************************************************
	// �o�b�N �o�b�t�@�̏�����
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

	// IDXGIFactory�C���^�[�t�F�C�X�̎擾
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pFactory));
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateDXGIFactory", hr);

	// [Alt]+[Enter]�L�[�ɂ���ʃ��[�h�؂�ւ��@�\��ݒ肷��
	hr = g_pFactory->MakeWindowAssociation(
				g_hWindow, // ��ʃ��[�h�؂�ւ��@�\��ݒ肷��E�C���h�E �n���h��
				0);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pFactory->MakeWindowAssociation", hr);


	return hr;
}

/*--------------------------------------------
	�K���}�ݒ�
--------------------------------------------*/
HRESULT SetGamma(float gamma) {
	// �X���b�v �`�F�C����IDXGIOutput�C���^�[�t�F�C�X���擾
	IDXGIOutput* pOutput;
	HRESULT hr = g_pSwapChain->GetContainingOutput(&pOutput);
	if (FAILED(hr))
		return DXTRACE_ERR(L"SetGamma g_pSwapChain->GetContainingOutput", hr);	// ���s

	// �g�[���J�[�u�̐ݒ���擾
	DXGI_GAMMA_CONTROL_CAPABILITIES gammacap;
	hr = pOutput->GetGammaControlCapabilities(&gammacap);
	if (FAILED(hr)) {
		pOutput->Release();
		return DXTRACE_ERR(L"SetGamma pOutput->GetGammaControlCapabilities", hr);	// ���s
	}

	// �g�[���J�[�u��ݒ�
	float g = 1.0f / gamma;
	DXGI_GAMMA_CONTROL gammacontrol;
	gammacontrol.Scale.Red   = 1.0f;
	gammacontrol.Scale.Green = 1.0f;
	gammacontrol.Scale.Blue  = 1.0f;
	gammacontrol.Offset.Red   = 0.0f;
	gammacontrol.Offset.Green = 0.0f;
	gammacontrol.Offset.Blue  = 0.0f;
	for (UINT i = 0; i < gammacap.NumGammaControlPoints; ++i) {
		float L0 = gammacap.ControlPointPositions[i];
		float L1 = pow(L0, g);	// �K���}�J�[�u���v�Z
		gammacontrol.GammaCurve[i].Red   = L1;
		gammacontrol.GammaCurve[i].Green = L1;
		gammacontrol.GammaCurve[i].Blue  = L1;
	}
	hr = pOutput->SetGammaControl(&gammacontrol);
	pOutput->Release();
	if (FAILED(hr))
		return DXTRACE_ERR(L"SetGamma pOutput->SetGammaControl", hr);  // ���s

	return hr;
}

/*-------------------------------------------
	�o�b�N �o�b�t�@�̏�����(�o�b�N �o�b�t�@��`��^�[�Q�b�g�ɐݒ�)
--------------------------------------------*/
HRESULT InitBackBuffer(void)
{
	HRESULT hr;

    // �X���b�v�E�`�F�C������ŏ��̃o�b�N�E�o�b�t�@���擾����
    ID3D11Texture2D *pBackBuffer;  // �o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�C�X
    hr = g_pSwapChain->GetBuffer(
            0,                         // �o�b�N�E�o�b�t�@�̔ԍ�
            __uuidof(ID3D11Texture2D), // �o�b�t�@�ɃA�N�Z�X����C���^�[�t�F�C�X
            (LPVOID*)&pBackBuffer);    // �o�b�t�@���󂯎��ϐ�
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pSwapChain->GetBuffer", hr);  // ���s

	// �o�b�N�E�o�b�t�@�̏��
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

    // �o�b�N�E�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
    hr = g_pD3DDevice->CreateRenderTargetView(
            pBackBuffer,           // �r���[�ŃA�N�Z�X���郊�\�[�X
            NULL,                  // �`��^�[�Q�b�g�E�r���[�̒�`
            &g_pRenderTargetView); // �`��^�[�Q�b�g�E�r���[���󂯎��ϐ�
    SAFE_RELEASE(pBackBuffer);  // �ȍ~�A�o�b�N�E�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // ���s

	// �[�x/�X�e���V���E�e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
//	descDepth.Width     = descBackBuffer.Width;   // ��
//	descDepth.Height    = descBackBuffer.Height;  // ����
	descDepth.MipLevels = 1;       // �~�b�v�}�b�v�E���x����
	descDepth.ArraySize = 1;       // �z��T�C�Y
	descDepth.Format    = DXGI_FORMAT_D32_FLOAT;  // �t�H�[�}�b�g(�[�x�̂�)
//	descDepth.SampleDesc.Count   = 1;  // �}���`�T���v�����O�̐ݒ�
//	descDepth.SampleDesc.Quality = 0;  // �}���`�T���v�����O�̕i��
	descDepth.Usage          = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	descDepth.BindFlags      = D3D11_BIND_DEPTH_STENCIL; // �[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0;   // CPU����̓A�N�Z�X���Ȃ�
	descDepth.MiscFlags      = 0;   // ���̑��̐ݒ�Ȃ�
	hr = g_pD3DDevice->CreateTexture2D(
			&descDepth,         // �쐬����2D�e�N�X�`���̐ݒ�
			NULL,               // 
			&g_pDepthStencil);  // �쐬�����e�N�X�`�����󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateTexture2D", hr);  // ���s

	// �[�x/�X�e���V�� �r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format             = descDepth.Format;            // �r���[�̃t�H�[�}�b�g
	descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags              = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
			g_pDepthStencil,       // �[�x/�X�e���V���E�r���[�����e�N�X�`��
			&descDSV,              // �[�x/�X�e���V���E�r���[�̐ݒ�
			&g_pDepthStencilView); // �쐬�����r���[���󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateDepthStencilView", hr);  // ���s

    // �r���[�|�[�g�̐ݒ�
    g_ViewPort[0].TopLeftX = 0.0f;    // �r���[�|�[�g�̈�̍���X���W�B
    g_ViewPort[0].TopLeftY = 0.0f;    // �r���[�|�[�g�̈�̍���Y���W�B
    g_ViewPort[0].Width    = (FLOAT)descBackBuffer.Width;  // �r���[�|�[�g�̈�̕�
    g_ViewPort[0].Height   = (FLOAT)descBackBuffer.Height; // �r���[�|�[�g�̈�̍���
    g_ViewPort[0].MinDepth = 0.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
    g_ViewPort[0].MaxDepth = 1.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ő�l

	// �萔�o�b�t�@�@���X�V
	// �ˉe�ϊ��s��(�p�[�X�y�N�e�B�u(�����@)�ˉe)
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f),		// ����p30��
			(float)descBackBuffer.Width / (float)descBackBuffer.Height,	// �A�X�y�N�g��
			1.0f,							// �O�����e�ʂ܂ł̋���
			20.0f);							// ������e�ʂ܂ł̋���
	mat = XMMatrixTranspose(mat);
	XMStoreFloat4x4(&g_cbNeverChanges.Projection, mat);
	// �萔�o�b�t�@�@�̃}�b�v�擾
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[0],           // �}�b�v���郊�\�[�X
	                  0,                       // �T�u���\�[�X�̃C���f�b�N�X�ԍ�
	                  D3D11_MAP_WRITE_DISCARD, // �������݃A�N�Z�X
	                  0,                       //
	                  &MappedResource);        // �f�[�^�̏������ݐ�|�C���^
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // ���s
	// �f�[�^��������
	CopyMemory(MappedResource.pData, &g_cbNeverChanges, sizeof(cbNeverChanges));
	// �}�b�v����
	g_pImmediateContext->Unmap(g_pCBuffer[0], 0);

	//�T�C�Y��ۑ�
	g_sizeWindow.cx = descBackBuffer.Width;
	g_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/*--------------------------------------------
	��ʂ̕`�揈��
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

    // �`��^�[�Q�b�g�̃N���A
    g_pImmediateContext->ClearRenderTargetView(
                       g_pRenderTargetView, // �N���A����`��^�[�Q�b�g
                       g_ClearColor);         // �N���A����l

	// �[�x/�X�e���V���̃N���A
	g_pImmediateContext->ClearDepthStencilView(
			g_pDepthStencilView, // �N���A����[�x/�X�e���V���E�r���[
			D3D11_CLEAR_DEPTH,   // �[�x�l�������N���A����
			1.0f,                // �[�x�o�b�t�@���N���A����l
			0);                  // �X�e���V���E�o�b�t�@���N���A����l(���̏ꍇ�A���֌W)

	// ***************************************
	// �����̂̕`��

	// �萔�o�b�t�@�A���X�V
	// �r���[�ϊ��s��
	XMVECTORF32 eyePosition   = { 0.0f, 5.0f, -5.0f, 1.0f };  // ���_(�J�����̈ʒu)
	XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f, 1.0f };  // �����_
	XMVECTORF32 upDirection   = { 0.0f, 1.0f,  0.0f, 1.0f };  // �J�����̏����
	XMMATRIX mat = XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	XMStoreFloat4x4(&g_cbChangesEveryFrame.View, XMMatrixTranspose(mat));
	// �_�������W
	XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&g_vLightPos), mat);
	XMStoreFloat3(&g_cbChangesEveryFrame.Light, vec);
	// �萔�o�b�t�@�A�̃}�b�v�擾
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[1],           // �}�b�v���郊�\�[�X
	                  0,                       // �T�u���\�[�X�̃C���f�b�N�X�ԍ�
	                  D3D11_MAP_WRITE_DISCARD, // �������݃A�N�Z�X
	                  0,                       //
	                  &MappedResource);        // �f�[�^�̏������ݐ�|�C���^
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // ���s
	// �f�[�^��������
	CopyMemory(MappedResource.pData, &g_cbChangesEveryFrame, sizeof(cbChangesEveryFrame));
	// �}�b�v����
	g_pImmediateContext->Unmap(g_pCBuffer[1], 0);

	// �萔�o�b�t�@�B���X�V
	// ���[���h�ϊ��s��
	XMMATRIX matY, matX;
	FLOAT rotate = (FLOAT)(XM_PI * (timeGetTime() % 3000)) / 1500.0f;
	matY = XMMatrixRotationY(rotate);
	rotate = (FLOAT)(XM_PI * (timeGetTime() % 1500)) / 750.0f;
	matX = XMMatrixRotationX(rotate);
	XMStoreFloat4x4(&g_cbChangesEveryObject.World, XMMatrixTranspose(matY * matX));
	// �萔�o�b�t�@�B�̃}�b�v�擾
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[2],           // �}�b�v���郊�\�[�X
	                  0,                       // �T�u���\�[�X�̃C���f�b�N�X�ԍ�
	                  D3D11_MAP_WRITE_DISCARD, // �������݃A�N�Z�X
	                  0,                       //
	                  &MappedResource);        // �f�[�^�̏������ݐ�|�C���^
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // ���s
	// �f�[�^��������
	CopyMemory(MappedResource.pData, &g_cbChangesEveryObject, sizeof(cbChangesEveryObject));
	// �}�b�v����
	g_pImmediateContext->Unmap(g_pCBuffer[2], 0);

	// ***************************************
	// IA�ɒ��_�o�b�t�@��ݒ�
	UINT strides[2] = { sizeof(XYZBuffer), sizeof(ColBuffer) };
	UINT offsets[2] = { 0, 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 2, g_pVerBuffer, strides, offsets);
	// IA�ɃC���f�b�N�X�E�o�b�t�@��ݒ�
	g_pImmediateContext->IASetIndexBuffer(g_pIdxBuffer, DXGI_FORMAT_R32_UINT, 0);
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	g_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// VS�ɒ��_�V�F�[�_��ݒ�
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	// VS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->VSSetConstantBuffers(0, 3, g_pCBuffer);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	g_pImmediateContext->GSSetShader(g_pGeometryShader, NULL, 0);
	// GS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->GSSetConstantBuffers(0, 3, g_pCBuffer);

	// RS�Ƀr���[�|�[�g��ݒ�
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);
	// RS�Ƀ��X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->RSSetState(g_pRasterizerState);

	// PS�Ƀs�N�Z���E�V�F�[�_��ݒ�
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	// PS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->PSSetConstantBuffers(0, 3, g_pCBuffer);

	// OM�ɕ`��^�[�Q�b�g �r���[�Ɛ[�x/�X�e���V���E�r���[��ݒ�
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_bDepthMode ? g_pDepthStencilView : NULL);
	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xffffffff);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	// ***************************************
	// ���_�o�b�t�@�̃f�[�^���C���f�b�N�X�E�o�b�t�@���g���ĕ`�悷��
	g_pImmediateContext->DrawIndexed(
			36, // �`�悷��C���f�b�N�X��(���_��)
			0,  // �C���f�b�N�X�E�o�b�t�@�̍ŏ��̃C���f�b�N�X����`��J�n
			0); // ���_�o�b�t�@���̍ŏ��̒��_�f�[�^����g�p�J�n

	// ***************************************
	// �o�b�N �o�b�t�@�̕\��
	hr = g_pSwapChain->Present(	0,	// ��ʂ𒼂��ɍX�V����
								0);	// ��ʂ����ۂɍX�V����

	return hr;
}

/*-------------------------------------------
	Direct3D�̏I������
--------------------------------------------*/
bool CleanupDirect3D(void)
{
    // �f�o�C�X�E�X�e�[�g�̃N���A
    if(g_pImmediateContext)
        g_pImmediateContext->ClearState();

	// �X���b�v �`�F�C�����E�C���h�E ���[�h�ɂ���
	if (g_pSwapChain)
		g_pSwapChain->SetFullscreenState(FALSE, NULL);

	// �擾�����C���^�[�t�F�C�X�̊J��
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pBlendState);
	SAFE_RELEASE(g_pRasterizerState);

	SAFE_RELEASE(g_pCBuffer[2]);
	SAFE_RELEASE(g_pCBuffer[1]);
	SAFE_RELEASE(g_pCBuffer[0]);

	SAFE_RELEASE(g_pInputLayout);

	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pGeometryShader);
	SAFE_RELEASE(g_pVertexShader);

	SAFE_RELEASE(g_pIdxBuffer);
	SAFE_RELEASE(g_pVerBuffer[1]);
	SAFE_RELEASE(g_pVerBuffer[0]);

	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencil);

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);

    SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pFactory);

	return true;
}

/*-------------------------------------------
	�A�v���P�[�V�����̏I������
--------------------------------------------*/
bool CleanupApp(void)
{
	// �E�C���h�E �N���X�̓o�^����
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/*-------------------------------------------
	�E�B���h�E����
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;
	BOOL fullscreen;

	switch(msg)
	{
	case WM_DESTROY:
		// Direct3D�̏I������
		CleanupDirect3D();
		// �E�C���h�E�����
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// �E�C���h�E �T�C�Y�̕ύX����
	case WM_SIZE:
		if (!g_pD3DDevice || wParam == SIZE_MINIMIZED)
			break;
		// �`��^�[�Q�b�g����������
		g_pImmediateContext->OMSetRenderTargets(0, NULL, NULL);	// �`��^�[�Q�b�g�̉���
		SAFE_RELEASE(g_pRenderTargetView);					    // �`��^�[�Q�b�g �r���[�̉��
		SAFE_RELEASE(g_pDepthStencilView);					// �[�x/�X�e���V�� �r���[�̉��
		SAFE_RELEASE(g_pDepthStencil);						// �[�x/�X�e���V�� �e�N�X�`���̉��

		// �o�b�t�@�̕ύX
		g_pSwapChain->ResizeBuffers(3, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// �o�b�N �o�b�t�@�̏�����
		InitBackBuffer();
		break;

	case WM_KEYDOWN:
		// �L�[���͂̏���
		switch(wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�ŃE�C���h�E�����
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_F2:		// [F2]�L�[�Ő[�x�o�b�t�@�̃��[�h��؂�ւ���
			g_bDepthMode = !g_bDepthMode;
			break;

		case VK_F5:		// [F5]�L�[�ŉ�ʃ��[�h��؂�ւ���
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
			break;

		case VK_F6:		// [F6]�L�[�Ńf�B�X�v���C ���[�h��ݒ肷��
			if (g_pSwapChain != NULL) {
				// �f�B�X�v���C ���[�h�̕ύX
				DXGI_MODE_DESC desc;
				desc.Width  = 800;
				desc.Height = 600;
				desc.RefreshRate.Numerator   = 60;
				desc.RefreshRate.Denominator = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				hr = g_pSwapChain->ResizeTarget(&desc);
				if (FAILED(hr))
					DXTRACE_ERR(L"MainWndProc g_pSwapChain->ResizeTarget", hr);  // ���s
			}
			break;

		case '1':
			SetGamma(0.5f);	// �K���}0.5�ɐݒ�
			break;

		case '2':
			SetGamma(1.0f);	// �K���}1.0�ɐݒ�
			break;

		case '3':
			SetGamma(2.0f);	// �K���}2.0�ɐݒ�
			break;
		}
		break;
	}

	// �f�t�H���g����
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*--------------------------------------------
	�f�o�C�X�̏�������
--------------------------------------------*/
HRESULT IsDeviceRemoved(void)
{
	HRESULT hr;

	// �f�o�C�X�̏����m�F
	hr = g_pD3DDevice->GetDeviceRemovedReason();
	switch (hr) {
	case S_OK:
		break;         // ����

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		CleanupDirect3D();   // Direct3D�̉��(�A�v���P�[�V������`)
		hr = InitDirect3D();  // Direct3D�̏�����(�A�v���P�[�V������`)
		if (FAILED(hr))
			return hr; // ���s�B�A�v���P�[�V�������I��
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
	case DXGI_ERROR_INVALID_CALL:
	default:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		return hr;   // �ǂ����悤���Ȃ��̂ŁA�A�v���P�[�V�������I���B
	};

	return S_OK;         // ����
}

/*--------------------------------------------
	�A�C�h�����̏���
--------------------------------------------*/
bool AppIdle(void)
{
	if (!g_pD3DDevice)
		return false;

	HRESULT hr;
	// �f�o�C�X�̏�������
	hr = IsDeviceRemoved();
	if (FAILED(hr))
		return false;

	// �X�^���o�C ���[�h
	if (g_bStandbyMode) {
		hr = g_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		if (hr != S_OK) {
			Sleep(100);	// 0.1�b�҂�
			return true;
		}
		g_bStandbyMode = false; // �X�^���o�C ���[�h����������
	}

	// ��ʂ̍X�V
	hr = Render();
	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;  // �X�^���o�C ���[�h�ɓ���
	}

	return true;
}

/*--------------------------------------------
	���C��
---------------------------------------------*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// �f�o�b�O �q�[�v �}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// XNA Math���C�u�����̃T�|�[�g �`�F�b�N
    if (XMVerifyCPUSupport() != TRUE)
	{
		DXTRACE_MSG(L"WinMain XMVerifyCPUSupport");
		return 0;
	}

	// �A�v���P�[�V�����Ɋւ��鏉����
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"WinMain InitApp", hr);
		return 0;
	}

	// Direct3D�̏�����
	hr = InitDirect3D();
	if (FAILED(hr)) {
		DXTRACE_ERR(L"WinMain InitDirect3D", hr);
		CleanupDirect3D();
		CleanupApp();
		return 0;
	}

	// ���b�Z�[�W ���[�v
	MSG msg;
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �A�C�h������
			if (!AppIdle())
				// �G���[������ꍇ�C�A�v���P�[�V�������I������
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	// �A�v���P�[�V�����̏I������
	CleanupApp();

	return (int)msg.wParam;
}
