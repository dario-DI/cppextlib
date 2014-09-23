#include <assert.h>
#include <algorithm>
#include <string>

#include <cex/String.h>

namespace cex {

	class String : public IString
	{
	public:
		String(){}

		String(const String& other) : _str(other.data()){}

		String(const char* str) : _str(str){}

		const char* assign(const char* data, size_t len)
		{
			_str.assign(data, len);
			return _str.c_str();
		}

		const char* assign(const char* data)
		{
			_str.assign(data);
			return _str.c_str();
		}

		void clear()
		{
			_str.clear();
		}


		const char* data() const
		{ 
			return _str.data();
		}

		const char* c_str() const
		{
			return _str.c_str();
		}


		size_t length() { return _str.length(); }

		char at(size_t index) { return _str.at(index); }


		void resize(size_t size)
		{
			return _str.resize(size);
		}


		size_t find(char s)
		{
			return _str.find(s);
		}

		size_t find(const char* str)
		{
			return _str.find(str);
		}

		int compare(const char* str)
		{
			return _str.compare(str);
		}

		IString* append(IString* str)
		{
			_str.append(str->data());
			return this;
		}

		std::shared_ptr<IString> substr(size_t start, size_t counter)
		{
			std::shared_ptr<IString> istr = std::make_shared<String>();
			istr->assign(_str.substr(start, counter).data());
			return istr;
		}


	protected:

		std::string _str;
	};


	REGIST_DELTA_CREATOR(IString, String);

}