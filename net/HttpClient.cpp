#include "HttpClient.hpp"
#include "Service.hpp"
#include "../OutputStream.hpp"
#if defined(___INANITY_PLATFORM_EMSCRIPTEN)
#include "../File.hpp"
#include <emscripten/fetch.h>
#else
#include "../MemoryFile.hpp"
#include "TcpSocket.hpp"
#include "HttpStream.hpp"
#include "../Strings.hpp"
#include "../Exception.hpp"
#include <sstream>
#include <cstring>
#endif

BEGIN_INANITY_NET

#if defined(___INANITY_PLATFORM_EMSCRIPTEN)

class HttpClientRequest : public Object
{
private:
	ptr<File> dataFile;
	ptr<SuccessHandler> handler;
	ptr<OutputStream> outputStream;

public:
	HttpClientRequest(ptr<File> dataFile, ptr<SuccessHandler> handler, ptr<OutputStream> outputStream)
	: dataFile(dataFile), handler(handler), outputStream(outputStream) {}

	ptr<File> GetData() const
	{
		return dataFile;
	}

	void OnSuccess(emscripten_fetch_t* fetch)
	{
		if(fetch->numBytes)
			outputStream->Write(fetch->data, fetch->numBytes);
		handler->FireSuccess();
		emscripten_fetch_close(fetch);
		Dereference();
	}

	void OnError(emscripten_fetch_t* fetch)
	{
		handler->FireError(NEW(Exception(String("Http client error: ") + fetch->statusText)));
		emscripten_fetch_close(fetch);
		Dereference();
	}

	void OnProgress(emscripten_fetch_t* fetch)
	{
		if(fetch->numBytes)
			outputStream->Write(fetch->data, fetch->numBytes);
	}

	static void StaticOnSuccess  (emscripten_fetch_t* fetch) { ((HttpClientRequest*)fetch->userData)->OnSuccess(fetch); }
	static void StaticOnError    (emscripten_fetch_t* fetch) { ((HttpClientRequest*)fetch->userData)->OnError(fetch); }
	static void StaticOnProgress (emscripten_fetch_t* fetch) { ((HttpClientRequest*)fetch->userData)->OnProgress(fetch); }
};

void HttpClient::Fetch(ptr<Service> service, const String& url, const String& method, ptr<File> dataFile, const String& contentType, ptr<SuccessHandler> handler, ptr<OutputStream> outputStream)
{
	BEGIN_TRY();

	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, method.c_str());
	attr.userData = NEW(HttpClientRequest(dataFile, handler, outputStream));
	attr.onsuccess = &HttpClientRequest::StaticOnSuccess;
	attr.onerror = &HttpClientRequest::StaticOnError;
	attr.onprogress = &HttpClientRequest::StaticOnProgress;
	// attr.attributes = EMSCRIPTEN_FETCH_STREAM_DATA; // doesn't work :(
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	const char* const headers[] = { "Content-Type", contentType.c_str(), nullptr };
	if(contentType.length())
		attr.requestHeaders = headers;
	attr.requestData = dataFile ? (const char*)dataFile->GetData() : nullptr;
	attr.requestDataSize = dataFile ? dataFile->GetSize() : 0;
	emscripten_fetch(&attr, url.c_str());

	END_TRY("Can't fetch http");
}

#else

class HttpClientRequest : public Object
{
private:
	ptr<File> requestFile;
	ptr<SuccessHandler> handler;
	ptr<HttpStream> outputStream;

public:
	HttpClientRequest(ptr<Service> service, const String& host, int port, ptr<File> requestFile, ptr<SuccessHandler> handler, ptr<OutputStream> outputStream)
	: requestFile(requestFile), handler(handler), outputStream(HttpStream::CreateResponseStream(outputStream))
	{
		service->ConnectTcp(host, port, Service::TcpSocketHandler::Bind(MakePointer(this), &HttpClientRequest::OnConnect));
	}

private:
	void OnConnect(const Service::TcpSocketHandler::Result& result)
	{
		ptr<HttpClientRequest> self = this;

		ptr<TcpSocket> socket;
		try
		{
			socket = result.GetData();

			// установить принимающий обработчик
			socket->SetReceiveHandler(TcpSocket::ReceiveHandler::Bind(MakePointer(this), &HttpClientRequest::OnReceive));

			// отправить HTTP-запрос
			socket->Send(requestFile);
			requestFile = nullptr;
		}
		catch(Exception* exception)
		{
			if(socket)
				socket->Close();
			handler->FireError(exception);
		}
	}

