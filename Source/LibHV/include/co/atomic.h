#pragma once

#ifdef _MSC_VER
#pragma warning (disable:4800)  // disable boring char-to-bool warnings

#include <intrin.h>

#ifndef _WIN64
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#ifdef InterlockedIncrement64
#define _InterlockedIncrement64    InterlockedIncrement64
#endif
#ifdef InterlockedDecrement64
#define _InterlockedDecrement64    InterlockedDecrement64
#endif
#ifdef InterlockedExchangeAdd64
#define _InterlockedExchangeAdd64  InterlockedExchangeAdd64
#endif
#ifdef InterlockedOr64
#define _InterlockedOr64           InterlockedOr64
#endif
#ifdef InterlockedAnd64
#define _InterlockedAnd64          InterlockedAnd64
#endif
#ifdef InterlockedXor64
#define _InterlockedXor64          InterlockedXor64
#endif
#ifdef InterlockedExchange64
#define _InterlockedExchange64     InterlockedExchange64
#endif
#endif

namespace xx {

template<int> struct I {};

template<> struct I<1> {
    typedef char type;
};

template<> struct I<2> {
    typedef short type;
};

template<> struct I<4> {
    typedef long type;
};

template<> struct I<8> {
    typedef long long type;
};

inline char atomic_inc(void* p, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, 1) + 1;
}

inline short atomic_inc(void* p, I<2>) {
    return _InterlockedIncrement16((short*)p);
}

inline long atomic_inc(void* p, I<4>) {
    return _InterlockedIncrement((long*)p);
}

inline long long atomic_inc(void* p, I<8>) {
    return _InterlockedIncrement64((long long*)p);
}

inline char atomic_dec(void* p, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, -1) - 1;
}

inline short atomic_dec(void* p, I<2>) {
    return _InterlockedDecrement16((short*)p);
}

inline long atomic_dec(void* p, I<4>) {
    return _InterlockedDecrement((long*)p);
}

inline long long atomic_dec(void* p, I<8>) {
    return _InterlockedDecrement64((long long*)p);
}

inline char atomic_add(void* p, char v, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, v) + v;
}

inline short atomic_add(void* p, short v, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, v) + v;
}

inline long atomic_add(void* p, long v, I<4>) {
    return _InterlockedExchangeAdd((long*)p, v) + v;
}

inline long long atomic_add(void* p, long long v, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, v) + v;
}

inline char atomic_sub(void* p, char v, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, -v) - v;
}

inline short atomic_sub(void* p, short v, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, -v) - v;
}

inline long atomic_sub(void* p, long v, I<4>) {
    return _InterlockedExchangeAdd((long*)p, -v) - v;
}

inline long long atomic_sub(void* p, long long v, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, -v) - v;
}

inline char atomic_fetch_inc(void* p, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, 1);
}

inline short atomic_fetch_inc(void* p, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, 1);
}

inline long atomic_fetch_inc(void* p, I<4>) {
    return _InterlockedExchangeAdd((long*)p, 1);
}

inline long long atomic_fetch_inc(void* p, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, 1);
}

inline char atomic_fetch_dec(void* p, I<1>) {
    return _InterlockedExchangeAdd8((char*)(p), -1);
}

inline short atomic_fetch_dec(void* p, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, -1);
}

inline long atomic_fetch_dec(void* p, I<4>) {
    return _InterlockedExchangeAdd((long*)(p), -1);
}

inline long long atomic_fetch_dec(void* p, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, -1);
}

inline char atomic_fetch_add(void* p, char v, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, v);
}

inline short atomic_fetch_add(void* p, short v, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, v);
}

inline long atomic_fetch_add(void* p, long v, I<4>) {
    return _InterlockedExchangeAdd((long*)p, v);
}

inline long long atomic_fetch_add(void* p, long long v, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, v);
}

inline char atomic_fetch_sub(void* p, char v, I<1>) {
    return _InterlockedExchangeAdd8((char*)p, -v);
}

inline short atomic_fetch_sub(void* p, short v, I<2>) {
    return _InterlockedExchangeAdd16((short*)p, -v);
}

inline long atomic_fetch_sub(void* p, long v, I<4>) {
    return _InterlockedExchangeAdd((long*)p, -v);
}

inline long long atomic_fetch_sub(void* p, long long v, I<8>) {
    return _InterlockedExchangeAdd64((long long*)p, -v);
}

inline char atomic_or(void* p, char v, I<1>) {
    return _InterlockedOr8((char*)p, v) | v;
}

inline short atomic_or(void* p, short v, I<2>) {
    return _InterlockedOr16((short*)p, v) | v;
}

inline long atomic_or(void* p, long v, I<4>) {
    return _InterlockedOr((long*)p, v) | v;
}

inline long long atomic_or(void* p, long long v, I<8>) {
    return _InterlockedOr64((long long*)p, v) | v;
}

