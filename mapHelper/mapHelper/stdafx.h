﻿// stdafx.h: 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>



// 在此处引用程序需要的其他标头

#include <string>
#include <map>
#include <vector>
#include <inttypes.h>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#ifdef _HAS_CXX17
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;

#endif

#include "fp_call.h"
#include "unicode.h"
#include "BinaryWriter.h"


std::string string_replaced(const std::string& source, const std::string& from, const std::string& to);
void replace_string(std::string::iterator begin, std::string::iterator end);