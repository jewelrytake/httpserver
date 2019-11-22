#include <URL.hpp>
#include <limits.h>
#include <string_view>

namespace
{
	void ProcessSpecialCase(std::string_view& str, std::string& out, uint8_t& flag, const char* delims = "/?#:");
	void ProcessSpecialCase(std::string_view& str, std::vector< std::string >& out, uint8_t& flag, const char* delims = "/?#:");
	void ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, uint8_t& mainFlag, const char* delims = "/?#:");
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, size_t& count);
}

namespace URL
{
	URL::~URL() = default;

	struct URL::URLNode
	{
		std::string m_scheme;
		std::string m_userInfo;
		std::string m_domain;
		uint16_t m_port = 0;
		std::vector< std::string > m_path;
		std::string m_query;
		std::string m_fragment;
	};

	URL::URL() : m_node(new URLNode) {}

	std::string URL::GetScheme() const
	{
		return m_node->m_scheme;
	}

	std::string URL::GetUserInfo() const
	{
		return m_node->m_userInfo;
	}

	std::string URL::GetDomain() const
	{
		return m_node->m_domain;
	}

	uint16_t URL::GetPort() const
	{
		return m_node->m_port;
	}

	std::vector<std::string> URL::GetPath() const
	{
		return m_node->m_path;
	}

	std::string URL::GetQuery() const
	{
		return m_node->m_query;
	}

	std::string URL::GetFragment() const
	{
		return m_node->m_fragment;
	}
	
	bool URL::ParseURL(const std::string& url)
	{
		m_node->m_port = 0;
		std::string_view startString = url;

		size_t schemeDelimiter = startString.find(':');
		uint8_t mainFlag = 0;
		std::string_view schemeOrDomain = startString.substr(0, schemeDelimiter);

		size_t notSchemeDelimiter = schemeOrDomain.find_first_of("./@");
		if (notSchemeDelimiter == std::string::npos)
		{
			m_node->m_scheme = schemeOrDomain;
			startString.remove_prefix(schemeDelimiter);

			if (auto authority = startString.substr(0, 3); authority == "://")
			{
				startString.remove_prefix(authority.length());
				if (size_t userDelim = startString.find('@'); userDelim != std::string::npos)
				{
					m_node->m_userInfo = startString.substr(0, userDelim);
					ParseRestString(startString, userDelim, m_node, mainFlag);
				}
			}
			else if (startString[0] == ':')
			{
				startString.remove_prefix(1);
				if (size_t userDelim = startString.find('@'); userDelim != std::string::npos)
				{
					m_node->m_userInfo = startString.substr(0, userDelim);
					std::string tmp = startString.data();
					tmp[userDelim] = ':';
					std::string_view rest = tmp;
					ParseRestString(rest, userDelim, m_node, mainFlag);
					if (mainFlag == 1)
						return true;
				}
			}
		}
		else if (notSchemeDelimiter != std::string::npos)
		{
			m_node->m_scheme.clear();
			size_t domainDelim = schemeOrDomain.find_first_of(":/?");
			m_node->m_domain = schemeOrDomain.substr(0, domainDelim);
			schemeOrDomain.remove_prefix(domainDelim);
		}



		//-----------------------------------------------
		//if (startString[schemeDelimiter + 1] == '/' && startString[schemeDelimiter + 2] == '/')
		//{
		//	m_node->m_scheme = startString.substr(0, schemeDelimiter);
		//	startString.remove_prefix(schemeDelimiter + 3);
  //    		size_t delimPos = startString.find_first_of(":/?=#@");
		//	if (delimPos == std::string::npos)
		//	{
		//		m_node->m_domain = startString;
		//	}
		//	else
		//	{
		//		if (startString[delimPos] == ':')
		//		{
		//			size_t portLength = 0;
		//			ParseUint16(startString, m_node->m_port, portLength);
		//			ParseRestString(startString, delimPos + portLength + 1, m_node, mainFlag);
		//			if (mainFlag == 1)
		//				return true;
		//		}
		//		else if (startString[delimPos] == '@')
		//		{
		//			m_node->m_userInfo = startString.substr(0, delimPos);
		//			m_node->m_domain = startString.substr(0, delimPos);
		//			ParseRestString(startString, delimPos + 1, m_node, mainFlag);
		//			if (mainFlag == 1)
		//				return true;
		//		}
		//		else
		//		{
		//			m_node->m_domain = startString.substr(0, delimPos);
		//			ParseRestString(startString, delimPos, m_node, mainFlag);
		//			if (mainFlag == 1)
		//				return true;
		//		}
		//	}
		//}
		//else
		//{
		//	// example - "urn:example:animal:ferret:nose"
		//	std::string_view schemeString = startString.substr(0, schemeDelimiter);
		//	if (schemeString.find_first_of("/.") == std::string::npos)
		//	{
		//		m_node->m_scheme = schemeString;
		//		m_node->m_domain.clear();
		//		ParseRestString(startString, schemeDelimiter, m_node, mainFlag);
		//		if (mainFlag == 1)
		//			return true;
		//	}
		//	else
		//	{
		//		m_node->m_scheme.clear();
		//		size_t domainEnd = startString.find_first_of(":/");
		//		if (domainEnd == std::string::npos)
		//		{
		//			m_node->m_domain = startString;
		//		}
		//		else
		//		{
		//			m_node->m_domain = startString.substr(0, domainEnd);
		//			if (startString[domainEnd] == ':')
		//			{
		//				size_t portLength = 0;
		//				ParseUint16(startString, m_node->m_port, portLength);
		//				ParseRestString(startString, domainEnd + portLength + 1, m_node, mainFlag);
		//				if (mainFlag == 1)
		//					return true;
		//			}
		//			else
		//			{
		//				ParseRestString(startString, domainEnd, m_node, mainFlag);
		//				if (mainFlag == 1)
		//					return true;
		//			}
		//		}
		//	}
		//}
		return true;
	}
}