inline char atomic_and(void* p, char v, I<1>) {
    return _InterlockedAnd8((char*)p, v) & v;
}

inline short atomic_and(void* p, short v, I<2>) {
    return _InterlockedAnd16((short*)p, v) & v;
}

inline long atomic_and(void* p, long v, I<4>) {
    return _InterlockedAnd((long*)p, v) & v;
}

inline long long atomic_and(void* p, long long v, I<8>) {
    return _InterlockedAnd64((long long*)p, v) & v;
}

inline char atomic_xor(void* p, char v, I<1>) {
    return _InterlockedXor8((char*)p, v) ^ v;
}

inline short atomic_xor(void* p, short v, I<2>) {
    return _InterlockedXor16((short*)p, v) ^ v;
}

inline long atomic_xor(void* p, long v, I<4>) {
    return _InterlockedXor((long*)p, v) ^ v;
}

inline long long atomic_xor(void* p, long long v, I<8>) {
    return _InterlockedXor64((long long*)p, v) ^ v;
}

inline char atomic_fetch_or(void* p, char v, I<1>) {
    return _InterlockedOr8((char*)p, v);
}

inline short atomic_fetch_or(void* p, short v, I<2>) {
    return _InterlockedOr16((short*)p, v);
}

inline long atomic_fetch_or(void* p, long v, I<4>) {
    return _InterlockedOr((long*)p, v);
}

inline long long atomic_fetch_or(void* p, long long v, I<8>) {
    return _InterlockedOr64((long long*)p, v);
}

inline char atomic_fetch_and(void* p, char v, I<1>) {
    return _InterlockedAnd8((char*)p, v);
}

inline short atomic_fetch_and(void* p, short v, I<2>) {
    return _InterlockedAnd16((short*)p, v);
}

inline long atomic_fetch_and(void* p, long v, I<4>) {
    return _InterlockedAnd((long*)p, v);
}

inline long long atomic_fetch_and(void* p, long long v, I<8>) {
    return _InterlockedAnd64((long long*)p, v);
}

inline char atomic_fetch_xor(void* p, char v, I<1>) {
    return _InterlockedXor8((char*)p, v);
}

inline short atomic_fetch_xor(void* p, short v, I<2>) {
    return _InterlockedXor16((short*)p, v);
}

inline long atomic_fetch_xor(void* p, long v, I<4>) {
    return _InterlockedXor((long*)p, v);
}

inline long long atomic_fetch_xor(void* p, long long v, I<8>) {
    return _InterlockedXor64((long long*)p, v);
}

inline char atomic_swap(void* p, char v, I<1>) {
    return _InterlockedExchange8((char*)p, v);
}

inline short atomic_swap(void* p, short v, I<2>) {
    return _InterlockedExchange16((short*)p, v);
}

inline long atomic_swap(void* p, long v, I<4>) {
    return _InterlockedExchange((long*)p, v);
}

inline long long atomic_swap(void* p, long long v, I<8>) {
    return _InterlockedExchange64((long long*)p, v);
}

inline char atomic_compare_swap(void* p, char o, char v, I<1>) {
    return _InterlockedCompareExchange8((char*)p, v, o);
}

inline short atomic_compare_swap(void* p, short o, short v, I<2>) {
    return _InterlockedCompareExchange16((short*)p, v, o);
}

inline long atomic_compare_swap(void* p, long o, long v, I<4>) {
    return _InterlockedCompareExchange((long*)p, v, o);
}

inline long long atomic_compare_swap(void* p, long long o, long long v, I<8>) {
    return _InterlockedCompareExchange64((long long*)p, v, o);
}

} // namespace xx

template<typename T>
inline T atomic_inc(T* p) {
    typedef xx::I<sizeof(T)> it;
    return (T)xx::atomic_inc(p, it());
}

template<typename T>
inline T atomic_dec(T* p) {
    typedef xx::I<sizeof(T)> it;
    return (T)xx::atomic_dec(p, it());
}

