#pragma once

#define STRINGIZE(arg) STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

#define EXPAND(x) x
#define FOR_EACH_1(what, x, ...) what(x)

#define FOR_EACH_2(what, x, ...)                                                                                                                \
    what(x);                                                                                                                                    \
    EXPAND(FOR_EACH_1(what, __VA_ARGS__))
#define FOR_EACH_3(what, x, ...)                                                                                                                \
    what(x);                                                                                                                                    \
    EXPAND(FOR_EACH_2(what, __VA_ARGS__))
#define FOR_EACH_4(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_3(what,  __VA_ARGS__))
#define FOR_EACH_5(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_4(what,  __VA_ARGS__))
#define FOR_EACH_6(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_5(what,  __VA_ARGS__))
#define FOR_EACH_7(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_6(what,  __VA_ARGS__))
#define FOR_EACH_8(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_7(what,  __VA_ARGS__))
#define FOR_EACH_9(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_8(what,  __VA_ARGS__))
#define FOR_EACH_10(what, x, ...)\
    what(x);\
    EXPAND(FOR_EACH_9(what,  __VA_ARGS__))

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) EXPAND(FOR_EACH_ARG_N(__VA_ARGS__))
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8,_9,_10, N, ...) N
#define FOR_EACH_RSEQ_N() 10,9,8, 7, 6, 5, 4, 3, 2, 1, 0
#define CONCATENATE(x,y) x##y
#define FOR_EACH_(N, what, ...) EXPAND(CONCATENATE(FOR_EACH_, N)(what, __VA_ARGS__))
#define FOR_EACH(what, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)
#define JADDEx_(jsonObj, field) jsonObj.insert(#field, field);
#define JADDEx(field) JADDEx_(root, field)

// Add key value pair into JSON named 'root'
#define JADD(...) FOR_EACH(JADDEx, __VA_ARGS__)

#define RROOT return root;