namespace
{
	void ParseRestString(std::string_view& startString, size_t removeIndex, std::shared_ptr< URL::URL::URLNode> node, uint8_t& mainFlag, const char* delims)
	{
		startString.remove_prefix(removeIndex);
		if (startString.empty())
			mainFlag = 1;
		uint8_t flag;
		for (;;)
		{
			flag = 0;
			if (startString.find_first_not_of(delims) != std::string::npos)
			{
				ProcessSpecialCase(startString, node->m_domain, flag);
				if (flag == 1)
					break;
			}

			//need to process port or not
			if (startString[0] == ':' || startString[0] == '/')
			{
				ProcessSpecialCase(startString, node->m_path, flag);
				if (flag == 1)
					break;
			}
			if (startString[0] == '?')
			{
				if (startString[1] == 'q' && startString[2] == '=')
				{
					startString.remove_prefix(2);
					ProcessSpecialCase(startString, node->m_query, flag);
				}
				else
				{
					break;
				}
				
				if (flag == 1)
					break;
			}
			if (startString[0] == '#')
			{
				ProcessSpecialCase(startString, node->m_fragment, flag);
				if (flag == 1)
					break;
			}
		}
		mainFlag = 0;
	}
	void ProcessSpecialCase(std::string_view& str, std::string& out, uint8_t& flag, const char* delims)
	{
		size_t pathEnd = str.find_first_of(delims, 1);
		if (pathEnd == std::string::npos)
		{
			out = str.substr(1);
			flag = 1;
		}
		else
		{
			out = str.substr(1, pathEnd - 1);
			str.remove_prefix(pathEnd);
			flag = 0;
		}
	}
	void ProcessSpecialCase(std::string_view& str, std::vector< std::string >& out, uint8_t& flag, const char* delims)
	{
		size_t pathEnd = str.find_first_of(delims, 1);
		if (pathEnd == std::string::npos)
		{
			out.emplace_back(str.substr(1));
			flag = 1;
		}
		else
		{
			out.emplace_back(str.begin() + 1, str.begin() + pathEnd);
			str.remove_prefix(pathEnd);
			flag = 0;
		}
	}
	bool ParseUint16(std::string_view& str, uint16_t& portNumber, size_t& count)
	{
		uint32_t tmpPort = 0;
		int index = 0;
		for (auto c : str)
		{
			if (c < '0' || c > '9')
				continue;
			tmpPort *= 10;
			tmpPort += (uint16_t)(c - '0');
			if ((tmpPort & ~((1 << 16) - 1)) != 0)
			{
				index = 0;
				return false;
			}
			++index;
		}
		count = index;
		portNumber = (uint16_t)tmpPort;
		return true;
	}
}
