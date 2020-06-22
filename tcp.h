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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "utils.h"

const unsigned kMaximumEvents = 256;

class EpollInstance {
public:
  EpollInstance();
  virtual ~EpollInstance();
  bool Create();
  void Release();
  int Wait(long timeout = -1);
  bool AddDescriptor(int descriptor, int flags);
  bool AddReadableDescriptor(int descriptor);
  bool AddWritableDescriptor(int descriptor);
  bool AddDuplexDescriptor(int descriptor);
  bool DeleteDescriptor(int descriptor);
  bool ModifyDescriptor(int descriptor, int flags);
  bool SetReadable(size_t index);
  bool SetWriteable(size_t index);
  bool SetDuplex(size_t index);
  int GetDescriptor(size_t index);
  int GetEvents(size_t index);
  bool IsReadable(size_t index);
  bool IsWritable(size_t index);
  bool HasErrors(size_t index);

private:
  int instance_;
  epoll_event event_;
  epoll_event events_[kMaximumEvents];
};

const std::string kTcpLocalHost = "127.0.0.1";
const long kTcpReceiveBufferSize = 65536L;
const long kTcpSendBufferSize = 65536L;
const long kTcpMaximumPayloadSize = 16777216L;
const long kTcpTimeout = 1000L;

enum IoStatusCode {
  SUCCESS = 0,
  NONE,
  ERROR,
  DISCONNECT,
  BLOCKED,
  TIMEOUT,
  OVERFLOW,
  SOCKET_FLAGS,
  NOT_LISTENING,
  NOT_CONNECTED,
  BAD,
  INTERRUPTED,
  EMPTY_BUFFER
};

class TcpSocket {
public:
  TcpSocket();
  virtual ~TcpSocket();
  void Close();
  const std::string &GetHost() const;
  const std::string &GetService() const;
  const int GetDescriptor() const;
  bool WaitReceive(long timeout = 0);
  bool WaitSend(long timeout = 0);
  bool IsConnected();
  bool Connect(const std::string &service, const std::string &host);
  bool IsListening();
  bool Listen(const std::string &service, const std::string &host);
  bool IsBlocking();
  bool Unblock();
  bool Block();
  bool IsGood();
  TcpSocket *Accept();
  IoStatusCode Receive(std::string &payload, long timeout = 0);
  IoStatusCode Send(std::string &payload, long timeout = 0);

private:
  std::string host_;
  std::string service_;
  int descriptor_;
  bool listening_;
  bool connected_;
};

class TcpReader {
public:
  TcpReader(TcpSocket *socket);
  virtual ~TcpReader();
  void ReadUntil(const std::string &token, long max_idle = kTcpTimeout);
  void ReadUntil(size_t length, long max_idle = kTcpTimeout);
  void ReadSome(long timeout = 0);
  IoStatusCode GetStatus();
  std::string PopSegment(const std::string &token);
  std::string PopSegment(size_t position);
  size_t GetPosition(const std::string &token);
  std::string PopAll();
  bool IsInBuffer(const std::string &token);
  void ClearBuffer();
  const std::string& GetBuffer();
  bool HasErrors();

private:
  std::string buffer_;
  TcpSocket *socket_;
  IoStatusCode status_;
  size_t position_;
};

class TcpWriter {
public:
  TcpWriter(TcpSocket *socket);
  virtual ~TcpWriter();
  void Write(const std::string &payload);
  void Send();
  void SendSome();
  IoStatusCode GetStatus();
  bool IsEmpty();
  bool HasErrors();

private:
  std::string buffer_;
  TcpSocket *socket_;
  IoStatusCode status_;
};

