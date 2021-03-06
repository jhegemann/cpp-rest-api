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

#include "tcp.h"

EpollInstance::EpollInstance() {}

EpollInstance::~EpollInstance() {}

bool EpollInstance::Create() {
  instance_ = epoll_create1(0);
  if (instance_ == -1) {
    return false;
  }
  return true;
}

void EpollInstance::Release() { close(instance_); }

int EpollInstance::Wait(long timeout) {
  return epoll_wait(instance_, events_, kMaximumEvents, timeout);
}

bool EpollInstance::AddDescriptor(int descriptor, int flags) {
  event_.events = flags | EPOLLERR | EPOLLHUP;
  event_.data.fd = descriptor;
  if (epoll_ctl(instance_, EPOLL_CTL_ADD, descriptor, &event_) == -1) {
    return false;
  }
  return true;
}

bool EpollInstance::AddReadableDescriptor(int descriptor) {
  return AddDescriptor(descriptor, EPOLLIN);
}

bool EpollInstance::AddWritableDescriptor(int descriptor) {
  return AddDescriptor(descriptor, EPOLLOUT);
}

bool EpollInstance::AddDuplexDescriptor(int descriptor) {
  return AddDescriptor(descriptor, EPOLLIN | EPOLLOUT);
}

bool EpollInstance::DeleteDescriptor(int descriptor) {
  if (epoll_ctl(instance_, EPOLL_CTL_DEL, descriptor, NULL) == -1) {
    return false;
  }
  return true;
}

bool EpollInstance::ModifyDescriptor(int descriptor, int flags) {
  event_.events = flags;
  event_.data.fd = descriptor;
  if (epoll_ctl(instance_, EPOLL_CTL_MOD, descriptor, &event_) == -1) {
    return false;
  }
  return true;
}

int EpollInstance::GetDescriptor(size_t index) {
  if (index >= kMaximumEvents) {
    return -1;
  }
  return events_[index].data.fd;
}

int EpollInstance::GetEvents(size_t index) {
  if (index >= kMaximumEvents) {
    return -1;
  }
  return events_[index].events;
}

bool EpollInstance::IsReadable(size_t index) {
  if (GetEvents(index) == -1) {
    return false;
  }
  return GetEvents(index) & EPOLLIN;
}

bool EpollInstance::IsWritable(size_t index) {
  if (GetEvents(index) == -1) {
    return false;
  }
  return GetEvents(index) & EPOLLOUT;
}

bool EpollInstance::HasErrors(size_t index) {
  if (GetEvents(index) == -1) {
    return false;
  }
  return GetEvents(index) & EPOLLERR || GetEvents(index) & EPOLLHUP;
}

bool EpollInstance::SetReadable(size_t index) {
  return ModifyDescriptor(GetDescriptor(index), EPOLLIN | EPOLLERR | EPOLLHUP);
}

bool EpollInstance::SetWriteable(size_t index) {
  return ModifyDescriptor(GetDescriptor(index), EPOLLOUT | EPOLLERR | EPOLLHUP);
}

bool EpollInstance::SetDuplex(size_t index) {
  return ModifyDescriptor(GetDescriptor(index),
                          EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP);
}

TcpSocket::TcpSocket()
    : host_(kStringEmpty), service_(kStringEmpty), descriptor_(-1),
      listening_(false), connected_(false) {}

TcpSocket::~TcpSocket() { Close(); }

void TcpSocket::Close() {
  if (listening_ || connected_ || descriptor_ != -1) {
    close(descriptor_);
  }
  descriptor_ = -1;
  listening_ = false;
  connected_ = false;
  host_ = kStringEmpty;
  service_ = kStringEmpty;
}

const std::string &TcpSocket::GetHost() const { return host_; }

const std::string &TcpSocket::GetService() const { return service_; }

const int TcpSocket::GetDescriptor() const { return descriptor_; }

bool TcpSocket::WaitReceive(long timeout) {
  struct pollfd event;
  event.fd = descriptor_;
  event.events = POLLIN | POLLHUP | POLLERR;
  int ready = poll(&event, 1, timeout);
  if (event.revents & POLLHUP || event.revents & POLLERR) {
    return false;
  }
  if (ready > 0 && event.revents & POLLIN) {
    return true;
  }
  return false;
}

bool TcpSocket::WaitSend(long timeout) {
  struct pollfd event;
  event.fd = descriptor_;
  event.events = POLLOUT | POLLHUP | POLLERR;
  int ready = poll(&event, 1, timeout);
  if (event.revents & POLLHUP || event.revents & POLLERR) {
    return false;
  }
  if (ready > 0 && event.revents & POLLOUT) {
    return true;
  }
  return false;
}

bool TcpSocket::IsConnected() { return connected_; }

