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

	// ��� ������ ��Ʈ �� �ϳ��� �����ϰ�, ���õ��� �ʾ����� �⺻��(ù ��° ��Ʈ)�� ����Ѵ�.
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

	// ��Ʈ �����Ͱ� ���Ե� �ؽ�Ʈ ������ �ε��Ѵ�.
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// ��Ʈ ���ڵ��� ���Ե� �ؽ�ó�� �ε��Ѵ�.
	result = LoadTexture(device, deviceContext, fontTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	// ��Ʈ �ؽ�ó�� �����Ѵ�.
	ReleaseTexture();

	// ��Ʈ �����͸� �����Ѵ�.
	ReleaseFontData();

	return;
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	// ��Ʈ ���� �����͸� ������ ���۸� �����Ѵ�.
	m_Font = new FontType[95];

	// ���� ũ�� �� ���� �� ���� ������ �д´�.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// �ؽ�Ʈ���� ���Ǵ� 95���� ASCII ���ڸ� �о���δ�.
	for (i = 0; i < 95; i++)
	{
		// ������ ������ ���� ���� ���ڵ� ��ŵ
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

		// ���� �ϳ��� ���� ���� �ؽ�ó ��ǥ, ���� �ؽ�ó ��ǥ, ���� ũ�� ������ �д´�.
		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// ������ �ݴ´�.
	fin.close();

	return true;
}



void FontClass::ReleaseFontData()
{
	// ��Ʈ ������ �迭�� �����Ѵ�.
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

	// ��Ʈ �ؽ�ó ��ü�� �����ϰ� �ʱ�ȭ�Ѵ�.
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
	// �ؽ�ó ��ü�� �����Ѵ�.
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
	// �ؽ�ó �ڿ� �並 ��ȯ�Ѵ�.
	return m_Texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// �Էµ� vertices �����͸� VertexType ����ü�� ���� ��ȯ�Ѵ�.
	vertexPtr = (VertexType*)vertices;

	// ���ڿ��� ���� ���� ���Ѵ�.
	numLetters = (int)strlen(sentence);

	// ���� �迭�� �ֱ� ���� �ε����� �ʱ�ȭ�Ѵ�.
	index = 0;

	// ���ڿ��� �� ���ڸ� ����(�簢��) ���·� �׸���.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32; // ASCII ���� ' ' (32)���� ����

		// ���� ������ ��쿡�� ��ġ�� ���������� 3�ȼ� �̵�
		if (letter == 0)
		{
			drawX = drawX + m_spaceSize;
		}
		else
		{
			// ù ��° �ﰢ�� (������ ���κ�)
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // �»��
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // ���ϴ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // ���ϴ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// �� ��° �ﰢ�� (������ �Ʒ��κ�)
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // �»��
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // ����
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // ���ϴ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// ���� ���ڸ� �׸��� ���� X ��ġ�� �̵� (���� �ʺ� + 1�ȼ� ����)
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}


int FontClass::GetSentencePixelLength(char* sentence)
{
	int pixelLength, numLetters, i, letter;

	pixelLength = 0;
	numLetters = (int)strlen(sentence);  // ���ڿ��� ���̸� ���Ѵ�

	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;  // ASCII ���� ����(' ')�� 0���� ����

		// ���� ���ڰ� �����̶�� 3�ȼ��� ����Ѵ�
		if (letter == 0)
		{
			pixelLength += m_spaceSize;
		}
		else
		{
			// ������ �ʺ� + ���� �� ����(1�ȼ�)�� ���Ѵ�
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
	m_fontHeight = newSize;  // ���ο� ��Ʈ ũ�� ����
}

float FontClass::GetFontSize()
{
	return m_fontHeight;
}
