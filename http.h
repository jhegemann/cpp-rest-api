/* MIT License

Copyright (c) 2020 Jonas Hegemann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <thread>
#include <unordered_map>

#include "tcp.h"

const std::string kHttpProtocol1_1 = "HTTP/1.1";
const std::string kHttpLineFeed = "\r\n";
const std::string kHttpDoubleLineFeed = "\r\n\r\n";
const long kHttpConnectionTimeout = 10000;
const long kHttpTick = 60000;

enum HttpMethod {
  INVALID = 0,
  POST,
  GET,
  HEAD,
  PUT,
  DELETE,
  CONNECT,
  UPDATE,
  TRACE,
  PATCH,
  OPTIONS
};

enum HttpStatus {
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,
  PROCESSING = 102,
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NO_CONTENT = 204,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  REQUEST_TIMEOUT = 408,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  REQUEST_ENTITY_TOO_LARGE = 413,
  REQUEST_URI_TOO_LONG = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  EXPECTATION_FAILED = 417,
  UNPROCESSABLE_ENTITY = 422,
  LOCKED = 423,
  TOO_MANY_REQUESTS = 429,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
};

class HttpConstants {
public:
  static std::string GetStatusString(int status);
  static std::string GetMethodString(const HttpMethod method);
  static HttpMethod GetMethod(const std::string method_string);
};

class HttpRequest {
public:
  HttpRequest();
  virtual ~HttpRequest();
  void Initialize();
  void SetMethod(const HttpMethod method);
  const HttpMethod &GetMethod() const;
  void SetUrl(const std::string &url);
  const std::string &GetUrl() const;
  void SetProtocol(const std::string &protocol);
  const std::string &GetProtocol() const;
  void AddHeader(const std::string &key, const std::string &value);
  void AddHeader(const std::string &key, size_t value);
  const std::string GetHeader(const std::string &key) const;
  void SetBody(const std::string &body);
  void AppendToBody(const std::string &text);
  const std::string &GetBody() const;
  const std::string AsString() const;
  const size_t CountHeaders() const;

private:
  HttpMethod method_;
  std::string url_;
  std::string protocol_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

class HttpResponse {
public:
  HttpResponse();
  virtual ~HttpResponse();
  void Initialize();
  void SetProtocol(const std::string &protocol);
  const std::string &GetProtocol() const;
  void SetStatus(const int status);
  const int GetStatus() const;
  void SetMessage(const std::string &message);
  const std::string &GetMessage() const;
  void AddHeader(const std::string &key, const std::string &value);
  void AddHeader(const std::string &key, size_t value);
  const std::string GetHeader(const std::string &key) const;
  void SetBody(const std::string &body);
  const std::string &GetBody() const;
  static HttpResponse Build(const int status);
  static HttpResponse Build(const int status, const std::string &body);
  const std::string AsString() const;

private:
  std::string protocol_;
  int status_;
  std::string message_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

typedef std::function<HttpResponse(const HttpRequest &)> HttpCallback;

class HttpHandler {
public:
  HttpHandler();
  HttpHandler(const HttpMethod method, const std::string &url,
              HttpCallback callback);
  virtual ~HttpHandler();
  void SetMethod(const HttpMethod method);
  const HttpMethod &GetMethod() const;
  void SetUrl(const std::string &url);
  const std::string &GetUrl() const;
  void SetCallback(HttpCallback callback);
  HttpCallback GetCallback();

private:
  HttpMethod method_;
  std::string url_;
  HttpCallback callback_;
};

enum HttpStage { START = 0, METHOD, URL, PROTOCOL, HEADER, BODY, END, FAILED };

class HttpConnection {
public:
  HttpConnection(TcpSocket *socket);
  virtual ~HttpConnection();
  const HttpStage GetStage() const;
  TcpReader *GetReader();
  TcpWriter *GetWriter();
  const HttpRequest &GetRequest();
  void Parse();
  void Restart();
  bool IsGood();
  long GetExpiry();

private:
  HttpRequest request_;
  HttpStage stage_;
  TcpReader *reader_;
  TcpWriter *writer_;
  TcpSocket *socket_;
  long expiry_;
};

class HttpServer {
public:
  typedef std::multimap<std::string, HttpHandler>::iterator HandlerIterator;
  typedef std::pair<HandlerIterator, HandlerIterator> HandlerRange;
  HttpServer();
  virtual ~HttpServer();
  void RegisterHandler(HttpMethod method, const std::string &url,
                       HttpCallback callback);
  HttpResponse ExecuteHandler(const HttpRequest &request);
  void Serve(const std::string &service, const std::string &host);

private:
  bool SetupServerSocket(const std::string &service, const std::string &host);
  void DeleteConnection(int descriptor);
  void DeleteConnections();
  void DeleteExpiredConnections();
  void ClearTimer();
  void ScheduleTimer(long duration);
  bool IsTimerScheduled();
  std::atomic<bool> running_;
  TcpSocket server_socket_;
  std::multimap<std::string, HttpHandler> handlers_;
  EpollInstance epoll_instance_;
  std::map<int, HttpConnection *> connections_;
  sigset_t sigset_;
  int signal_descriptor_;
  struct signalfd_siginfo signal_info_;
  int timer_descriptor_;
  struct itimerspec timer_current_;
  struct itimerspec timer_schedule_;
};
