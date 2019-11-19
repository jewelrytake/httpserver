#include <Uri.hpp>
#include <string>

namespace Uri
{
	struct Uri::Impl
	{
		std::string scheme_;
		std::string host_;
		std::vector< std::string > path_;
	};
	Uri::~Uri() = default;
	Uri::Uri() :
		impl_(new Impl)
	{}
	bool Uri::ParseFromString(const std::string & uriString)
	{
		const char* delimiters = ":/";
		size_t end = uriString.find_first_of(delimiters);
		impl_->scheme_ = uriString.substr(0, end);
		if (uriString[end + 1] == '/' && uriString[end + 2] == '/')
		{
			std::string rest = uriString.substr(end + 3);
			end = rest.find_first_of(delimiters);
			impl_->host_ = rest.substr(0, end);
			impl_->path_.clear();
			if (rest[end] == '/')
			{
				impl_->path_.emplace_back("");
				rest = rest.substr(end + 1);
			}
			for (;;)
			{
				end = rest.find_first_of(delimiters);
				size_t endPos = rest.find_first_of(delimiters, 1);
				if (!rest.substr(0, endPos).empty())
					impl_->path_.emplace_back(rest.substr(0, endPos));
				rest = rest.substr(end + 1);
				if (endPos == std::string::npos)
					break;
				
			}
		}
		else 
		{
			impl_->host_ = "";
			end += 1;
			std::string rest = uriString.substr((end) + uriString.find_first_not_of(delimiters));
			impl_->path_.clear();
			for (;;)
			{
				end = rest.find_first_of(delimiters);
				size_t endPos = rest.find_first_of(delimiters, 1);
				if (!rest.substr(0, endPos).empty())
					impl_->path_.emplace_back(rest.substr(0, endPos));
				rest = rest.substr(end + 1);	
				if (endPos == std::string::npos)
					break;
			}
		}
		return true;
	}
	std::string Uri::GetHost() const
	{
		return impl_->host_;
	}
	std::string Uri::GetScheme() const
	{
		return impl_->scheme_;
	}
	std::vector<std::string> Uri::GetPath() const
	{
		return impl_->path_;
	}
}