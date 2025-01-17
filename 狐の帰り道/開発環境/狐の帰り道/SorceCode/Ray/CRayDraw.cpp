#include "CRayDraw.h"

//シェーダーファイル名(ディレクトリも含む).
const char SHADER_NAME[] = "Data\\Shader\\Ray.hlsl";

CRayDraw::CRayDraw()
	: m_pVertexShader(nullptr)	
	, m_pVertexLayout(nullptr)	
	, m_pPixelShader(nullptr)	
	, m_pConstantBuffer(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_Ray()
{
}

CRayDraw::~CRayDraw()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}

//初期化.
HRESULT CRayDraw::Init(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11)
{
	m_pDevice11 = pDevice11;
	m_pContext11 = pContext11;

	if (FAILED(InitShader()))
	{
		return E_FAIL;
	}
	if (FAILED(InitModel()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//==================================================================.
//	HLSLファイルを読み込みシェーダを作成する.
//	HLSL : High Level Shading Language の略.
//==================================================================.
HRESULT CRayDraw::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT	  uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	//#ifdef _DEBUG.

	//HLSLからバーテックスシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		//シェーダファイル名(HLSLファイル).
			nullptr,			//マクロ定義の配列へのポインタ(未使用).
			nullptr,			//インクルードファイルを扱うインターフェイスへのポインタ(未使用).
			"VS_Main",			//シェーダエントリーポイント関数の名前.				//""はプロパティの設定項目と一緒.
			"vs_5_0",			//シェーダのモデルを指定する文字列(プロファイル).
			uCompileFlag,		//シェーダコンパイルフラグ.
			0,					//エフェクトコンパイルフラグ(未使用).
			nullptr,			//スレッドポンプインターフェイスへのポインタ(未使用).
			&pCompiledShader,	//ブロブを格納するメモリへのポインタ.
			&pErrors,			//エラーと警告一覧を格納するメモリへのポインタ.
			nullptr)))			//戻り値へのポインタ(未使用).
	{
		_ASSERT_EXPR(false, L"hlsl読み込み失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「バーテックスシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader)))	//(out)バーテックスシェーダ.
	{
		_ASSERT_EXPR(false, L"バーテックスシェーダ作成失敗");
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						//位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	//DXGIのフォーマット(32bit float型*3).
			0,
			0,								//データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))	//(out)頂点インプットレイアウト.
	{
		_ASSERT_EXPR(false, L"頂点インプットレイアウト作成失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSLからピクセルシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		//シェーダファイル名(HLSLファイル).
			nullptr,			//マクロ定義の配列へのポインタ(未使用).
			nullptr,			//インクルードファイルを扱うインターフェイスへのポインタ(未使用).
			"PS_Main",			//シェーダエントリーポイント関数の名前.
			"ps_5_0",			//シェーダのモデルを指定する文字列(プロファイル).
			uCompileFlag,		//シェーダコンパイルフラグ.
			0,					//エフェクトコンパイルフラグ(未使用).
			nullptr,			//スレッドポンプインターフェイスへのポインタ(未使用).
			&pCompiledShader,	//ブロブを格納するメモリへのポインタ.
			&pErrors,			//エラーと警告一覧を格納するメモリへのポインタ.
			nullptr)))			//戻り値へのポインタ(未使用).
	{
		_ASSERT_EXPR(false, L"hlsl読み込み失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「ピクセルシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader)))	//(out)ピクセルシェーダ.
	{
		_ASSERT_EXPR(false, L"ピクセルシェーダ作成失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//コンスタント(定数)バッファ作成.
	//シェーダに特定の数値を送るバッファ.
	//ここでは変換行列渡し用.
	//シェーダに World, View, Projection 行列を渡す.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//コンスタントバッファを指定.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);	//コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//書き込みでアクセス.
	cb.MiscFlags = 0;								//その他のフラグ(未使用).
	cb.StructureByteStride = 0;								//構造体のサイズ().
	cb.Usage = D3D11_USAGE_DYNAMIC;				//使用方法：直接書き込み.

	//コンスタントバッファの作成.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer)))
	{
		_ASSERT_EXPR(false, L"コンスタントバッファ作成失敗");
		return E_FAIL;
	}

	return S_OK;
}


//レイ初期化.
HRESULT CRayDraw::InitModel()
{
	//レイのバーテックバッファ作成.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX)*2;	//2点分.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Ray.Point;	//レイの座標をセット.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		_ASSERT_EXPR(false, "バーテックスバッファ作成失敗");
		return E_FAIL;
	}
	return S_OK;
}

//描画.
void CRayDraw::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	//ワールド行列.
	D3DXMATRIX mWorld;
	//回転行列,　移動行列.
	D3DXMATRIX mRot, mTran;

	//回転行列.
	D3DXMatrixRotationY(&mRot, m_Ray.fYaw);

	//平行移動.
	D3DXMatrixTranslation(&mTran, m_Ray.Pos.x, m_Ray.Pos.y, m_Ray.Pos.z);

	//使用するシェーダの登録.
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);

	//ワールド行列.
	mWorld = mRot * mTran;

	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE	pData;
	SHADER_CONSTANT_BUFFER		cb;		//コンスタントバッファ.
	//バッファ内のデータの書き換え開始時にmap.
	if (SUCCEEDED(
		m_pContext11->Map(m_pConstantBuffer,
			0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド，ビュー，プロジェクション行列を渡す.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);			//行列を転置する.
		cb.mWVP = m;

		//カラー.
		cb.vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	//このコンスタントバッファをどのシェーダで使うか？.
	//registarのb2の2と連動している.
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//頂点バッファをセット.
	UINT stride = sizeof(VERTEX);	//データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout(m_pVertexLayout);
	//プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	//プリミティブをレンダリング.
	m_pContext11->Draw(2, 0);	//線分(頂点2つ分).
}
