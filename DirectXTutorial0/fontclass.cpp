///////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"


FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}


bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int fontChoice)
{
	char fontFilename[128];
	char fontTextureFilename[128];
	bool result;

	// 사용 가능한 폰트 중 하나를 선택하고, 선택되지 않았으면 기본값(첫 번째 폰트)을 사용한다.
	switch (fontChoice)
	{
	case 0:
	{
		strcpy_s(fontFilename, "../Engine/data/font/font01.txt");
		strcpy_s(fontTextureFilename, "../Engine/data/font/font01.tga");
		m_fontHeight = 32.0f;
		m_spaceSize = 3;
		break;
	}
	default:
	{
		strcpy_s(fontFilename, "../Engine/data/font/font01.txt");
		strcpy_s(fontTextureFilename, "../Engine/data/font/font01.tga");
		m_fontHeight = 32.0f;
		m_spaceSize = 3;
		break;
	}
	}

	// 폰트 데이터가 포함된 텍스트 파일을 로드한다.
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// 폰트 문자들이 포함된 텍스처를 로드한다.
	result = LoadTexture(device, deviceContext, fontTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	// 폰트 텍스처를 해제한다.
	ReleaseTexture();

	// 폰트 데이터를 해제한다.
	ReleaseFontData();

	return;
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// 폰트 간격 데이터를 저장할 버퍼를 생성한다.
	m_Font = new FontType[95];

	// 문자 크기 및 문자 간 간격 정보를 읽는다.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// 텍스트에서 사용되는 95개의 ASCII 문자를 읽어들인다.
	for (i = 0; i < 95; i++)
	{
		// 문자의 정렬을 위한 구분 문자들 스킵
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		// 문자 하나에 대한 좌측 텍스처 좌표, 우측 텍스처 좌표, 문자 크기 정보를 읽는다.
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// 파일을 닫는다.
	fin.close();

	return true;
}



void FontClass::ReleaseFontData()
{
	// 폰트 데이터 배열을 해제한다.
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}

	return;
}


bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// 폰트 텍스처 객체를 생성하고 초기화한다.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}



void FontClass::ReleaseTexture()
{
	// 텍스처 객체를 해제한다.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}



ID3D11ShaderResourceView* FontClass::GetTexture()
{
	// 텍스처 자원 뷰를 반환한다.
	return m_Texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// 입력된 vertices 포인터를 VertexType 구조체로 강제 변환한다.
	vertexPtr = (VertexType*)vertices;

	// 문자열의 글자 수를 구한다.
	numLetters = (int)strlen(sentence);

	// 정점 배열에 넣기 위한 인덱스를 초기화한다.
	index = 0;

	// 문자열의 각 문자를 쿼드(사각형) 형태로 그린다.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32; // ASCII 기준 ' ' (32)부터 시작

		// 공백 문자인 경우에는 위치만 오른쪽으로 3픽셀 이동
		if (letter == 0)
		{
			drawX = drawX + m_spaceSize;
		}
		else
		{
			// 첫 번째 삼각형 (쿼드의 윗부분)
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 좌상단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // 우하단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // 좌하단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// 두 번째 삼각형 (쿼드의 아랫부분)
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 좌상단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // 우상단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // 우하단
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// 다음 문자를 그리기 위해 X 위치를 이동 (문자 너비 + 1픽셀 간격)
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}


int FontClass::GetSentencePixelLength(char* sentence)
{
	int pixelLength, numLetters, i, letter;

	pixelLength = 0;
	numLetters = (int)strlen(sentence);  // 문자열의 길이를 구한다

	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;  // ASCII 기준 공백(' ')을 0으로 맞춤

		// 만약 문자가 공백이라면 3픽셀로 계산한다
		if (letter == 0)
		{
			pixelLength += m_spaceSize;
		}
		else
		{
			// 문자의 너비 + 문자 간 간격(1픽셀)을 더한다
			pixelLength += (m_Font[letter].size + 1);
		}
	}

	return pixelLength;
}



int FontClass::GetFontHeight()
{
	return (int)m_fontHeight;
}

void FontClass::SetFontSize(float newSize)
{
	m_fontHeight = newSize;  // 새로운 폰트 크기 적용
}

float FontClass::GetFontSize()
{
	return m_fontHeight;
}
