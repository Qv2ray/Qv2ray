#pragma once

#define STRINGIZE(arg) STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

// Add key value pair into JSON named 'root'
#define JADDEx(field) root.insert(#field, field);
#define JADD(...) FOR_EACH(JADDEx, __VA_ARGS__)

#define JAUTOREMOVE(jObj, key)                                                                                                                  \
    {                                                                                                                                           \
        if ((jObj[key].isArray() && jObj[key].toArray().isEmpty()) || (jObj[key].isObject() && jObj[key].toObject().isEmpty()) ||               \
            (jObj[key].isString() && jObj[key].toString().isEmpty()))                                                                           \
            jObj.remove(key);                                                                                                                   \
    }
