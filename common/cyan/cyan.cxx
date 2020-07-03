/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Sayan Chaliha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#include <cyan/config.h>
#include <cyan/cyan.h>

#define _STR(X) #X
#define STR(X) _STR(X)

namespace cyan::version {

std::int_least8_t major() {
  return CYAN_VERSION_MAJOR;
}

std::int_least8_t minor() {
  return CYAN_VERSION_MINOR;
}

std::int_least8_t patch() {
  return CYAN_VERSION_PATCH;
}

std::int_least8_t tweak() {
    return CYAN_VERSION_TWEAK;
}

char const* str() {
  return "" STR(CYAN_VERSION_MAJOR) "."
         STR(CYAN_VERSION_MINOR) "."
         STR(CYAN_VERSION_PATCH) "."
         STR(CYAN_VERSION_TWEAK);
}

wchar_t const* wstr() {
  return L"" STR(CYAN_VERSION_MAJOR) "."
         STR(CYAN_VERSION_MINOR) "."
         STR(CYAN_VERSION_PATCH) "."
         STR(CYAN_VERSION_TWEAK);
}

}

