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

#include "http.h"

std::string HttpConstants::GetStatusString(int status) {
  struct StaticMap : std::unordered_map<int, std::string> {
    StaticMap() {
      insert(std::make_pair(CONTINUE, "Continue"));
      insert(std::make_pair(SWITCHING_PROTOCOLS, "Switching Protocols"));
      insert(std::make_pair(PROCESSING, "Processing"));
      insert(std::make_pair(OK, "OK"));
      insert(std::make_pair(CREATED, "Created"));
      insert(std::make_pair(ACCEPTED, "Accepted"));
      insert(std::make_pair(NO_CONTENT, "No Content"));
      insert(std::make_pair(MOVED_PERMANENTLY, "Moved Permanently"));
      insert(std::make_pair(FOUND, "Found"));
      insert(std::make_pair(SEE_OTHER, "See Other"));
      insert(std::make_pair(NOT_MODIFIED, "Not Modfied"));
      insert(std::make_pair(BAD_REQUEST, "Bad Request"));
      insert(std::make_pair(UNAUTHORIZED, "Unauthorized"));
      insert(std::make_pair(FORBIDDEN, "Forbidden"));
      insert(std::make_pair(NOT_FOUND, "Not Found"));
      insert(std::make_pair(METHOD_NOT_ALLOWED, "Method Not Allowed"));
      insert(std::make_pair(NOT_ACCEPTABLE, "Not Acceptable"));
      insert(std::make_pair(REQUEST_TIMEOUT, "Request Timeout"));
      insert(std::make_pair(GONE, "Gone"));
      insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
      insert(
          std::make_pair(REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"));
      insert(std::make_pair(REQUEST_URI_TOO_LONG, "Request URI Too Long"));
      insert(std::make_pair(UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"));
      insert(std::make_pair(EXPECTATION_FAILED, "Expectation Failed"));
      insert(std::make_pair(UNPROCESSABLE_ENTITY, "Unprocessable Entity"));
      insert(std::make_pair(LOCKED, "Locked"));
      insert(std::make_pair(TOO_MANY_REQUESTS, "Too Many Requests"));
      insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
      insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
      insert(std::make_pair(BAD_GATEWAY, "Bad Gateway"));
      insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
    }
  } static status_to_string;
  auto it = status_to_string.find(status);
  if (it != status_to_string.end()) {
    return it->second;
  }
  return kStringEmpty;
}

std::string HttpConstants::GetMethodString(const HttpMethod method) {
  struct StaticMap : std::unordered_map<HttpMethod, std::string> {
    StaticMap() {
      insert(std::make_pair(INVALID, "INVALID"));
      insert(std::make_pair(POST, "POST"));
      insert(std::make_pair(GET, "GET"));
      insert(std::make_pair(HEAD, "HEAD"));
      insert(std::make_pair(PUT, "PUT"));
      insert(std::make_pair(DELETE, "DELETE"));
      insert(std::make_pair(CONNECT, "CONNECT"));
      insert(std::make_pair(UPDATE, "UPDATE"));
      insert(std::make_pair(TRACE, "TRACE"));
      insert(std::make_pair(PATCH, "PATCH"));
      insert(std::make_pair(OPTIONS, "OPTIONS"));
    }
  } static method_to_string;
  auto it = method_to_string.find(method);
  if (it != method_to_string.end()) {
    return it->second;
  }
  return kStringEmpty;
}

HttpMethod HttpConstants::GetMethod(const std::string method_string) {
  struct StaticMap : std::unordered_map<std::string, HttpMethod> {
    StaticMap() {
      insert(std::make_pair("INVALID", INVALID));
      insert(std::make_pair("POST", POST));
      insert(std::make_pair("GET", GET));
      insert(std::make_pair("HEAD", HEAD));
      insert(std::make_pair("PUT", PUT));
      insert(std::make_pair("DELETE", DELETE));
      insert(std::make_pair("CONNECT", CONNECT));
      insert(std::make_pair("UPDATE", UPDATE));
      insert(std::make_pair("TRACE", TRACE));
      insert(std::make_pair("PATCH", PATCH));
      insert(std::make_pair("OPTIONS", OPTIONS));
    }
  } static string_to_method;
  auto it = string_to_method.find(method_string);
  if (it != string_to_method.end()) {
    return it->second;
  }
  return INVALID;
}

HttpRequest::HttpRequest()
    : method_(GET), url_(kStringSlash), protocol_(kHttpProtocol1_1),
      body_(kStringEmpty) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::Initialize() {
  method_ = GET;
  url_ = kStringSlash;
  protocol_ = kHttpProtocol1_1;
  headers_.clear();
  body_ = kStringEmpty;
}

void HttpRequest::SetMethod(const HttpMethod method) { method_ = method; }

const HttpMethod &HttpRequest::GetMethod() const { return method_; }

void HttpRequest::SetUrl(const std::string &url) { url_ = url; }

const std::string &HttpRequest::GetUrl() const { return url_; }

void HttpRequest::SetProtocol(const std::string &protocol) {
  protocol_ = protocol;
}

const std::string &HttpRequest::GetProtocol() const { return protocol_; }

void HttpRequest::AddHeader(const std::string &key, const std::string &value) {
  headers_[StringToLower(key)] = value;
}

void HttpRequest::AddHeader(const std::string &key, size_t value) {
  headers_[StringToLower(key)] = std::to_string(value);
}

const std::string HttpRequest::GetHeader(const std::string &key) const {
  auto it = headers_.find(StringToLower(key));
  if (it != headers_.end()) {
    return it->second;
  }
  return kStringEmpty;
}

void HttpRequest::SetBody(const std::string &body) { body_ = body; }

void HttpRequest::AppendToBody(const std::string &text) { body_.append(text); }

const std::string &HttpRequest::GetBody() const { return body_; }

const std::string HttpRequest::AsString() const {
  std::stringstream packet;
  packet << HttpConstants::GetMethodString(method_) << kStringSpace << url_
         << kStringSpace << protocol_ << kHttpLineFeed;
  for (auto it = headers_.begin(); it != headers_.end(); it++) {
    packet << it->first << kStringColon << kStringSpace << it->second
           << kHttpLineFeed;
  }
  packet << kHttpLineFeed;
  packet << body_;
  return packet.str();
}

const size_t HttpRequest::CountHeaders() const { return headers_.size(); }

HttpResponse::HttpResponse()
    : protocol_(kHttpProtocol1_1), status_(OK),
      message_(HttpConstants::GetStatusString(OK)), body_(kStringEmpty) {}

HttpResponse::~HttpResponse() {}

void HttpResponse::Initialize() {
  protocol_ = kHttpProtocol1_1;
  status_ = OK;
  message_ = HttpConstants::GetStatusString(OK);
  headers_.clear();
  body_ = kStringEmpty;
}

void HttpResponse::SetProtocol(const std::string &protocol) {
  protocol_ = protocol;
}

const std::string &HttpResponse::GetProtocol() const { return protocol_; }

void HttpResponse::SetStatus(const int status) { status_ = status; }

const int HttpResponse::GetStatus() const { return status_; }

void HttpResponse::SetMessage(const std::string &message) {
  message_ = message;
}

const std::string &HttpResponse::GetMessage() const { return message_; }

void HttpResponse::AddHeader(const std::string &key, const std::string &value) {
  headers_[StringToLower(key)] = value;
}

void HttpResponse::AddHeader(const std::string &key, const size_t value) {
  headers_[StringToLower(key)] = std::to_string(value);
}

const std::string HttpResponse::GetHeader(const std::string &key) const {
  auto it = headers_.find(StringToLower(key));
  if (it != headers_.end()) {
    return it->second;
  }
  return kStringEmpty;
}

void HttpResponse::SetBody(const std::string &body) { body_ = body; }

const std::string &HttpResponse::GetBody() const { return body_; }

HttpResponse HttpResponse::Build(const int status) {
  HttpResponse response;
  response.SetStatus(status);
  response.SetMessage(HttpConstants::GetStatusString(status));
  response.AddHeader("date", time(nullptr));
  response.AddHeader("server", "alvagis version 1.0");
  response.AddHeader("content-length", 0);
  return response;
}

HttpResponse HttpResponse::Build(const int status, const std::string &body) {
  HttpResponse response;
  response.SetStatus(status);
  response.SetMessage(HttpConstants::GetStatusString(status));
  response.AddHeader("date", time(nullptr));
  response.AddHeader("server", "alvagis version 1.0");
  response.AddHeader("content-length", body.length());
  response.SetBody(body);
  return response;
}

const std::string HttpResponse::AsString() const {
  std::stringstream packet;
  packet << protocol_ << kStringSpace << status_ << kStringSpace << message_
         << kHttpLineFeed;
  for (auto it = headers_.begin(); it != headers_.end(); it++) {
    packet << it->first << kStringColon << kStringSpace << it->second
           << kHttpLineFeed;
  }
  packet << kHttpLineFeed;
  packet << body_;
  return packet.str();
}

HttpHandler::HttpHandler() : method_(GET), url_(kStringSlash) {}

HttpHandler::HttpHandler(const HttpMethod method, const std::string &url,
                         HttpCallback callback)
    : method_(method), url_(url), callback_(callback) {}

HttpHandler::~HttpHandler() {}

void HttpHandler::SetMethod(const HttpMethod method) { method_ = method; }

const HttpMethod &HttpHandler::GetMethod() const { return method_; }

void HttpHandler::SetUrl(const std::string &url) { url_ = url; }

const std::string &HttpHandler::GetUrl() const { return url_; }

void HttpHandler::SetCallback(HttpCallback callback) { callback_ = callback; }

HttpCallback HttpHandler::GetCallback() { return callback_; }

HttpConnection::HttpConnection(TcpSocket *socket) {
  stage_ = START;
  expiry_ = TimeEpochMilliseconds() + kHttpConnectionTimeout;
  socket_ = socket;
  reader_ = new TcpReader(socket);
  writer_ = new TcpWriter(socket);
}

HttpConnection::~HttpConnection() {
  delete reader_;
  delete writer_;
  socket_->Close();
}

const HttpStage HttpConnection::GetStage() const { return stage_; }

TcpReader *HttpConnection::GetReader() { return reader_; }

TcpWriter *HttpConnection::GetWriter() { return writer_; }

const HttpRequest &HttpConnection::GetRequest() { return request_; }

long HttpConnection::GetExpiry() { return expiry_; }

void HttpConnection::Parse() {
  static std::string token;
  static std::string line;
  static std::string key;
  static std::string value;
  static std::string content_length_string;
  static size_t position;
  static size_t content_length;
  static HttpMethod method;
  switch (stage_) {
  case START:
  case METHOD:
    if ((position = reader_->GetPosition(kStringSpace)) == std::string::npos) {
      break;
    }
    token = reader_->PopSegment(position);
    if (token.empty()) {
      stage_ = FAILED;
      return;
    }
    method = HttpConstants::GetMethod(token);
    if (method == INVALID) {
      stage_ = FAILED;
      return;
    }
    request_.SetMethod(method);
    stage_ = URL;
  case URL:
    if ((position = reader_->GetPosition(kStringSpace)) == std::string::npos) {
      return;
    }
    token = reader_->PopSegment(position);
    if (token.empty() || !StringStartsWith(token, kStringSlash) || StringContains(token, kStringSlash + kStringSlash)) {
      stage_ = FAILED;
      return;
    }
    request_.SetUrl(token);
    stage_ = PROTOCOL;
  case PROTOCOL:
    if ((position = reader_->GetPosition(kHttpLineFeed)) == std::string::npos) {
      return;
    }
    token = reader_->PopSegment(position);
    if (token.empty() || token.compare(kHttpProtocol1_1) != 0) {
      stage_ = FAILED;
      return;
    }
    request_.SetProtocol(kHttpProtocol1_1);
    stage_ = HEADER;
  case HEADER:
    if (!reader_->IsInBuffer(kHttpDoubleLineFeed)) {
      return;
    }
    for (;;) {
      line = reader_->PopSegment(kHttpLineFeed);
      StringTrim(line, kStringSpace);
      if (line.empty()) {
        break;
      }
      key = StringPopSegment(line, kStringColon);
      StringTrim(key, kStringSpace);
      if (key.empty()) {
        stage_ = FAILED;
        return;
      }
      value = line;
      StringTrim(value, kStringSpace);
      if (value.empty()) {
        stage_ = FAILED;
        return;
      }
      request_.AddHeader(key, value);
    }
    stage_ = BODY;
  case BODY:
    content_length_string = request_.GetHeader("content-length");
    content_length = content_length_string.empty()
                         ? -1
                         : atoi(content_length_string.c_str());
    if (content_length == -1) {
      stage_ = END;
      return;
    }
    request_.AppendToBody(
        reader_->PopSegment(content_length - request_.GetBody().length()));
    if (request_.GetBody().length() < content_length) {
      return;
    }
    stage_ = END;
  case END:
    return;
  default:
    return;
  }
}

void HttpConnection::Restart() {
  stage_ = START;
  expiry_ = TimeEpochMilliseconds() + kHttpConnectionTimeout;
}

bool HttpConnection::IsGood() { return socket_->IsGood(); }

HttpServer::HttpServer() : running_(false) {}

HttpServer::~HttpServer() {}

void HttpServer::RegisterHandler(HttpMethod method, const std::string &url,
                                 HttpCallback callback) {
  if (running_) {
    return;
  }
  HttpHandler handler(method, url, callback);
  bool registered = false;
  HandlerRange range = handlers_.equal_range(handler.GetUrl());
  for (HandlerIterator it = range.first; it != range.second; it++) {
    if (handler.GetMethod() == it->second.GetMethod()) {
      registered = true;
    }
  }
  if (registered) {
    return;
  }
  handlers_.insert(std::make_pair(url, handler));
}

HttpResponse HttpServer::ExecuteHandler(const HttpRequest &request) {
  HttpResponse response = HttpResponse::Build(NOT_FOUND);
  HandlerRange range = handlers_.equal_range(request.GetUrl());
  for (HandlerIterator it = range.first; it != range.second; it++) {
    if (request.GetMethod() == it->second.GetMethod()) {
      response = (it->second.GetCallback())(request);
    }
  }
  return response;
}

void HttpServer::Serve(const std::string &service, const std::string &host) {
  bool success;
  if (!SetupServerSocket(service, host)) {
    printf("cannot not set up server socket\n");
    return;
  }
  if (!epoll_instance_.Create()) {
    printf("cannot not set up epoll instance\n");
    return;
  }
  if (!epoll_instance_.AddReadableDescriptor(server_socket_.GetDescriptor())) {
    printf("cannot not add listening socket to epoll instance\n");
    return;
  }
  if (sigemptyset(&sigset_) == -1) {
    printf("cannot clear signal set\n");
    return;
  }
  if (sigaddset(&sigset_, SIGINT) == -1 || sigaddset(&sigset_, SIGKILL) == -1 || sigaddset(&sigset_, SIGTERM) == -1) {
    printf("cannot add signal to signal set\n");
    return;
  }
  if (sigprocmask(SIG_BLOCK, &sigset_, nullptr) == -1) {
    printf("cannot block signals\n");
    return;
  }
  if ((signal_descriptor_ = signalfd(-1, &sigset_, 0)) == -1) {
    printf("cannot open signal descriptor\n");
    return;
  }
  if (!UnblockDescriptor(signal_descriptor_)) {
    printf("cannot set signal descriptor to nonblocking mode\n");
    return;
  }
  if (!epoll_instance_.AddReadableDescriptor(signal_descriptor_)) {
    printf("cannot add signal descriptor to epoll instance\n");
    return;
  }
  if ((timer_descriptor_ = timerfd_create(CLOCK_MONOTONIC, 0)) == -1) {
    printf("cannot open timer descriptor\n");
    return;
  }
  if (!UnblockDescriptor(timer_descriptor_)) {
    printf("cannot set timer descriptor to nonblocking mode\n");
    return;
  }
  if (!epoll_instance_.AddReadableDescriptor(timer_descriptor_)) {
    printf("cannot add timer descriptor to epoll instance\n");
    return;
  }
  ScheduleTimer(kHttpConnectionTimeout);
  running_ = true;
  while (running_) {
    int ready = epoll_instance_.Wait();
    for (size_t i = 0; i < ready; i++) {
      if (timer_descriptor_ == epoll_instance_.GetDescriptor(i)) {
        printf("event on timer descriptor\n");
        uint64_t expired = 0;
        ssize_t bytes = read(timer_descriptor_, &expired, sizeof(uint64_t));
        if (bytes == -1) {
          printf("error reading time from timer descriptor\n");
          continue;
        }
        printf("delete expired connections\n");
        DeleteExpiredConnections();
        printf("show connections:\n");
        if (connections_.size() == 0) {
          printf("tick: no connections\n");
          continue;
        }
        for (auto it = connections_.begin(); it != connections_.end(); it++) {
          printf("tick: connection %d\n", it->first);
        }
        continue;
      }
      if (signal_descriptor_ == epoll_instance_.GetDescriptor(i)) {
        printf("event on signal descriptor\n");
        memset(&signal_info_, 0, sizeof(struct signalfd_siginfo));
        ssize_t bytes = read(signal_descriptor_, &signal_info_,
                             sizeof(struct signalfd_siginfo));
        if (bytes == -1) {
          printf("error reading signal info from signal descriptor\n");
          continue;
        }
        if (signal_info_.ssi_signo == SIGINT ||
            signal_info_.ssi_signo == SIGKILL ||
            signal_info_.ssi_signo == SIGTERM) {
          printf("process stopped by signal\n");
          running_ = false;
          break;
        }
        continue;
      }
      if (server_socket_.GetDescriptor() == epoll_instance_.GetDescriptor(i)) {
        printf("event on server socket\n");
        if (epoll_instance_.HasErrors(i)) {
          printf("error condition on server socket\n");
          epoll_instance_.DeleteDescriptor(server_socket_.GetDescriptor());
          if (!SetupServerSocket(service, host)) {
            printf("cannot set up server socket\n");
            return;
          }
          if (!epoll_instance_.AddReadableDescriptor(
                  server_socket_.GetDescriptor())) {
            printf("cannot add listening socket to epoll instance\n");
            return;
          }
          printf("server socket has been restarted\n");
          continue;
        }
        TcpSocket *client_socket;
        client_socket = server_socket_.Accept();
        if (client_socket == nullptr) {
          printf("error accepting new client socket\n");
          continue;
        }
        if (!epoll_instance_.AddReadableDescriptor(
                client_socket->GetDescriptor())) {
          printf("cannot add new client socket to epoll instance\n");
          client_socket->Close();
          delete client_socket;
          continue;
        }
        client_socket->Unblock();
        HttpConnection *connection = new HttpConnection(client_socket);
        connections_.insert(
            std::make_pair(client_socket->GetDescriptor(), connection));
      } else {
        auto lookup = connections_.find(epoll_instance_.GetDescriptor(i));
        if (lookup == connections_.end()) {
          printf("cannot not find connection\n");
          continue;
        }
        int descriptor = lookup->first;
        HttpConnection *connection = lookup->second;
        if (!connection->IsGood() || epoll_instance_.HasErrors(i)) {
          printf("error condition on client socket - remove client\n");
          DeleteConnection(descriptor);
          continue;
        }
        if (epoll_instance_.IsReadable(i)) {
          if (connection->GetStage() == END) {
            printf("connection still readable though successfully parsed\n");
            DeleteConnection(descriptor);
            continue;
          }
          connection->GetReader()->ReadSome();
          if (connection->GetReader()->HasErrors()) {
            printf("error condition on reader - probably connection closed\n");
            DeleteConnection(descriptor);
            continue;
          }
          printf("parse request incoming on connection %d\n", descriptor);
          connection->Parse();
          if (connection->GetStage() == FAILED) {
            printf("parsing of request failed\n");
            DeleteConnection(descriptor);
            continue;
          }
          if (connection->GetStage() == END) {
            printf("execute handler\n");
            connection->GetWriter()->Write(
                ExecuteHandler(connection->GetRequest()).AsString());
            if (!epoll_instance_.SetWriteable(i)) {
              printf("could not set descriptor to write mode\n");
              DeleteConnection(descriptor);
              continue;
            }
          }
        } else if (epoll_instance_.IsWritable(i)) {
          printf("got request:\n");
          printf("%s\n", connection->GetRequest().AsString().c_str());
          printf("send response\n");
          connection->GetWriter()->SendSome();
          if (connection->GetWriter()->IsEmpty()) {
            printf("response has been sent for connection %d\n", descriptor);
            if (connection->GetRequest()
                    .GetHeader("connection")
                    .compare("keep-alive") == 0) {
              printf("keep-alive request detected\n");
              connection->Restart();
              if (!epoll_instance_.SetReadable(i)) {
                printf("could not set descriptor to read mode\n");
                DeleteConnection(descriptor);
                continue;
              }
              printf("connection restarted due to keep-alive header\n");
              continue;
            }
            DeleteConnection(descriptor);
            continue;
          }
          if (connection->GetWriter()->HasErrors()) {
            printf("error occurred when sending response\n");
            DeleteConnection(descriptor);
            continue;
          }
        } else {
          printf("unknown event on client socket\n");
          DeleteConnection(descriptor);
          continue;
        }
      }
    }
  }
  printf("close timer descriptor\n");
  close(timer_descriptor_);
  printf("close signal descriptor\n");
  close(signal_descriptor_);
  printf("close server socket\n");
  server_socket_.Close();
  printf("delete connections\n");
  DeleteConnections();
  printf("release epoll instance\n");
  epoll_instance_.Release();
  running_ = false;
  printf("clean http server shutdown succeeded\n");
}

bool HttpServer::SetupServerSocket(const std::string &service,
                                   const std::string &host) {
  if (!server_socket_.Listen(service, host)) {
    return false;
  }
  server_socket_.Unblock();
  return true;
}

void HttpServer::DeleteConnection(int descriptor) {
  auto it_connection = connections_.find(descriptor);
  if (it_connection == connections_.end()) {
    return;
  }
  printf("delete connection %d\n", it_connection->first);
  epoll_instance_.DeleteDescriptor(it_connection->first);
  delete it_connection->second;
  it_connection = connections_.erase(it_connection);
}

void HttpServer::DeleteConnections() {
  auto it_connection = connections_.begin();
  while (it_connection != connections_.end()) {
    printf("remove connection %d\n", it_connection->first);
    epoll_instance_.DeleteDescriptor(it_connection->first);
    delete it_connection->second;
    it_connection = connections_.erase(it_connection);
  }
  connections_.clear();
}

void HttpServer::DeleteExpiredConnections() {
  auto it_connection = connections_.begin();
  while (it_connection != connections_.end()) {
    if (it_connection->second->GetExpiry() <= TimeEpochMilliseconds()) {
      printf("remove expired connection %d\n", it_connection->first);
      epoll_instance_.DeleteDescriptor(it_connection->first);
      delete it_connection->second;
      it_connection = connections_.erase(it_connection);
    } else {
      it_connection++;
    }
  }
}

void HttpServer::ClearTimer() {
  timer_schedule_.it_interval.tv_sec = 0;
  timer_schedule_.it_interval.tv_nsec = 0;
  timer_schedule_.it_value.tv_sec = 0;
  timer_schedule_.it_value.tv_nsec = 0;
  if (timerfd_settime(timer_descriptor_, 0, &timer_schedule_, 0) == -1) {
    printf("cannot clear timer\n");
  }
}

void HttpServer::ScheduleTimer(long duration) {
  timer_schedule_.it_interval.tv_sec = duration / 1000;
  timer_schedule_.it_interval.tv_nsec = 0;
  timer_schedule_.it_value.tv_sec = duration / 1000;
  timer_schedule_.it_value.tv_nsec = 0;
  if (timerfd_settime(timer_descriptor_, 0, &timer_schedule_, 0) == -1) {
    printf("cannot schedule timer\n");
  }
}

bool HttpServer::IsTimerScheduled() {
  if (timerfd_gettime(timer_descriptor_, &timer_current_) == -1) {
    return false;
  }
  if (timer_current_.it_interval.tv_sec == 0 &&
      timer_current_.it_interval.tv_nsec == 0 &&
      timer_current_.it_value.tv_sec == 0 &&
      timer_current_.it_value.tv_nsec == 0) {
    return false;
  }
  return true;
}