bool TcpSocket::Connect(const std::string &service, const std::string &host) {
  Close();
  struct addrinfo hints;
  struct addrinfo *result, *iter;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
    return false;
  }
  int sfd;
  for (iter = result; iter != nullptr; iter = iter->ai_next) {
    if ((sfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) ==
        -1) {
      continue;
    }
    if (connect(sfd, iter->ai_addr, iter->ai_addrlen) != -1) {
      break;
    }
    close(sfd);
  }
  freeaddrinfo(result);
  descriptor_ = (iter == nullptr) ? -1 : sfd;
  if (descriptor_ == -1) {
    return false;
  }
  host_ = host;
  service_ = service;
  connected_ = true;
  return true;
}

bool TcpSocket::IsListening() { return listening_; }

bool TcpSocket::Listen(const std::string &service, const std::string &host) {
  Close();
  struct addrinfo hints;
  struct addrinfo *result, *iter;
  memset(&hints, 0, sizeof(struct addrinfo));
  socklen_t *addrlen = nullptr;
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
    return false;
  }
  int option_value = 1;
  int sfd;
  for (iter = result; iter != nullptr; iter = iter->ai_next) {
    if ((sfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) ==
        -1) {
      continue;
    }
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &option_value,
                   sizeof(option_value)) == -1) {
      close(sfd);
      freeaddrinfo(result);
      return false;
    }
    if (bind(sfd, iter->ai_addr, iter->ai_addrlen) == 0) {
      break;
    }
    close(sfd);
  }
  if (iter != nullptr) {
    if (listen(sfd, SOMAXCONN) == -1) {
      freeaddrinfo(result);
      return false;
    }
  }
  if (iter != nullptr && addrlen != nullptr) {
    *addrlen = iter->ai_addrlen;
  }
  freeaddrinfo(result);
  descriptor_ = (iter == nullptr) ? -1 : sfd;
  if (descriptor_ == -1) {
    return false;
  }
  host_ = kTcpLocalHost;
  service_ = service;
  listening_ = true;
  return true;
}

bool TcpSocket::IsBlocking() {
  int flags = fcntl(descriptor_, F_GETFL, 0);
  if (flags == -1) {
    return false;
  }
  return !(flags & O_NONBLOCK);
}

bool TcpSocket::Unblock() {
  int flags = fcntl(descriptor_, F_GETFL, 0);
  if (flags == -1) {
    return false;
  }
  flags |= O_NONBLOCK;
  int err = fcntl(descriptor_, F_SETFL, flags);
  if (err == -1) {
    return false;
  }
  return true;
}

bool TcpSocket::Block() {
  int flags = fcntl(descriptor_, F_GETFL, 0);
  if (flags == -1) {
    return false;
  }
  flags &= ~O_NONBLOCK;
  int err = fcntl(descriptor_, F_SETFL, flags);
  if (err == -1) {
    return false;
  }
  return true;
}

bool TcpSocket::IsGood() {
  int err;
  int option_value;
  socklen_t option_length = sizeof(int);
  err = getsockopt(descriptor_, SOL_SOCKET, SO_ERROR, &option_value,
                   &option_length);
  if (err || option_value) {
    return false;
  }
  return true;
}

TcpSocket *TcpSocket::Accept() {
  if (!IsListening() || !IsGood()) {
    return nullptr;
  }
  struct sockaddr address;
  socklen_t address_length = sizeof(address);
  memset(&address, 0, address_length);
  int cfd = accept(descriptor_, &address, &address_length);
  if (cfd == -1) {
    return nullptr;
  }
  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  if (getnameinfo(&address, address_length, host, sizeof(host), service,
                  sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
    return nullptr;
  }
  TcpSocket *client = new TcpSocket();
  client->descriptor_ = cfd;
  client->host_ = host;
  client->service_ = service;
  client->listening_ = false;
  client->connected_ = true;
  return client;
}

IoStatusCode TcpSocket::Receive(std::string &payload, long timeout) {
  if (IsBlocking()) {
    return SOCKET_FLAGS;
  }
  if (!IsConnected()) {
    return NOT_CONNECTED;
  }
  if (!IsGood()) {
    return BAD;
  }
  ssize_t bytes;
  ssize_t length;
  char buffer[kTcpReceiveBufferSize];
  long start = TimeEpochMilliseconds();
  for (;;) {
    length = std::min(kTcpReceiveBufferSize,
                      kTcpMaximumPayloadSize - (long)payload.size());
    bytes = recv(descriptor_, buffer, length, 0);
    switch (bytes) {
    case -1:
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        if (timeout == 0) {
          return BLOCKED;
        }
        if (TimeEpochMilliseconds() - start >= timeout) {
          return TIMEOUT;
        }
        usleep(timeout * 100);
        continue;
      }
      if (errno == EINTR) {
        if (timeout == 0) {
          return INTERRUPTED;
        }
        if (TimeEpochMilliseconds() - start >= timeout) {
          return TIMEOUT;
        }
        continue;
      }
      return ERROR;
    case 0:
      return DISCONNECT;
    default:
      payload.insert(payload.end(), &buffer[0], &buffer[bytes]);
      if (payload.size() >= kTcpMaximumPayloadSize) {
        return OVERFLOW;
      }
      if (timeout == 0) {
        return SUCCESS;
      }
      if (TimeEpochMilliseconds() - start >= timeout) {
        return TIMEOUT;
      }
      continue;
    }
  }
}

