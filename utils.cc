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

#include "utils.h"

bool StringContains(const std::string &text, const std::string &token) {
  if (text.find(token) != std::string::npos) {
    return true;
  }
  return false;
}

bool StringContains(const std::string &text, const std::string &token,
                    size_t start) {
  if (start >= text.size()) {
    return false;
  }
  if (text.find(token, start) != std::string::npos) {
    return true;
  }
  return false;
}

bool StringStartsWith(const std::string &text, const std::string &token) {
  if (text.find(token) == 0) {
    return true;
  }
  return false;
}

bool StringStopsWith(const std::string &text, const std::string &token) {
  if (text.rfind(token) == 0) {
    return true;
  }
  return false;
}

std::string StringToLower(const std::string &text) {
  std::string lowercase = text;
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lowercase;
}

std::string StringToUpper(const std::string &text) {
  std::string lowercase = text;
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return lowercase;
}

size_t StringPosition(const std::string &text, const std::string &token,
                      size_t start) {
  return text.find(token, start);
}

size_t StringPositionNoEscape(const std::string &text, const std::string &token,
                              size_t start) {
  size_t position = start;
  while ((position = StringPosition(text, token, position)) !=
         std::string::npos) {
    if (position == 0 || (position > 0 && text[position - 1] != '\\')) {
      return position;
    }
    position++;
  }
  return std::string::npos;
}

size_t StringReplace(std::string &text, const std::string &from,
                     const std::string &to) {
  size_t offset = 0;
  size_t number = 0;
  size_t position = std::string::npos;
  while ((position = text.find(from, offset)) != std::string::npos) {
    text.replace(position, from.length(), to);
    number++;
    offset = position + to.length();
  }
  return number;
}

size_t StringCountTokens(const std::string &text, const std::string &token,
                         size_t start) {
  size_t position = start;
  size_t counter = 0;
  while ((position = StringPosition(text, token, position)) !=
         std::string::npos) {
    position++;
    counter++;
  }
  return counter;
}

size_t StringCountTokensNoEscape(const std::string &text,
                                 const std::string &token, size_t start) {
  size_t position = start;
  size_t counter = 0;
  while ((position = StringPosition(text, token, position)) !=
         std::string::npos) {
    if (position == 0 || (position > 0 && text[position - 1] != '\\')) {
      counter++;
    }
    position++;
  }
  return counter;
}

void StringReplaceAll(std::string &text, const std::string &from,
                      const std::string &to) {
  if (StringContains(to, from)) {
    return;
  }
  while (StringReplace(text, from, to) > 0) {
    continue;
  }
}

void StringLtrim(std::string &text, const std::string &token) {
  text.erase(0, text.find_first_not_of(token));
}

void StringRtrim(std::string &text, const std::string &token) {
  text.erase(text.find_last_not_of(token) + 1);
}

void StringTrim(std::string &text, const std::string &token) {
  StringLtrim(text, token);
  StringRtrim(text, token);
}

void StringLtrimCharset(std::string &text, const std::string &charset) {
  size_t counter = 0;
  bool done = false;
  while (counter < text.length() && !done) {
    bool match = false;
    for (size_t i = 0; i < charset.length(); i++) {
      if (text[counter] == charset[i]) {
        match = true;
        break;
      }
    }
    if (match) {
      counter++;
    } else {
      done = true;
    }
  }
  text = text.substr(counter);
}

void StringRtrimCharset(std::string &text, const std::string &charset) {
  size_t counter = 0;
  bool done = false;
  while (counter < text.length() && !done) {
    bool match = false;
    size_t position = text.length() - 1 - counter;
    for (size_t i = 0; i < charset.length(); i++) {
      if (text[position] == charset[i]) {
        match = true;
        break;
      }
    }
    if (match) {
      counter++;
    } else {
      done = true;
    }
  }
  text = text.substr(0, text.length() - counter);
}

void StringTrimCharset(std::string &text, const std::string &charset) {
  StringLtrimCharset(text, charset);
  StringRtrimCharset(text, charset);
}

