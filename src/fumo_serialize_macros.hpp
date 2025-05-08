#pragma once

#include "cereal/cereal.hpp"
#define CONCATENATE(arg1, arg2) CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

#define srlzFOR_EACH_1(what, x) what(x)

#define srlzFOR_EACH_2(what, x, ...) what(x), srlzFOR_EACH_1(what, __VA_ARGS__)

#define srlzFOR_EACH_3(what, x, ...) what(x), srlzFOR_EACH_2(what, __VA_ARGS__)

#define srlzFOR_EACH_4(what, x, ...) what(x), srlzFOR_EACH_3(what, __VA_ARGS__)

#define srlzFOR_EACH_5(what, x, ...) what(x), srlzFOR_EACH_4(what, __VA_ARGS__)

#define srlzFOR_EACH_6(what, x, ...) what(x), srlzFOR_EACH_5(what, __VA_ARGS__)

#define srlzFOR_EACH_7(what, x, ...) what(x), srlzFOR_EACH_6(what, __VA_ARGS__)

#define srlzFOR_EACH_8(what, x, ...) what(x), srlzFOR_EACH_7(what, __VA_ARGS__)
#define srlzFOR_EACH_9(what, x, ...) what(x), srlzFOR_EACH_8(what, __VA_ARGS__)
#define srlzFOR_EACH_10(what, x, ...) what(x), srlzFOR_EACH_9(what, __VA_ARGS__)
#define srlzFOR_EACH_11(what, x, ...) what(x), srlzFOR_EACH_10(what, __VA_ARGS__)

#define srlzFOR_EACH_NARG(...) \
    srlzFOR_EACH_NARG_(__VA_ARGS__, srlzFOR_EACH_RSEQ_N())
#define srlzFOR_EACH_NARG_(...) srlzFOR_EACH_ARG_N(__VA_ARGS__)
#define srlzFOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define srlzFOR_EACH_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define srlzFOR_EACH_(N, what, ...) \
    CONCATENATE(srlzFOR_EACH_, N)(what, __VA_ARGS__)

#define srlzFOR_EACH(what, ...) \
    srlzFOR_EACH_(srlzFOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define srlzINNER(x) cereal::make_nvp(std::string(#x), x)

#define SERIALIZE(...) \
    template<class Archive> \
    void serialize(Archive& archive) { \
        archive(srlzFOR_EACH(srlzINNER, __VA_ARGS__)); \
    }

// #define SERIALIZE(...) \
//     template<class Archive> \
//     void serialize(Archive& archive) { \
//         archive(__VA_ARGS__); \
//     }
