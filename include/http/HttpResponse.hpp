#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP


# include <string>
# include <map>
# include <cctype>

# include "cgi/CGIResult.hpp"

struct CaseInsensitiveLess {
	bool operator()(const std::string& a, const std::string& b) const
	{
		std::string x = a;
		std::string y = b;
		for (size_t i = 0; i < x.length(); i++)
		{
			x[i] = tolower(static_cast<unsigned char>(x[i]));
		}
		for (size_t i = 0; i < y.length(); i++)
		{
			y[i] = tolower(static_cast<unsigned char>(y[i]));
		}
		return x < y;
	}
};

class HttpResponse {
public:
	int statusCode = 200;
	std::string statusText;
	std::map<std::string, std::string, CaseInsensitiveLess> headers;
	std::string body;
	HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& rhs);
	~HttpResponse();
	static HttpResponse make(int code, std::string body = "");
	static HttpResponse makeError(int code);
	static HttpResponse redirect(const std::string &location, int code = 301);
	static HttpResponse fromCGI(const CGIResult &result);
	static std::string reasonFor(int code);
	void setHeader(const std::string &name,const std::string &value);
	std::string getHeader(const std::string &name) const;
	bool hasHeader(const std::string &name) const;
	std::string toBytes() const;
};

#endif
