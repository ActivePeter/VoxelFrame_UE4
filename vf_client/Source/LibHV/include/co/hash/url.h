#pragma once

#pragma once

#include "../fastring.h"
#include <string>

/**
 * url encode 
 *   - The following characters will not be encoded: 
 *     - reserved characters :  ! ( ) * # $ & ' + , / : ; = ? @ [ ] 
 *     - a-z  A-Z  0-9  - _ . ~ 
 *
 * @param s  a pointer to the data to be encoded.
 * @param n  size of the data.
 *
 * @return   a url-encoded string.
 */
__codec fastring url_encode(const void* s, size_t n);

/**
 * url decode
 *   - This function will return an empty string in two cases:
 *     - Size of the input data is 0.
 *     - The input data is not a valid url-encoded string.
 *
 * @param s  a pointer to the data to be decoded.
 * @param n  size of the data.
 *
 * @return   a decoded string on success, or an empty string on any error.
 */
__codec fastring url_decode(const void* s, size_t n);

inline fastring url_encode(const char* s) {
    return url_encode(s, strlen(s));
}

inline fastring url_encode(const fastring& s) {
    return url_encode(s.data(), s.size());
}

inline fastring url_encode(const std::string& s) {
    return url_encode(s.data(), s.size());
}

inline fastring url_decode(const char* s) {
    return url_decode(s, strlen(s));
}

inline fastring url_decode(const fastring& s) {
    return url_decode(s.data(), s.size());
}

inline fastring url_decode(const std::string& s) {
    return url_decode(s.data(), s.size());
}
