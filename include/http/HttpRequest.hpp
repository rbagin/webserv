#pragma once

#include <iostream>
#include <string>
#include <map>
#include <algorithm>

enum HttpMethod
{
	GET,
	POST,
	DELETE,
	UNKNOWN
};

class HttpRequest
{
	public:
		HttpMethod _method;
		std::string _uri;
		std::string _path;
		std::string _query;
		std::string _version;
		std::string _body;
		std::map<std::string, std::string> _headers;
		size_t _contentLength;
		bool _chunked;
		bool _complete;
		bool _hasError;
		int _errorCode;
		std::string _errorMessage;

		HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);
		~HttpRequest() = default;

		std::string getHeader(const std::string& name) const;
		bool hasHeader(const std::string& name) const;
		void clear();

		void print() const; //debug function, delete at the end
};