std::vector<std::string> StringExplode(const std::string &text,
                                       const std::string &delimiter) {
  std::vector<std::string> parts;
  std::string copy = text;
  StringTrim(copy, delimiter);
  size_t position;
  std::string segment;
  while ((position = copy.find(delimiter)) != std::string::npos) {
    segment = copy.substr(0, position);
    parts.push_back(segment);
    copy = copy.substr(position + delimiter.length());
  }
  segment = copy;
  parts.push_back(segment);
  std::vector<std::string>::iterator iter = parts.begin();
  while (iter != parts.end()) {
    if (iter->empty()) {
      iter = parts.erase(iter);
      continue;
    }
    iter++;
  }
  return parts;
}

std::string StringImplode(const std::vector<std::string> &segments,
                          const std::string &delimiter) {
  std::string joined(segments[0]);
  for (size_t i = 1; i < segments.size(); i++) {
    joined += delimiter;
    joined += segments[i];
  }
  return joined;
}

std::string StringPopSegment(std::string &text, const std::string &delimiter) {
  size_t position = text.find(delimiter);
  if (position == std::string::npos) {
    return kStringEmpty;
  }
  std::string segment = text.substr(0, position);
  text = text.substr(position + delimiter.length());
  return segment;
}

std::string StringPopSegment(std::string &text, size_t position) {
  if (position == std::string::npos) {
    return kStringEmpty;
  }
  if (position >= text.length()) {
    std::string segment = text;
    text = std::string();
    return segment;
  }
  std::string segment = text.substr(0, position);
  text = text.substr(position + 1);
  return segment;
}

std::string FileToString(const std::string &filename) {
  static const size_t chunk_size = 4096;
  char buffer[chunk_size];
  std::string content;
  FILE *stream = fopen(filename.c_str(), "r");
  if (!stream) {
    return kStringEmpty;
  }
  size_t bytes_read;
  while ((bytes_read = fread(buffer, sizeof(char), chunk_size, stream)) > 0) {
    content.insert(content.length(), buffer, bytes_read);
  }
  fclose(stream);
  return content;
}

void StringToFile(const std::string &filename, const std::string &content) {
  static const size_t chunk_size = 4096;
  FILE *stream = fopen(filename.c_str(), "w");
  if (!stream) {
    return;
  }
  size_t total_written = 0;
  size_t bytes_written;
  while (total_written < content.length()) {
    if ((bytes_written =
             fwrite(&content[total_written], sizeof(char),
                    std::min(chunk_size, content.length() - total_written),
                    stream)) < 1) {
      break;
    }
    total_written += bytes_written;
  }
  fclose(stream);
}

long TimeElapsedMilliseconds(struct timeval *from, struct timeval *to) {
  static const suseconds_t sec = 1000000;
  if (from->tv_sec > to->tv_sec) {
    return 0;
  }
  if (from->tv_sec == to->tv_sec) {
    if (from->tv_usec > to->tv_usec) {
      return 0;
    }
    return (to->tv_usec - from->tv_usec) / 1000;
  }
  return ((sec - from->tv_usec) + (to->tv_sec - from->tv_sec - 1) * sec +
          (to->tv_usec)) /
         1000;
}

long TimeEpochMilliseconds() {
  struct timeval epoch;
  gettimeofday(&epoch, nullptr);
  return epoch.tv_sec * 1000 + epoch.tv_usec / 1000;
}

bool IsDirectory(const std::string &path) {
  struct stat info;
  if (stat(path.c_str(), &info) == 0 && info.st_mode & S_IFDIR) {
    return true;
  }
  return false;
}

bool IsFile(const std::string &path) {
  struct stat info;
  if (stat(path.c_str(), &info) == 0 && info.st_mode & S_IFREG) {
    return true;
  }
  return false;
}

bool FileExists(const std::string &filename) {
  if (access(filename.c_str(), F_OK) != -1) {
    return true;
  }
  return false;
}

time_t FileModificationTime(const std::string &filename) {
  struct stat info;
  if (stat(filename.c_str(), &info) != 0) {
    return -1;
  }
  return info.st_mtim.tv_sec;
}

off_t FileSize(const std::string &filename) {
  struct stat info;
  if (stat(filename.c_str(), &info) != 0) {
    return (off_t)-1;
  }
  return info.st_size;
}

