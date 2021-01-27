#ifndef _STRUCT_DATAS_
#define _STRUCT_DATAS_

template<typename T>
struct Struct_Datas
{
	T** p;
	int pos = 0;

	void push(unsigned char* buffer)
	{
		p[pos++] = new T(buffer);
	}

	Struct_Datas(int pack_num)
	{
		p = new T * [pack_num];
	}

	~Struct_Datas()
	{
		for (int i = 0; i < pos; ++i)
		{
			if (p[i] != nullptr)
				delete p[i];
		}
		delete[] p;
	}
};

#endif
