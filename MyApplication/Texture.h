#pragma once

class Texture
{
public:
	Texture() {}
	Texture(const char* name)
	{
		m_id = LoadTexture(name);
	}

	static unsigned int LoadTexture(const char* name);

	void SetID(unsigned int _id) { m_id = _id; }
	unsigned int GetID() { return m_id; }

private:
	unsigned int m_id;
};
