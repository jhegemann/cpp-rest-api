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

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

const std::string kStringEmpty = "";
const std::string kStringSpace = " ";
const std::string kStringDoubleSpace = "  ";
const std::string kStringLineFeed = "\n";
const std::string kStringCarriageReturn = "\r";
const std::string kStringTab = "\t";
const std::string kStringSlash = "/";
const std::string kStringColon = ":";

bool StringContains(const std::string &text, const std::string &token);
bool StringContains(const std::string &text, const std::string &token,
                    size_t start);
bool StringStartsWith(const std::string &text, const std::string &token);
bool StringStopsWith(const std::string &text, const std::string &token);
std::string StringToLower(const std::string &text);
std::string StringToUpper(const std::string &text);
size_t StringPosition(const std::string &text, const std::string &token,
                      size_t start = 0);
size_t StringPositionNoEscape(const std::string &text, const std::string &token,
                              size_t start = 0);
size_t StringReplace(std::string &text, const std::string &from,
                     const std::string &to);
size_t StringCountTokens(const std::string &text, const std::string &token,
                         size_t start = 0);
size_t StringCountTokensNoEscape(const std::string &text,
                                 const std::string &token, size_t start = 0);
void StringReplaceAll(std::string &text, const std::string &from,
                      const std::string &to);
void StringLtrim(std::string &text, const std::string &token);
void StringRtrim(std::string &text, const std::string &token);
void StringTrim(std::string &text, const std::string &token);
void StringLtrimCharset(std::string &text, const std::string &charset);
void StringRtrimCharset(std::string &text, const std::string &charset);
void StringTrimCharset(std::string &text, const std::string &charset);
std::vector<std::string> StringExplode(const std::string &text,
                                       const std::string &delimiter);
std::string StringImplode(const std::vector<std::string> &segments,
                          const std::string &delimiter);
std::string StringPopSegment(std::string &text, const std::string &delimiter);
std::string StringPopSegment(std::string &text, size_t position);
std::string FileToString(const std::string &filename);
void StringToFile(const std::string &filename, const std::string &content);
long TimeElapsedMilliseconds(struct timeval *from, struct timeval *to);
long TimeEpochMilliseconds();
bool IsDirectory(const std::string &path);
bool IsFile(const std::string &path);
bool FileExists(const std::string &filename);
time_t FileModificationTime(const std::string &filename);
off_t FileSize(const std::string &filename);
void CopyFile(const std::string &from, const std::string &to);
std::vector<std::string> FindFiles(const std::string &directory,
                                   const std::string &pattern = kStringEmpty);
std::string JoinPath(const std::string &directory, const std::string &filename);
bool MakePath(const std::string &path, mode_t mode);
bool UnblockDescriptor(int descriptor);
std::string ExecuteProcess(const std::string &command);
int DaemonizeProcess(const std::string &directory);
uint64_t GetAligned(uint64_t base, uint64_t alignment);
bool IsFlagSet(const uint64_t &base, uint64_t flag);
void SetFlag(uint64_t &base, uint64_t flag);
void UnsetFlag(uint64_t &base, uint64_t flag);

