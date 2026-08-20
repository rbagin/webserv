#include "http/HttpResponse.hpp"
#include <iostream>
#include <cstdlib>

HttpResponse::HttpResponse()
{}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& rhs)
{
	if (this != &rhs)
	{
		this->statusCode = rhs.statusCode;
		this->statusText = rhs.statusText;
		this->headers = rhs.headers;
		this->body = rhs.body;

	}
	return (*this);
}

HttpResponse::~HttpResponse()
{}

std::string HttpResponse::reasonFor(int code)
{
	static const std::map<int, std::string> lookupTable{
		{200, "OK"},
		{201, "Created"},
		{204, "No Content"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{304, "Not Modified"},
		{400, "Bad Request"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{413, "Payload Too Large"},
		{414, "URI Too Long"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"}};
	auto it = lookupTable.find(code);
	if (it != lookupTable.end())
		return (it->second);
	else
		return ("Missing code");
}

HttpResponse HttpResponse::make(int code, std::string body)
{
	HttpResponse response;
	response.body = body;
	response.statusCode = code;
	response.statusText = reasonFor(code);
	return response;
}

static	std::string errorPage(int code)
{
	std::string defaultPage = R"(<html><body><h1>)" + std::to_string(code) + " "
		+ HttpResponse::reasonFor(code) + R"(</h1></body></html>)";
	return defaultPage;
}

HttpResponse HttpResponse::makeError(int code)
{
	HttpResponse response;
	response.statusCode = code;
	response.statusText = reasonFor(code);
	response.body = errorPage(code);
	response.setHeader("Content-Type", "text/html");
	return response;
}

HttpResponse HttpResponse::redirect(const std::string &location, int code)
{
	HttpResponse response;
	response.statusCode = code;
	response.statusText = reasonFor(code);
	response.body = R"(<html><body><h1>)" + std::to_string(code) + " "
		+ response.statusText + R"(</h1><a href=")" + location + R"(">)"
		+ location + R"(</a></body></html>)";
	response.setHeader("Location", location);
	response.setHeader("Content-Type", "text/html");
	return response;
}

static std::string toLower(const std::string &s)
{
	std::string out = s;
	for (size_t i = 0; i < out.length(); i++)
		out[i] = tolower(static_cast<unsigned char>(out[i]));
	return out;
}

static std::string trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t");
	if (start == std::string::npos)
		return ("");
	size_t end = s.find_last_not_of(" \t");
	return s.substr(start, end - start + 1);
}

/*
** CGI headers end at the first blank line. Scripts are inconsistent about
** line endings, so accept both \r\n\r\n and \n\n and take whichever comes
** first. Returns false when there is no blank line at all.
*/
static bool splitCGIOutput(const std::string &out, std::string &headerBlock,
	std::string &body)
{
	size_t crlf = out.find("\r\n\r\n");
	size_t lf = out.find("\n\n");
	size_t pos;
	size_t len;

	if (crlf != std::string::npos && (lf == std::string::npos || crlf < lf))
	{
		pos = crlf;
		len = 4;
	}
	else if (lf != std::string::npos)
	{
		pos = lf;
		len = 2;
	}
	else
		return (false);
	headerBlock = out.substr(0, pos);
	body = out.substr(pos + len);
	return (true);
}

HttpResponse HttpResponse::fromCGI(const CGIResult &result)
{
	if (result.timedOut)
		return makeError(504);
	if (!result.success)
		return makeError(500);

	std::string headerBlock;
	std::string body;
	if (!splitCGIOutput(result.output, headerBlock, body))
		return makeError(500);

	HttpResponse response;
	std::string statusValue;
	size_t start = 0;
	while (start <= headerBlock.length())
	{
		size_t end = headerBlock.find('\n', start);
		std::string line = (end == std::string::npos)
			? headerBlock.substr(start)
			: headerBlock.substr(start, end - start);
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colon = line.find(':');
		if (colon != std::string::npos)
		{
			std::string name = trim(line.substr(0, colon));
			std::string value = trim(line.substr(colon + 1));
			if (toLower(name) == "status")
				statusValue = value;
			else if (!name.empty())
				response.setHeader(name, value);
		}
		if (end == std::string::npos)
			break;
		start = end + 1;
	}

	// "Status: 302 Found" — code first, optional reason phrase after it.
	if (!statusValue.empty())
	{
		response.statusCode = atoi(statusValue.c_str());
		size_t space = statusValue.find(' ');
		if (space != std::string::npos)
			response.statusText = trim(statusValue.substr(space + 1));
	}
	else if (response.hasHeader("Location"))
		response.statusCode = 302;
	else
		response.statusCode = 200;
	if (response.statusText.empty())
		response.statusText = reasonFor(response.statusCode);
	response.body = body;
	return response;
}

void HttpResponse::setHeader(const std::string &name,const std::string &value)
{
	headers[name] = value;
}

std::string HttpResponse::getHeader(const std::string &name) const
{
	auto it = headers.find(name);
	if (it != headers.end())
		return (it->second);
	else
		return ("");
}

bool HttpResponse::hasHeader(const std::string &name) const
{
	auto it = headers.find(name);
	return it != headers.end();
}

/*
** 1xx, 204 and 304 carry no body and no Content-Length. For every other
** status we emit Content-Length ourselves, unless a header already set it
** (a CGI script may have) — otherwise it would go out twice.
*/
std::string HttpResponse::toBytes() const
{
	bool bodyless = statusCode == 204 || statusCode == 304
		|| (statusCode >= 100 && statusCode < 200);

	std::string output;
	output += "HTTP/1.1 ";
	output += std::to_string(statusCode);
	output += " ";
	output += statusText;
	output += "\r\n";
	for (const auto& h : headers)
	{
		if (bodyless && h.first == "Content-Length")
			continue;
		output += h.first + ": " + h.second + "\r\n";
	}
	if (!bodyless && !hasHeader("Content-Length"))
	{
		output += "Content-Length: ";
		output += std::to_string(body.size());
		output += "\r\n";
	}
	output += "\r\n";
	if (!bodyless)
		output += body;
	return output;
}

