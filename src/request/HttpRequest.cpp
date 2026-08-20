#include "../../include/http/HttpRequest.hpp"

HttpRequest::HttpRequest() :
	_method(UNKNOWN),
	_uri(""),
	_path(""),
	_query(""),
	_version(""),
	_body(""),
	_headers(),
	_contentLength(0),
	_chunked(false),
	_complete(false),
	_hasError(false),
	_errorCode(0),
	_errorMessage("") {}

HttpRequest::HttpRequest(const HttpRequest& other) :
	_method(other._method),
	_uri(other._uri),
	_path(other._path),
	_query(other._query),
	_version(other._version),
	_body(other._body),
	_headers(other._headers),
	_contentLength(other._contentLength),
	_chunked(other._chunked),
	_complete(other._complete),
	_hasError(other._hasError),
	_errorCode(other._errorCode),
	_errorMessage(other._errorMessage) {}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		_method = other._method;
		_uri = other._uri;
		_path = other._path;
		_query = other._query;
		_version = other._version;
		_body = other._body;
		_headers = other._headers;
		_contentLength = other._contentLength;
		_chunked = other._chunked;
		_complete = other._complete;
		_hasError = other._hasError;
		_errorCode = other._errorCode;
		_errorMessage = other._errorMessage;
	}
	return *this;
}

std::string HttpRequest::getHeader(const std::string& name) const
{
	std::string lowerName = name;
	std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

	std::map<std::string, std::string>::const_iterator it = _headers.find(lowerName);
	if (it != _headers.end())
		return it->second;
	return "";
}

bool HttpRequest::hasHeader(const std::string& name) const
{
	std::string lowerName = name;
	std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

	return _headers.find(lowerName) != _headers.end();
}

void HttpRequest::clear()
{
	_method = UNKNOWN;
	_uri.clear();
	_path.clear();
	_query.clear();
	_version.clear();
	_body.clear();
	_headers.clear();
	_contentLength = 0;
	_chunked = false;
	_complete = false;
	_hasError = false;
	_errorCode = 0;
	_errorMessage.clear();
}

void HttpRequest::print() const
{
	std::cout << "Method: " << _method << std::endl;
	std::cout << "URI: " << _uri << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Query: " << _query << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Body: " << _body << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "Content-Length: " << _contentLength << std::endl;
	std::cout << "Chunked: " << (_chunked ? "true" : "false") << std::endl;
	std::cout << "Complete: " << (_complete ? "true" : "false") << std::endl;
	std::cout << "Has Error: " << (_hasError ? "true" : "false") << std::endl;
	if (_hasError)
	{
		std::cout << "Error Code: " << _errorCode << std::endl;
		std::cout << "Error Message: " << _errorMessage << std::endl;
	}
}