template<typename T, typename V>
inline T atomic_add(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_add(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_sub(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_sub(p, (type)v, it());
}

template<typename T>
inline T atomic_fetch_inc(T* p) {
    typedef xx::I<sizeof(T)> it;
    return (T)xx::atomic_fetch_inc(p, it());
}

template<typename T>
inline T atomic_fetch_dec(T* p) {
    typedef xx::I<sizeof(T)> it;
    return (T)xx::atomic_fetch_dec(p, it());
}

template<typename T, typename V>
inline T atomic_fetch_add(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_fetch_add(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_fetch_sub(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_fetch_sub(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_or(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_or(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_and(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_and(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_xor(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_xor(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_fetch_or(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_fetch_or(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_fetch_and(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_fetch_and(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_fetch_xor(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_fetch_xor(p, (type)v, it());
}

template<typename T, typename V>
inline T atomic_swap(T* p, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_swap(p, (type)v, it());
}

template<typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v) {
    typedef xx::I<sizeof(T)> it;
    typedef typename it::type type;
    return (T)xx::atomic_compare_swap(p, (type)o, (type)v, it());
}

template<typename T>
inline T atomic_get(T* p) {
    return atomic_fetch_xor(p, 0);
}

template<typename T, typename V>
inline void atomic_set(T* p, V v) {
    (void)atomic_swap(p, v);
}

template<typename T>
inline void atomic_reset(T* p) {
    (void)atomic_swap(p, 0);
}

#else /* gcc/clang */

// clang or gcc 4.7+
#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)))

template <typename T>
inline T atomic_inc(T* p) {
    return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T atomic_dec(T* p) {
    return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_add(T* p, V v) {
    return __atomic_add_fetch(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_sub(T* p, V v) {
    return __atomic_sub_fetch(p, v, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T atomic_fetch_inc(T* p) {
    return __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T atomic_fetch_dec(T* p) {
    return __atomic_fetch_sub(p, 1, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_fetch_add(T* p, V v) {
    return __atomic_fetch_add(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_fetch_sub(T* p, V v) {
    return __atomic_fetch_sub(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_or(T* p, V v) {
    return __atomic_or_fetch(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_and(T* p, V v) {
    return __atomic_and_fetch(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_xor(T* p, V v) {
    return __atomic_xor_fetch(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_fetch_or(T* p, V v) {
    return __atomic_fetch_or(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_fetch_and(T* p, V v) {
    return __atomic_fetch_and(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_fetch_xor(T* p, V v) {
    return __atomic_fetch_xor(p, v, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline T atomic_swap(T* p, V v) {
    return __atomic_exchange_n(p, (T)v, __ATOMIC_SEQ_CST);
}

template <typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v) {
    T x = (T)o;
    __atomic_compare_exchange_n(p, &x, (T)v, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return x;
}

template <typename T>
inline T atomic_get(T* p) {
    return __atomic_load_n(p, __ATOMIC_SEQ_CST);
}

template <typename T, typename V>
inline void atomic_set(T* p, V v) {
    __atomic_store_n(p, (T)v, __ATOMIC_SEQ_CST);
}

template <typename T>
inline void atomic_reset(T* p) {
    __atomic_store_n(p, (T)0, __ATOMIC_SEQ_CST);
}

// gcc 4.1+
#elif defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)

template <typename T>
inline T atomic_inc(T* p) {
    return __sync_add_and_fetch(p, 1);
}

template <typename T>
inline T atomic_dec(T* p) {
    return __sync_sub_and_fetch(p, 1);
}

template <typename T, typename V>
inline T atomic_add(T* p, V v) {
    return __sync_add_and_fetch(p, v);
}

template <typename T, typename V>
inline T atomic_sub(T* p, V v) {
    return __sync_sub_and_fetch(p, v);
}

template <typename T>
inline T atomic_fetch_inc(T* p) {
    return __sync_fetch_and_add(p, 1);
}

template <typename T>
inline T atomic_fetch_dec(T* p) {
    return __sync_fetch_and_sub(p, 1);
}

template <typename T, typename V>
inline T atomic_fetch_add(T* p, V v) {
    return __sync_fetch_and_add(p, v);
}

template <typename T, typename V>
inline T atomic_fetch_sub(T* p, V v) {
    return __sync_fetch_and_sub(p, v);
}

template <typename T, typename V>
inline T atomic_or(T* p, V v) {
    return __sync_or_and_fetch(p, v);
}

template <typename T, typename V>
inline T atomic_and(T* p, V v) {
    return __sync_and_and_fetch(p, v);
}

template <typename T, typename V>
inline T atomic_xor(T* p, V v) {
    return __sync_xor_and_fetch(p, v);
}

template <typename T, typename V>
inline T atomic_fetch_or(T* p, V v) {
    return __sync_fetch_and_or(p, v);
}

template <typename T, typename V>
inline T atomic_fetch_and(T* p, V v) {
    return __sync_fetch_and_and(p, v);
}

template <typename T, typename V>
inline T atomic_fetch_xor(T* p, V v) {
    return __sync_fetch_and_xor(p, v);
}

template <typename T, typename V>
inline T atomic_swap(T* p, V v) {
    return __sync_lock_test_and_set(p, (T)v);  // acquire barrier
}

template <typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v) {
    return __sync_val_compare_and_swap(p, (T)o, (T)v);
}

template <typename T>
inline T atomic_get(T* p) {
    return atomic_fetch_or(p, 0);
}

//     |
//     v   ^
//---------|-----  release barrier
//
//---------|-----  acquire barrier
//     ^   v
//     |

template <typename T, typename V>
inline void atomic_set(T* p, V v) {
    __sync_lock_test_and_set(p, v); // acquire barrier
}

template <typename T>
inline void atomic_reset(T* p) {
    __sync_lock_release(p);         // release barrier
}

#else
#error "clang or gcc 4.1+ required"
#endif

#endif