	void OnReceive(const TcpSocket::ReceiveHandler::Result& result)
	{
		try
		{
			ptr<File> data = result.GetData();

			if(data)
				// пришли данные
				outputStream->OutputStream::Write(data);
			else
			{
				// корректный конец данных
				outputStream->End();
				if(outputStream->IsCompleted())
					handler->FireSuccess();
				else
					THROW("HTTP response is not completed");
			}
		}
		catch(Exception* exception)
		{
			// ошибка получения
			handler->FireError(exception);
		}
	}
};

void HttpClient::Fetch(ptr<Service> service, const String& url, const String& method, ptr<File> data, const String& contentType, ptr<SuccessHandler> handler, ptr<OutputStream> outputStream)
{
	BEGIN_TRY();

	// разобрать URL
	http_parser_url parsedUrl;
	if(http_parser_parse_url(url.c_str(), url.length(), 0, &parsedUrl))
		THROW("Can't parse url");
	String schema = "http";
	if(parsedUrl.field_set & (1 << UF_SCHEMA))
		schema = url.substr(parsedUrl.field_data[UF_SCHEMA].off, parsedUrl.field_data[UF_SCHEMA].len);
	String host;
	if(parsedUrl.field_set & (1 << UF_HOST))
		host = url.substr(parsedUrl.field_data[UF_HOST].off, parsedUrl.field_data[UF_HOST].len);
	int defaultPort = schema == "https" ? 443 : 80, port = defaultPort;
	if(parsedUrl.field_set & (1 << UF_PORT))
		port = parsedUrl.port;
	String path = "/";
	if(parsedUrl.field_set & (1 << UF_PATH))
		path = url.substr(parsedUrl.field_data[UF_PATH].off, parsedUrl.field_data[UF_PATH].len);
	String query = "";
	if(parsedUrl.field_set & (1 << UF_QUERY))
		query = url.substr(parsedUrl.field_data[UF_QUERY].off, parsedUrl.field_data[UF_QUERY].len);
	if(!query.empty())
		query = "?" + query;

	// сформировать запрос
	std::ostringstream request(std::ios::out | std::ios::binary);
	request << method << " " << path << query << " HTTP/1.1\r\n";
	request << "Connection: close\r\n";
	request << "Host: " << host;
	if(port != defaultPort)
		request << ":" << port;
	request << "\r\n";
	request << "User-Agent: Inanity HttpClient/2.0\r\n";
	if(contentType.length())
		request << "Content-Type: " << contentType << "\r\n";
	request << "Content-Length: " << (data ? data->GetSize() : 0) << "\r\n";
	request << "\r\n";

	ptr<MemoryFile> requestFile = NEW(MemoryFile(request.str().length() + (data ? data->GetSize() : 0)));
	memcpy(requestFile->GetData(), request.str().c_str(), request.str().length());
	if(data)
		memcpy((char*)requestFile->GetData() + request.str().length(), data->GetData(), data->GetSize());

	MakePointer(NEW(HttpClientRequest(service, host, port, requestFile, handler, outputStream)));

	END_TRY("Can't fetch http");
}

#endif

void HttpClient::Get(ptr<Service> service, const String& url, ptr<SuccessHandler> handler, ptr<OutputStream> outputStream)
{
	return Fetch(service, url, "", nullptr, "", handler, outputStream);
}

END_INANITY_NET