void CopyFile(const std::string &from, const std::string &to) {
  static const size_t chunk_size = 4096;
  char buffer[chunk_size];
  ssize_t bytes_read;
  ssize_t bytes_written;
  FILE *source = fopen(from.c_str(), "r");
  FILE *destination = fopen(to.c_str(), "w");
  while ((bytes_read = fread(buffer, sizeof(char), chunk_size, source)) > 0) {
    bytes_written = fwrite(buffer, sizeof(char), bytes_read, destination);
    if (bytes_written == -1) {
      break;
    }
  }
  fclose(source);
  fclose(destination);
}

std::vector<std::string> FindFiles(const std::string &directory,
                                   const std::string &pattern) {
  std::vector<std::string> files;
  struct dirent **namelist;
  if (!IsDirectory(directory)) {
    return files;
  }
  int number = scandir(directory.c_str(), &namelist, nullptr, alphasort);
  if (number == -1) {
    return files;
  }
  for (int i = 0; i < number; i++) {
    std::string filename = namelist[i]->d_name;
    if (StringContains(filename, pattern)) {
      files.push_back(JoinPath(directory, filename));
    }
    free(namelist[i]);
  }
  free(namelist);
  return files;
}

std::string JoinPath(const std::string &directory,
                     const std::string &filename) {
  std::string folder = directory;
  StringRtrim(folder, kStringSlash);
  std::string file = filename;
  StringLtrim(file, kStringSlash);
  return folder + kStringSlash + file;
}

bool MakePath(const std::string &path, mode_t mode) {
  std::string copy = path;
  StringRtrim(copy, kStringSlash);
  int error = mkdir(copy.c_str(), mode);
  if (error == 0) {
    return true;
  }
  if (errno == ENOENT) {
    size_t position = copy.find_last_of(kStringSlash);
    if (position == std::string::npos) {
      return false;
    }
    if (!MakePath(copy.substr(0, position), mode)) {
      return false;
    }
    return 0 == mkdir(copy.c_str(), mode);
  } else if (errno == EEXIST) {
    return IsDirectory(copy);
  } else {
    return false;
  }
}

bool UnblockDescriptor(int descriptor) {
  int flags = fcntl(descriptor, F_GETFL, 0);
  if (flags == -1) {
    return false;
  }
  flags |= O_NONBLOCK;
  int error = fcntl(descriptor, F_SETFL, flags);
  if (error == -1) {
    return false;
  }
  return true;
}

std::string ExecuteProcess(const std::string &command) {
  static const size_t chunk_size = 4096;
  char buffer[chunk_size];
  std::string output;
  FILE *stream = popen(command.c_str(), "r");
  if (!stream) {
    return kStringEmpty;
  }
  size_t bytes_read;
  while ((bytes_read = fread(buffer, sizeof(char), chunk_size, stream)) > 0) {
    output.insert(output.length(), buffer, bytes_read);
  }
  int err __attribute__((unused));
  err = pclose(stream);
  return output;
}

int DaemonizeProcess(const std::string &directory) {
  static const std::string null_device = "/dev/null";
  static const std::string current_directory = ".";
  if (!IsDirectory(directory)) {
    return -1;
  }
  switch (fork()) {
  case -1:
    return -1;
  case 0:
    break;
  default:
    _exit(EXIT_SUCCESS);
  }
  if (setsid() == -1) {
    return -1;
  }
  switch (fork()) {
  case -1:
    return -1;
  case 0:
    break;
  default:
    _exit(EXIT_SUCCESS);
  }
  umask(0);
  if (strncmp(directory.c_str(), current_directory.c_str(), 1) != 0) {
    if (chdir(directory.c_str()) == -1) {
      return -1;
    }
  }
  int maxfd = sysconf(_SC_OPEN_MAX);
  if (maxfd == -1) {
    maxfd = 8192;
  }
  for (int fd = 0; fd < maxfd; fd++) {
    close(fd);
  }
  close(STDIN_FILENO);
  int fd = open(null_device.c_str(), O_RDWR);
  if (fd != STDIN_FILENO) {
    return -1;
  }
  if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
    return -1;
  }
  if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
    return -1;
  }
  return 0;
}

uint64_t GetAligned(uint64_t base, uint64_t alignment) {
  return base - (base & alignment);
}

bool IsFlagSet(const uint64_t &base, uint64_t flag) { return base & flag; }

void SetFlag(uint64_t &base, uint64_t flag) { base |= flag; }

void UnsetFlag(uint64_t &base, uint64_t flag) { base &= ~flag; }