IoStatusCode TcpSocket::Send(std::string &payload, long timeout) {
  if (IsBlocking()) {
    return SOCKET_FLAGS;
  }
  if (!IsConnected()) {
    return NOT_CONNECTED;
  }
  if (!IsGood()) {
    return BAD;
  }
  if (payload.size() > kTcpMaximumPayloadSize) {
    return OVERFLOW;
  }
  ssize_t bytes;
  ssize_t length;
  long start = TimeEpochMilliseconds();
  for (;;) {
    length = std::min(kTcpSendBufferSize, (long)payload.size());
    bytes = send(descriptor_, &payload[0], length, 0);
    switch (bytes) {
    case -1:
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        if (timeout == 0) {
          return BLOCKED;
        }
        if (TimeEpochMilliseconds() - start >= timeout) {
          return TIMEOUT;
        }
        usleep(timeout * 100);
        continue;
      }
      if (errno == EINTR) {
        if (timeout == 0) {
          return INTERRUPTED;
        }
        if (TimeEpochMilliseconds() - start >= timeout) {
          return TIMEOUT;
        }
        continue;
      }
      return ERROR;
    case 0:
      return ERROR;
    default:
      payload.erase(0, bytes);
      if (payload.empty()) {
        return SUCCESS;
      }
      if (TimeEpochMilliseconds() - start >= timeout) {
        return TIMEOUT;
      }
      continue;
    }
  }
}

TcpReader::TcpReader(TcpSocket *socket)
    : socket_(socket), buffer_(kStringEmpty), status_(NONE) {}

TcpReader::~TcpReader() {}

void TcpReader::ReadUntil(const std::string &token, long max_idle) {
  size_t start = 0;
  while (!StringContains(buffer_, token, start)) {
    start = buffer_.size();
    if (!socket_->WaitReceive(max_idle)) {
      status_ = EMPTY_BUFFER;
      break;
    }
    status_ = socket_->Receive(buffer_);
    if (HasErrors()) {
      break;
    }
  }
}

void TcpReader::ReadUntil(size_t length, long max_idle) {
  while (buffer_.length() < length) {
    if (!socket_->WaitReceive(max_idle)) {
      status_ = EMPTY_BUFFER;
      break;
    }
    status_ = socket_->Receive(buffer_);
    if (HasErrors()) {
      break;
    }
  }
}

bool TcpReader::HasErrors() { return status_ != SUCCESS && status_ != BLOCKED; }

void TcpReader::ReadSome(long timeout) {
  status_ = socket_->Receive(buffer_, timeout);
}

std::string TcpReader::PopSegment(const std::string &token) {
  return StringPopSegment(buffer_, token);
}

std::string TcpReader::PopSegment(size_t position) {
  return StringPopSegment(buffer_, position);
}

size_t TcpReader::GetPosition(const std::string &token) {
  return StringPosition(buffer_, token);
}

std::string TcpReader::PopAll() {
  std::string temp = buffer_;
  buffer_.clear();
  return temp;
}

IoStatusCode TcpReader::GetStatus() { return status_; }

bool TcpReader::IsInBuffer(const std::string &token) {
  return StringContains(buffer_, token);
}

void TcpReader::ClearBuffer() { buffer_.clear(); }

const std::string &TcpReader::GetBuffer() { return buffer_; }

TcpWriter::TcpWriter(TcpSocket *socket)
    : socket_(socket), buffer_(kStringEmpty), status_(NONE) {}

TcpWriter::~TcpWriter() {}

void TcpWriter::Write(const std::string &payload) {
  buffer_.insert(buffer_.end(), payload.begin(), payload.end());
}

void TcpWriter::Send() {
  while (!buffer_.empty()) {
    if (!socket_->WaitSend(kTcpTimeout)) {
      break;
    }
    status_ = socket_->Send(buffer_);
    if (HasErrors()) {
      break;
    }
  }
}

bool TcpWriter::HasErrors() {
  return status_ != SUCCESS && status_ != BLOCKED;
}

void TcpWriter::SendSome() { status_ = socket_->Send(buffer_); }

IoStatusCode TcpWriter::GetStatus() { return status_; }

bool TcpWriter::IsEmpty() { return buffer_.empty(); }
