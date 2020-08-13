#include "utils.h"

/*static*/std::string Utils::trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
	{
		return ""; // no content
	}

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

/*static*/bool Utils::icasecmp(const std::string& l, const std::string& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::string::value_type l1, std::string::value_type r1)
			{ return toupper(l1) == toupper(r1); });
}

/*static*/std::uint8_t Utils::HexCharToInt(char c)
{
	switch (c)
	{
	case '0':return 0;
	case '1':return 1;
	case '2':return 2;
	case '3':return 3;
	case '4':return 4;
	case '5':return 5;
	case '6':return 6;
	case '7':return 7;
	case '8':return 8;
	case '9':return 9;
	case 'A':case 'a':return 10;
	case 'B':case 'b':return 11;
	case 'C':case 'c':return 12;
	case 'D':case 'd':return 13;
	case 'E':case 'e':return 14;
	case 'F':case 'f':return 15;
	}

	return 0xff;
}