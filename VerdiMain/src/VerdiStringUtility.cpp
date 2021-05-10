
#include "VerdiStringUtility.h"
#include "VerdiVec4.h"

namespace Verdi {

	String StringUtility::toString(const Exception& e)
	{
		String s = String(typeid(e).name()) + " : " + e.getDescription() + ", in File: " + e.getSourceFile();
		s += ", at Line: " + toString(e.getLine());
		return s;
	}

	String StringUtility::toString(int a)
	{
		std::stringstream ss;
		ss << a;
		return ss.str();
	}

	String StringUtility::toString(uint32_t a)
	{
		std::stringstream ss;
		ss << a;
		return ss.str();
	}

	String StringUtility::toString(const Vec3& v)
	{
		std::stringstream ss;
		ss << v.x << "," << v.y << "," << v.z;
		return ss.str();
	}

	int StringUtility::toInt(const String& str)
	{
		int i = 0;
		sscanf(str.c_str(),"%d",&i);
		return i;
	}

	String StringUtility::toString(const Vec4& v)
	{
		std::stringstream ss;
		ss << v.x << "," << v.y << "," << v.z << "," << v.w;
		return ss.str();
	}

	String StringUtility::toString(float a) 
	{
		std::stringstream ss;
		ss << a;
		return ss.str();
	}

	float StringUtility::toFloat(const String& str)
	{
		float f = 0.0f;

		sscanf(str.c_str(),"%f",&f);

		return f;
	}
	Vec3 StringUtility::toVec3(const String& str)
	{
		Vec3 ret = Vec3(0.0f,0.0f,0.0f);

		sscanf(str.c_str(),"%f,%f,%f",
			&ret.x,&ret.y,&ret.z);

		return ret;
	}

	Vec4 StringUtility::toVec4(const String& str)
	{
		Vec4 ret;

		sscanf(str.c_str(),"%f,%f,%f,%f",
			&ret.x,&ret.y,&ret.z,&ret.w);

		return ret;
	}

	void StringUtility::split(const String& str, const char delim, StringVector& sub)
	{
		String accum;
		accum.clear();

		if(str.size() < 1)
			return;

		for(size_t i = 0; i < str.size(); ++i) {
			accum += str[i];
			if(str[i] == delim || i == str.size()-1) {
				sub.push_back(accum);
				accum.clear();
			}
		}
	}
}