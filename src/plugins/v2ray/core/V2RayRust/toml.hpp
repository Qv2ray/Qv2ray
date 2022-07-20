//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v3.0.1
// https://github.com/marzer/tomlplusplus
// SPDX-License-Identifier: MIT
//
//----------------------------------------------------------------------------------------------------------------------
//
// -         THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY            -
//
// If you wish to submit a contribution to toml++, hooray and thanks! Before you crack on, please be aware that this
// file was assembled from a number of smaller files by a python script, and code contributions should not be made
// against it directly. You should instead make your changes in the relevant source file(s). The file names of the files
// that contributed to this header can be found at the beginnings and ends of the corresponding sections of this file.
//
//----------------------------------------------------------------------------------------------------------------------
//
// TOML Language Specifications:
// latest:      https://github.com/toml-lang/toml/blob/master/README.md
// v1.0.0:      https://toml.io/en/v1.0.0
// v0.5.0:      https://toml.io/en/v0.5.0
// changelog:   https://github.com/toml-lang/toml/blob/master/CHANGELOG.md
//
//----------------------------------------------------------------------------------------------------------------------
//
// MIT License
//
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------------------------------------------------
#ifndef TOMLPLUSPLUS_H
#define TOMLPLUSPLUS_H

#define INCLUDE_TOMLPLUSPLUS_H // old guard name used pre-v3

//********  impl/preprocessor.h  ***************************************************************************************

// clang-format off

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif

#ifdef __INTELLISENSE__
	#define TOML_INTELLISENSE		1
#else
	#define TOML_INTELLISENSE		0
#endif
#ifdef __clang__
	#define TOML_CLANG				__clang_major__
#else
	#define TOML_CLANG				0
#endif
#ifdef __INTEL_COMPILER
	#define TOML_ICC				__INTEL_COMPILER
	#ifdef __ICL
		#define TOML_ICC_CL			TOML_ICC
	#else
		#define TOML_ICC_CL			0
	#endif
#else
	#define TOML_ICC				0
	#define TOML_ICC_CL				0
#endif
#if defined(_MSC_VER) && !TOML_CLANG && !TOML_ICC
	#define TOML_MSVC				_MSC_VER
#else
	#define TOML_MSVC				0
#endif
#if defined(__GNUC__) && !TOML_CLANG && !TOML_ICC
	#define TOML_GCC				__GNUC__
#else
	#define TOML_GCC				0
#endif

#ifdef __has_include
	#define TOML_HAS_INCLUDE(header)		__has_include(header)
#else
	#define TOML_HAS_INCLUDE(header)		0
#endif

#if TOML_CLANG

	#define TOML_PUSH_WARNINGS \
		_Pragma("clang diagnostic push") \
		static_assert(true)

	#define TOML_DISABLE_SWITCH_WARNINGS \
		_Pragma("clang diagnostic ignored \"-Wswitch\"") \
		static_assert(true)

	#define TOML_DISABLE_ARITHMETIC_WARNINGS \
		_Pragma("clang diagnostic ignored \"-Wfloat-equal\"") \
		_Pragma("clang diagnostic ignored \"-Wdouble-promotion\"") \
		_Pragma("clang diagnostic ignored \"-Wshift-sign-overflow\"") \
		static_assert(true)

	#if TOML_CLANG >= 10
		#define TOML_DISABLE_SPAM_WARNINGS_CLANG_10 \
			_Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"") \
			static_assert(true)
	#else
		#define TOML_DISABLE_SPAM_WARNINGS_CLANG_10 static_assert(true)
	#endif

	#if TOML_CLANG >= 11
		#define TOML_DISABLE_SPAM_WARNINGS_CLANG_11 \
			_Pragma("clang diagnostic ignored \"-Wsuggest-destructor-override\"") \
			static_assert(true)
	#else
		#define TOML_DISABLE_SPAM_WARNINGS_CLANG_11 static_assert(true)
	#endif

	#define TOML_DISABLE_SPAM_WARNINGS \
		TOML_DISABLE_SPAM_WARNINGS_CLANG_10; \
		TOML_DISABLE_SPAM_WARNINGS_CLANG_11; \
		_Pragma("clang diagnostic ignored \"-Wweak-vtables\"")	\
		_Pragma("clang diagnostic ignored \"-Wweak-template-vtables\"") \
		_Pragma("clang diagnostic ignored \"-Wdouble-promotion\"") \
		_Pragma("clang diagnostic ignored \"-Wchar-subscripts\"") \
		_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"") \
		_Pragma("clang diagnostic ignored \"-Wpadded\"") \
		static_assert(true)

	#define TOML_POP_WARNINGS \
		_Pragma("clang diagnostic pop") \
		static_assert(true)

	#define TOML_DISABLE_WARNINGS \
		TOML_PUSH_WARNINGS; \
		_Pragma("clang diagnostic ignored \"-Weverything\"") \
		static_assert(true)

	#define TOML_ENABLE_WARNINGS				TOML_POP_WARNINGS

	#define TOML_ASSUME(expr)					__builtin_assume(expr)
	#define TOML_UNREACHABLE					__builtin_unreachable()
	#define TOML_ATTR(...)						__attribute__((__VA_ARGS__))
	#if defined(_MSC_VER) // msvc compat mode
		#ifdef __has_declspec_attribute
			#if __has_declspec_attribute(novtable)
				#define TOML_ABSTRACT_BASE		__declspec(novtable)
			#endif
			#if __has_declspec_attribute(empty_bases)
				#define TOML_EMPTY_BASES	__declspec(empty_bases)
			#endif
			#ifndef TOML_ALWAYS_INLINE
				#define TOML_ALWAYS_INLINE	__forceinline
			#endif
			#if __has_declspec_attribute(noinline)
				#define TOML_NEVER_INLINE	__declspec(noinline)
			#endif
		#endif
	#endif
	#ifdef __has_attribute
		#if !defined(TOML_ALWAYS_INLINE) && __has_attribute(always_inline)
			#define TOML_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
		#endif
		#if !defined(TOML_NEVER_INLINE) && __has_attribute(noinline)
			#define TOML_NEVER_INLINE		__attribute__((__noinline__))
		#endif
		#if !defined(TOML_TRIVIAL_ABI) && __has_attribute(trivial_abi)
			#define TOML_TRIVIAL_ABI		__attribute__((__trivial_abi__))
		#endif
		#if !defined(TOML_FLAGS_ENUM) && __has_attribute(flag_enum)
			#define TOML_FLAGS_ENUM		__attribute__((__flag_enum__))
		#endif
		#if __has_attribute(enum_extensibility)
			#ifndef TOML_OPEN_ENUM
				#define TOML_OPEN_ENUM		__attribute__((enum_extensibility(open)))
			#endif
			#ifndef TOML_CLOSED_ENUM
				#define TOML_CLOSED_ENUM	__attribute__((enum_extensibility(closed)))
			#endif
		#endif
	#endif
	#define TOML_LIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 1) )
	#define TOML_UNLIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 0) )

	#define TOML_SIMPLE_STATIC_ASSERT_MESSAGES	1

#endif // clang

#if TOML_MSVC || TOML_ICC_CL

	#define TOML_CPP_VERSION					_MSVC_LANG
	#if TOML_MSVC // !intel-cl

		#define TOML_PUSH_WARNINGS \
			__pragma(warning(push)) \
			static_assert(true)

		#if TOML_HAS_INCLUDE(<CodeAnalysis\Warnings.h>)
			#pragma warning(push, 0)
			#include <CodeAnalysis\Warnings.h>
			#pragma warning(pop)
			#define TOML_DISABLE_CODE_ANALYSIS_WARNINGS \
				__pragma(warning(disable: ALL_CODE_ANALYSIS_WARNINGS)) \
				static_assert(true)
		#else
			#define TOML_DISABLE_CODE_ANALYSIS_WARNINGS
				static_assert(true)
		#endif

		#define TOML_DISABLE_SWITCH_WARNINGS \
			__pragma(warning(disable: 4061)) /* enumerator 'identifier' is not explicitly handled by a case label */ \
			__pragma(warning(disable: 4062)) /* enumerator 'identifier' is not handled */ \
			__pragma(warning(disable: 4063)) \
			__pragma(warning(disable: 26819)) \
			static_assert(true)

		#define TOML_DISABLE_SPAM_WARNINGS \
			__pragma(warning(disable: 4127)) /* conditional expr is constant */ \
			__pragma(warning(disable: 4324)) /* structure was padded due to alignment specifier */  \
			__pragma(warning(disable: 4348)) \
			__pragma(warning(disable: 4464)) /* relative include path contains '..' */ \
			__pragma(warning(disable: 4505)) /* unreferenced local function removed */  \
			__pragma(warning(disable: 4514)) /* unreferenced inline function has been removed */ \
			__pragma(warning(disable: 4582)) /* constructor is not implicitly called */ \
			__pragma(warning(disable: 4623)) /* default constructor was implicitly defined as deleted		*/ \
			__pragma(warning(disable: 4625)) /* copy constructor was implicitly defined as deleted			*/ \
			__pragma(warning(disable: 4626)) /* assignment operator was implicitly defined as deleted		*/ \
			__pragma(warning(disable: 4710)) /* function not inlined */ \
			__pragma(warning(disable: 4711)) /* function selected for automatic expansion */ \
			__pragma(warning(disable: 4820)) /* N bytes padding added */  \
			__pragma(warning(disable: 4946)) /* reinterpret_cast used between related classes */ \
			__pragma(warning(disable: 5026)) /* move constructor was implicitly defined as deleted	*/ \
			__pragma(warning(disable: 5027)) /* move assignment operator was implicitly defined as deleted	*/ \
			__pragma(warning(disable: 5039)) /* potentially throwing function passed to 'extern "C"' function */ \
			__pragma(warning(disable: 5045)) /* Compiler will insert Spectre mitigation */ \
			__pragma(warning(disable: 26451)) \
			__pragma(warning(disable: 26490)) \
			__pragma(warning(disable: 26495)) \
			__pragma(warning(disable: 26812)) \
			__pragma(warning(disable: 26819)) \
			static_assert(true)

		#define TOML_DISABLE_ARITHMETIC_WARNINGS \
			__pragma(warning(disable: 4365)) /* argument signed/unsigned mismatch */ \
			__pragma(warning(disable: 4738)) /* storing 32-bit float result in memory */ \
			__pragma(warning(disable: 5219)) /* implicit conversion from integral to float */ \
			static_assert(true)

		#define TOML_POP_WARNINGS \
			__pragma(warning(pop)) \
			static_assert(true)

		#define TOML_DISABLE_WARNINGS \
			__pragma(warning(push, 0))			\
			__pragma(warning(disable: 4348))	\
			__pragma(warning(disable: 4668))	\
			__pragma(warning(disable: 5105))	\
			TOML_DISABLE_CODE_ANALYSIS_WARNINGS;\
			TOML_DISABLE_SWITCH_WARNINGS;		\
			TOML_DISABLE_SPAM_WARNINGS;			\
			TOML_DISABLE_ARITHMETIC_WARNINGS;	\
			static_assert(true)

		#define TOML_ENABLE_WARNINGS			TOML_POP_WARNINGS

	#endif
	#ifndef TOML_ALWAYS_INLINE
		#define TOML_ALWAYS_INLINE				__forceinline
	#endif
	#define TOML_NEVER_INLINE					__declspec(noinline)
	#define TOML_ASSUME(expr)					__assume(expr)
	#define TOML_UNREACHABLE					__assume(0)
	#define TOML_ABSTRACT_BASE					__declspec(novtable)
	#define TOML_EMPTY_BASES					__declspec(empty_bases)
	#ifdef _CPPUNWIND
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
	#endif

#endif // msvc

#if TOML_ICC

	#define TOML_PUSH_WARNINGS \
		__pragma(warning(push)) \
		static_assert(true)

	#define TOML_DISABLE_SPAM_WARNINGS \
		__pragma(warning(disable: 82))	/* storage class is not first */ \
		__pragma(warning(disable: 111))	/* statement unreachable (false-positive) */ \
		__pragma(warning(disable: 869)) /* unreferenced parameter */ \
		__pragma(warning(disable: 1011)) /* missing return (false-positive) */ \
		__pragma(warning(disable: 2261)) /* assume expr side-effects discarded */  \
		static_assert(true)

	#define TOML_POP_WARNINGS \
		__pragma(warning(pop)) \
		static_assert(true)

	#define TOML_DISABLE_WARNINGS \
		__pragma(warning(push, 0)) \
		static_assert(true)

	#define TOML_ENABLE_WARNINGS \
		TOML_POP_WARNINGS

#endif // icc

#if TOML_GCC

	#define TOML_PUSH_WARNINGS \
		_Pragma("GCC diagnostic push") \
		static_assert(true)

	#define TOML_DISABLE_SWITCH_WARNINGS \
		_Pragma("GCC diagnostic ignored \"-Wswitch\"")						\
		_Pragma("GCC diagnostic ignored \"-Wswitch-enum\"")					\
		_Pragma("GCC diagnostic ignored \"-Wswitch-default\"")				\
		static_assert(true)

	#define TOML_DISABLE_ARITHMETIC_WARNINGS \
		_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")					\
		_Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")				\
		static_assert(true)

	#define TOML_DISABLE_SUGGEST_ATTR_WARNINGS \
		_Pragma("GCC diagnostic ignored \"-Wsuggest-attribute=const\"")		\
		_Pragma("GCC diagnostic ignored \"-Wsuggest-attribute=pure\"")		\
		static_assert(true)

	#define TOML_DISABLE_SPAM_WARNINGS \
		_Pragma("GCC diagnostic ignored \"-Wpadded\"")						\
		_Pragma("GCC diagnostic ignored \"-Wcast-align\"")					\
		_Pragma("GCC diagnostic ignored \"-Wcomment\"")						\
		_Pragma("GCC diagnostic ignored \"-Wtype-limits\"")					\
		_Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")				\
		_Pragma("GCC diagnostic ignored \"-Wchar-subscripts\"")				\
		_Pragma("GCC diagnostic ignored \"-Wsubobject-linkage\"")			\
		_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")	\
		_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")			\
		static_assert(true)

	#define TOML_POP_WARNINGS \
		_Pragma("GCC diagnostic pop") \
		static_assert(true)

	#define TOML_DISABLE_WARNINGS \
		TOML_PUSH_WARNINGS;													\
		_Pragma("GCC diagnostic ignored \"-Wall\"")							\
		_Pragma("GCC diagnostic ignored \"-Wextra\"")						\
		_Pragma("GCC diagnostic ignored \"-Wpedantic\"")					\
		TOML_DISABLE_SWITCH_WARNINGS;										\
		TOML_DISABLE_ARITHMETIC_WARNINGS;									\
		TOML_DISABLE_SPAM_WARNINGS;											\
		TOML_DISABLE_SUGGEST_ATTR_WARNINGS;									\
		static_assert(true)

	#define TOML_ENABLE_WARNINGS \
		TOML_POP_WARNINGS

	#define TOML_ATTR(...)						__attribute__((__VA_ARGS__))
	#ifndef TOML_ALWAYS_INLINE
		#define TOML_ALWAYS_INLINE				__attribute__((__always_inline__)) inline
	#endif
	#define TOML_NEVER_INLINE					__attribute__((__noinline__))
	#define TOML_UNREACHABLE					__builtin_unreachable()
	#define TOML_LIKELY(...)					(__builtin_expect(!!(__VA_ARGS__), 1) )
	#define TOML_UNLIKELY(...)					(__builtin_expect(!!(__VA_ARGS__), 0) )

#endif

#ifndef TOML_CPP_VERSION
	#define TOML_CPP_VERSION __cplusplus
#endif
#if TOML_CPP_VERSION < 201103L
	#error toml++ requires C++17 or higher. For a TOML library supporting pre-C++11 see https://github.com/ToruNiina/Boost.toml
#elif TOML_CPP_VERSION < 201703L
	#error toml++ requires C++17 or higher. For a TOML library supporting C++11 see https://github.com/ToruNiina/toml11
#endif

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
#endif

// is the library being built as a shared lib/dll using meson and friends?
#ifndef TOML_SHARED_LIB
	#define TOML_SHARED_LIB 0
#endif

// header-only mode
#if !defined(TOML_HEADER_ONLY) && defined(TOML_ALL_INLINE) // was TOML_ALL_INLINE pre-2.0
	#define TOML_HEADER_ONLY TOML_ALL_INLINE
#endif
#if !defined(TOML_HEADER_ONLY) || (defined(TOML_HEADER_ONLY) && TOML_HEADER_ONLY) || TOML_INTELLISENSE
	#undef TOML_HEADER_ONLY
	#define TOML_HEADER_ONLY 1
#endif
#if defined(DOXYGEN) || TOML_SHARED_LIB
	#undef TOML_HEADER_ONLY
	#define TOML_HEADER_ONLY 0
#endif

// internal implementation switch
#if defined(TOML_IMPLEMENTATION) || TOML_HEADER_ONLY
	#undef TOML_IMPLEMENTATION
	#define TOML_IMPLEMENTATION 1
#else
	#define TOML_IMPLEMENTATION 0
#endif

// dll/shared lib function exports (legacy - TOML_API was the old name for this setting)
#if !defined(TOML_EXPORTED_MEMBER_FUNCTION)		   \
		&& !defined(TOML_EXPORTED_STATIC_FUNCTION) \
		&& !defined(TOML_EXPORTED_FREE_FUNCTION)   \
		&& defined(TOML_API)
	#define TOML_EXPORTED_MEMBER_FUNCTION	TOML_API
	#define TOML_EXPORTED_STATIC_FUNCTION	TOML_API
	#define TOML_EXPORTED_FREE_FUNCTION		TOML_API
#endif

// dll/shared lib exports
#if TOML_SHARED_LIB
	#undef TOML_API
	#undef TOML_EXPORTED_CLASS
	#undef TOML_EXPORTED_MEMBER_FUNCTION
	#undef TOML_EXPORTED_STATIC_FUNCTION
	#undef TOML_EXPORTED_FREE_FUNCTION
	#if defined(_WIN32) || defined(__CYGWIN__)
		#if TOML_IMPLEMENTATION
			#define TOML_EXPORTED_CLASS			__declspec(dllexport)
			#define TOML_EXPORTED_FREE_FUNCTION	__declspec(dllexport)
		#else
			#define TOML_EXPORTED_CLASS			__declspec(dllimport)
			#define TOML_EXPORTED_FREE_FUNCTION	__declspec(dllimport)
		#endif
	#elif defined(__GNUC__) && __GNUC__ >= 4
		#define TOML_EXPORTED_CLASS				__attribute__((visibility("default")))
		#define TOML_EXPORTED_MEMBER_FUNCTION	__attribute__((visibility("default")))
		#define TOML_EXPORTED_STATIC_FUNCTION	__attribute__((visibility("default")))
		#define TOML_EXPORTED_FREE_FUNCTION		__attribute__((visibility("default")))
	#endif
#endif
#ifndef TOML_EXPORTED_CLASS
	#define TOML_EXPORTED_CLASS
#endif
#ifndef TOML_EXPORTED_MEMBER_FUNCTION
	#define TOML_EXPORTED_MEMBER_FUNCTION
#endif
#ifndef TOML_EXPORTED_STATIC_FUNCTION
	#define TOML_EXPORTED_STATIC_FUNCTION
#endif
#ifndef TOML_EXPORTED_FREE_FUNCTION
	#define TOML_EXPORTED_FREE_FUNCTION
#endif

// experimental language features
#if !defined(TOML_ENABLE_UNRELEASED_FEATURES) && defined(TOML_UNRELEASED_FEATURES) // was TOML_UNRELEASED_FEATURES pre-3.0
	#define TOML_ENABLE_UNRELEASED_FEATURES TOML_UNRELEASED_FEATURES
#endif
#if (defined(TOML_ENABLE_UNRELEASED_FEATURES) && TOML_ENABLE_UNRELEASED_FEATURES) || TOML_INTELLISENSE
	#undef TOML_ENABLE_UNRELEASED_FEATURES
	#define TOML_ENABLE_UNRELEASED_FEATURES 1
#endif
#ifndef TOML_ENABLE_UNRELEASED_FEATURES
	#define TOML_ENABLE_UNRELEASED_FEATURES 0
#endif

// parser
#if !defined(TOML_ENABLE_PARSER) && defined(TOML_PARSER) // was TOML_PARSER pre-3.0
	#define TOML_ENABLE_PARSER TOML_PARSER
#endif
#if !defined(TOML_ENABLE_PARSER) || (defined(TOML_ENABLE_PARSER) && TOML_ENABLE_PARSER) || TOML_INTELLISENSE
	#undef TOML_ENABLE_PARSER
	#define TOML_ENABLE_PARSER 1
#endif

// formatters
#if !defined(TOML_ENABLE_FORMATTERS)									\
		|| (defined(TOML_ENABLE_FORMATTERS) && TOML_ENABLE_FORMATTERS)	\
		|| TOML_INTELLISENSE
	#undef TOML_ENABLE_FORMATTERS
	#define TOML_ENABLE_FORMATTERS 1
#endif

// SIMD
#if !defined(TOML_ENABLE_SIMD)									\
		|| (defined(TOML_ENABLE_SIMD) && TOML_ENABLE_SIMD)	\
		|| TOML_INTELLISENSE
	#undef TOML_ENABLE_SIMD
	#define TOML_ENABLE_SIMD 1
#endif

// windows compat
#if !defined(TOML_ENABLE_WINDOWS_COMPAT) && defined(TOML_WINDOWS_COMPAT) // was TOML_WINDOWS_COMPAT pre-3.0
	#define TOML_ENABLE_WINDOWS_COMPAT TOML_WINDOWS_COMPAT
#endif
#if !defined(TOML_ENABLE_WINDOWS_COMPAT)										\
		|| (defined(TOML_ENABLE_WINDOWS_COMPAT) && TOML_ENABLE_WINDOWS_COMPAT)	\
		|| TOML_INTELLISENSE
	#undef TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_ENABLE_WINDOWS_COMPAT 1
#endif

#ifndef _WIN32
	#undef TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_ENABLE_WINDOWS_COMPAT 0
#endif

#ifndef TOML_INCLUDE_WINDOWS_H
	#define TOML_INCLUDE_WINDOWS_H 0
#endif

// custom optional
#ifdef TOML_OPTIONAL_TYPE
	#define TOML_HAS_CUSTOM_OPTIONAL_TYPE 1
#else
	#define TOML_HAS_CUSTOM_OPTIONAL_TYPE 0
#endif

// exceptions (compiler support)
#ifndef TOML_COMPILER_EXCEPTIONS
	#if defined(__EXCEPTIONS) || defined(__cpp_exceptions)
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
	#endif
#endif

// exceptions (library use)
#if TOML_COMPILER_EXCEPTIONS
	#if !defined(TOML_EXCEPTIONS) || (defined(TOML_EXCEPTIONS) && TOML_EXCEPTIONS)
		#undef TOML_EXCEPTIONS
		#define TOML_EXCEPTIONS 1
	#endif
#else
	#if defined(TOML_EXCEPTIONS) && TOML_EXCEPTIONS
		#error TOML_EXCEPTIONS was explicitly enabled but exceptions are disabled/unsupported by the compiler.
	#endif
	#undef TOML_EXCEPTIONS
	#define TOML_EXCEPTIONS	0
#endif

#ifndef TOML_UNDEF_MACROS
	#define TOML_UNDEF_MACROS 1
#endif

#ifndef TOML_MAX_NESTED_VALUES
	#define TOML_MAX_NESTED_VALUES 256
	// this refers to the depth of nested values, e.g. inline tables and arrays.
	// 256 is crazy high! if you're hitting this limit with real input, TOML is probably the wrong tool for the job...
#endif

#ifdef TOML_CHAR_8_STRINGS
	#if TOML_CHAR_8_STRINGS
		#error TOML_CHAR_8_STRINGS was removed in toml++ 2.0.0; all value setters and getters now work with char8_t strings implicitly.
	#endif
#endif

#ifdef TOML_LARGE_FILES
	#if !TOML_LARGE_FILES
		#error Support for !TOML_LARGE_FILES (i.e. 'small files') was removed in toml++ 3.0.0.
	#endif
#endif

#if !defined(TOML_FLOAT_CHARCONV) && (TOML_GCC || TOML_CLANG || (TOML_ICC && !TOML_ICC_CL))
	// not supported by any version of GCC or Clang as of 26/11/2020
	// not supported by any version of ICC on Linux as of 11/01/2021
	#define TOML_FLOAT_CHARCONV 0
#endif
#if !defined(TOML_INT_CHARCONV) && (defined(__EMSCRIPTEN__) || defined(__APPLE__))
	// causes link errors on emscripten
	// causes Mac OS SDK version errors on some versions of Apple Clang
	#define TOML_INT_CHARCONV 0
#endif
#ifndef TOML_INT_CHARCONV
	#define TOML_INT_CHARCONV 1
#endif
#ifndef TOML_FLOAT_CHARCONV
	#define TOML_FLOAT_CHARCONV 1
#endif
#if (TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV) && !TOML_HAS_INCLUDE(<charconv>)
	#undef TOML_INT_CHARCONV
	#undef TOML_FLOAT_CHARCONV
	#define TOML_INT_CHARCONV 0
	#define TOML_FLOAT_CHARCONV 0
#endif

#ifndef TOML_PUSH_WARNINGS
	#define TOML_PUSH_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_CODE_ANALYSIS_WARNINGS
	#define	TOML_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_SWITCH_WARNINGS
	#define	TOML_DISABLE_SWITCH_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_SUGGEST_ATTR_WARNINGS
	#define TOML_DISABLE_SUGGEST_ATTR_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_SPAM_WARNINGS
	#define TOML_DISABLE_SPAM_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_ARITHMETIC_WARNINGS
	#define TOML_DISABLE_ARITHMETIC_WARNINGS static_assert(true)
#endif
#ifndef TOML_POP_WARNINGS
	#define TOML_POP_WARNINGS static_assert(true)
#endif
#ifndef TOML_DISABLE_WARNINGS
	#define TOML_DISABLE_WARNINGS static_assert(true)
#endif
#ifndef TOML_ENABLE_WARNINGS
	#define TOML_ENABLE_WARNINGS static_assert(true)
#endif

#ifndef TOML_ATTR
	#define TOML_ATTR(...)
#endif

#ifndef TOML_ABSTRACT_BASE
	#define TOML_ABSTRACT_BASE
#endif

#ifndef TOML_EMPTY_BASES
	#define TOML_EMPTY_BASES
#endif

#ifndef TOML_ALWAYS_INLINE
	#define TOML_ALWAYS_INLINE	inline
#endif
#ifndef TOML_NEVER_INLINE
	#define TOML_NEVER_INLINE
#endif

#ifndef TOML_ASSUME
	#define TOML_ASSUME(expr)	static_assert(true)
#endif

#ifndef TOML_UNREACHABLE
	#define TOML_UNREACHABLE	TOML_ASSUME(false)
#endif

#ifndef TOML_FLAGS_ENUM
	#define TOML_FLAGS_ENUM
#endif

#ifndef TOML_OPEN_ENUM
	#define TOML_OPEN_ENUM
#endif

#ifndef TOML_CLOSED_ENUM
	#define TOML_CLOSED_ENUM
#endif

#ifndef TOML_OPEN_FLAGS_ENUM
	#define TOML_OPEN_FLAGS_ENUM TOML_OPEN_ENUM TOML_FLAGS_ENUM
#endif

#ifndef TOML_CLOSED_FLAGS_ENUM
	#define TOML_CLOSED_FLAGS_ENUM TOML_CLOSED_ENUM TOML_FLAGS_ENUM
#endif

#ifdef __has_cpp_attribute
	#define TOML_HAS_ATTR(...)	__has_cpp_attribute(__VA_ARGS__)
#else
	#define TOML_HAS_ATTR(...)	0
#endif

#if TOML_HAS_ATTR(likely) >= 201803
	#ifndef TOML_LIKELY
		#define TOML_LIKELY(...)	(__VA_ARGS__) [[likely]]
	#endif
	#ifndef TOML_LIKELY_CASE
		#define TOML_LIKELY_CASE	[[likely]]
	#endif
#endif
#ifndef TOML_LIKELY
	#define TOML_LIKELY(...)	(__VA_ARGS__)
#endif
#ifndef TOML_LIKELY_CASE
	#define TOML_LIKELY_CASE
#endif

#if TOML_HAS_ATTR(unlikely) >= 201803
	#ifndef TOML_UNLIKELY
		#define TOML_UNLIKELY(...)	(__VA_ARGS__) [[unlikely]]
	#endif
	#ifndef TOML_UNLIKELY_CASE
		#define TOML_UNLIKELY_CASE	[[unlikely]]
	#endif
#endif
#ifndef TOML_UNLIKELY
	#define TOML_UNLIKELY(...)	(__VA_ARGS__)
#endif
#ifndef TOML_UNLIKELY_CASE
	#define TOML_UNLIKELY_CASE
#endif

#if TOML_HAS_ATTR(nodiscard)
	#define TOML_NODISCARD [[nodiscard]]
#else
	#define TOML_NODISCARD
#endif

#if TOML_HAS_ATTR(nodiscard) >= 201907
	#define TOML_NODISCARD_CTOR [[nodiscard]]
#else
	#define TOML_NODISCARD_CTOR
#endif

#ifndef TOML_TRIVIAL_ABI
	#define TOML_TRIVIAL_ABI
#endif

#define TOML_ASYMMETRICAL_EQUALITY_OPS(LHS, RHS, ...)															\
	__VA_ARGS__ TOML_NODISCARD friend bool operator == (RHS rhs, LHS lhs) noexcept { return lhs == rhs; }		\
	__VA_ARGS__ TOML_NODISCARD friend bool operator != (LHS lhs, RHS rhs) noexcept { return !(lhs == rhs); }	\
	__VA_ARGS__ TOML_NODISCARD friend bool operator != (RHS rhs, LHS lhs) noexcept { return !(lhs == rhs); }	\
	static_assert(true)

#ifndef TOML_SIMPLE_STATIC_ASSERT_MESSAGES
	#define TOML_SIMPLE_STATIC_ASSERT_MESSAGES	0
#endif

#define TOML_CONCAT_1(x, y) x##y
#define TOML_CONCAT(x, y) TOML_CONCAT_1(x, y)

#define TOML_EVAL_BOOL_1(T, F)	T
#define TOML_EVAL_BOOL_0(T, F)	F

#if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) || defined(_M_ARM64) || defined(__ARM_64BIT_STATE)	\
		|| defined(__arm__) || defined(_M_ARM) || defined(__ARM_32BIT_STATE)
	#define TOML_ARM 1
#else
	#define TOML_ARM 0
#endif

#define TOML_MAKE_FLAGS_(name, op)																	\
	TOML_CONST_INLINE_GETTER																		\
	constexpr name operator op(name lhs, name rhs) noexcept											\
	{																								\
		using under = std::underlying_type_t<name>;													\
		return static_cast<name>(static_cast<under>(lhs) op static_cast<under>(rhs));				\
	}																								\
	constexpr name& operator TOML_CONCAT(op, =)(name & lhs, name rhs) noexcept						\
	{																								\
		return lhs = (lhs op rhs);																	\
	}																								\
	static_assert(true, "")

#define TOML_MAKE_FLAGS(name)																		\
	TOML_MAKE_FLAGS_(name, &);																		\
	TOML_MAKE_FLAGS_(name, |);																		\
	TOML_MAKE_FLAGS_(name, ^);																		\
	TOML_CONST_INLINE_GETTER																		\
	constexpr name operator~(name val) noexcept														\
	{																								\
		using under = std::underlying_type_t<name>;													\
		return static_cast<name>(~static_cast<under>(val));											\
	}																								\
	TOML_CONST_INLINE_GETTER																		\
	constexpr bool operator!(name val) noexcept														\
	{																								\
		using under = std::underlying_type_t<name>;													\
		return !static_cast<under>(val);															\
	}																								\
	static_assert(true, "")

#ifndef TOML_LIFETIME_HOOKS
	#define TOML_LIFETIME_HOOKS 0
#endif

#if !defined(__POXY__) && !defined(POXY_IMPLEMENTATION_DETAIL)
	#define POXY_IMPLEMENTATION_DETAIL(...) __VA_ARGS__
#endif

#ifdef NDEBUG
	#define TOML_PURE_GETTER			TOML_NODISCARD						TOML_ATTR(pure)
	#define TOML_CONST_GETTER			TOML_NODISCARD						TOML_ATTR(const)
	#define TOML_PURE_INLINE_GETTER		TOML_NODISCARD	TOML_ALWAYS_INLINE	TOML_ATTR(pure)
	#define TOML_CONST_INLINE_GETTER	TOML_NODISCARD	TOML_ALWAYS_INLINE	TOML_ATTR(const)
#else
	#define TOML_PURE_GETTER			TOML_NODISCARD
	#define TOML_CONST_GETTER			TOML_NODISCARD
	#define TOML_PURE_INLINE_GETTER		TOML_NODISCARD	TOML_ALWAYS_INLINE
	#define TOML_CONST_INLINE_GETTER	TOML_NODISCARD	TOML_ALWAYS_INLINE
#endif

#define TOML_UNUSED(...) static_cast<void>(__VA_ARGS__)

#define TOML_DELETE_DEFAULTS(T)                                                                                        \
	T(const T&) = delete;                                                                                              \
	T(T&&)		= delete;                                                                                              \
	T& operator=(const T&) = delete;                                                                                   \
	T& operator=(T&&) = delete

// SFINAE
#if defined(__cpp_concepts) && __cpp_concepts >= 201907
	#define TOML_REQUIRES(...)	requires(__VA_ARGS__)
#else
	#define TOML_REQUIRES(...)
#endif
#define TOML_ENABLE_IF(...)	, typename std::enable_if<(__VA_ARGS__), int>::type = 0
#define TOML_CONSTRAINED_TEMPLATE(condition, ...)	template <__VA_ARGS__ TOML_ENABLE_IF(condition)> TOML_REQUIRES(condition)
#define TOML_HIDDEN_CONSTRAINT(condition, ...)	TOML_CONSTRAINED_TEMPLATE(condition, __VA_ARGS__)

#ifdef __FLT16_MANT_DIG__
	#if __FLT_RADIX__ == 2					\
			&& __FLT16_MANT_DIG__ == 11		\
			&& __FLT16_DIG__ == 3			\
			&& __FLT16_MIN_EXP__ == -13		\
			&& __FLT16_MIN_10_EXP__ == -4	\
			&& __FLT16_MAX_EXP__ == 16		\
			&& __FLT16_MAX_10_EXP__ == 4
		#if TOML_ARM && (TOML_GCC || TOML_CLANG)
			#define TOML_FP16 __fp16
		#endif
		#if TOML_ARM && TOML_CLANG // not present in g++
			#define TOML_FLOAT16 _Float16
		#endif
	#endif
#endif

#if defined(__SIZEOF_FLOAT128__)		\
	&& defined(__FLT128_MANT_DIG__)		\
	&& defined(__LDBL_MANT_DIG__)		\
	&& __FLT128_MANT_DIG__ > __LDBL_MANT_DIG__
	#define TOML_FLOAT128	__float128
#endif

#ifdef __SIZEOF_INT128__
	#define TOML_INT128		__int128_t
	#define TOML_UINT128	__uint128_t
#endif

#define TOML_LIB_MAJOR 3
#define TOML_LIB_MINOR 0
#define TOML_LIB_PATCH 1

#define TOML_LANG_MAJOR 1
#define TOML_LANG_MINOR 0
#define TOML_LANG_PATCH 0

#define	TOML_LIB_SINGLE_HEADER 1

#define TOML_MAKE_VERSION(major, minor, patch)											\
		((major) * 10000 + (minor) * 100 + (patch))

#if TOML_ENABLE_UNRELEASED_FEATURES
	#define TOML_LANG_EFFECTIVE_VERSION													\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH+1)
#else
	#define TOML_LANG_EFFECTIVE_VERSION													\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)
#endif

#define TOML_LANG_HIGHER_THAN(major, minor, patch)										\
		(TOML_LANG_EFFECTIVE_VERSION > TOML_MAKE_VERSION(major, minor, patch))

#define TOML_LANG_AT_LEAST(major, minor, patch)											\
		(TOML_LANG_EFFECTIVE_VERSION >= TOML_MAKE_VERSION(major, minor, patch))

#define TOML_LANG_UNRELEASED															\
		TOML_LANG_HIGHER_THAN(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)

#ifndef TOML_ABI_NAMESPACES
	#ifdef DOXYGEN
		#define TOML_ABI_NAMESPACES 0
	#else
		#define TOML_ABI_NAMESPACES 1
	#endif
#endif
#if TOML_ABI_NAMESPACES
	#define TOML_NAMESPACE_START				namespace toml { inline namespace TOML_CONCAT(v, TOML_LIB_MAJOR)
	#define TOML_NAMESPACE_END					} static_assert(true)
	#define TOML_NAMESPACE						::toml::TOML_CONCAT(v, TOML_LIB_MAJOR)
	#define TOML_ABI_NAMESPACE_START(name)		inline namespace name { static_assert(true)
	#define TOML_ABI_NAMESPACE_BOOL(cond, T, F)	TOML_ABI_NAMESPACE_START(TOML_CONCAT(TOML_EVAL_BOOL_, cond)(T, F))
	#define TOML_ABI_NAMESPACE_END				} static_assert(true)
#else
	#define TOML_NAMESPACE_START				namespace toml
	#define TOML_NAMESPACE_END					static_assert(true)
	#define TOML_NAMESPACE						toml
	#define TOML_ABI_NAMESPACE_START(...)		static_assert(true)
	#define TOML_ABI_NAMESPACE_BOOL(...)		static_assert(true)
	#define TOML_ABI_NAMESPACE_END				static_assert(true)
#endif
#define TOML_IMPL_NAMESPACE_START				TOML_NAMESPACE_START { namespace impl
#define TOML_IMPL_NAMESPACE_END					} TOML_NAMESPACE_END
#if TOML_HEADER_ONLY
	#define TOML_ANON_NAMESPACE_START			static_assert(TOML_IMPLEMENTATION); TOML_IMPL_NAMESPACE_START
	#define TOML_ANON_NAMESPACE_END				TOML_IMPL_NAMESPACE_END
	#define TOML_ANON_NAMESPACE					TOML_NAMESPACE::impl
	#define TOML_EXTERNAL_LINKAGE				inline
	#define TOML_INTERNAL_LINKAGE				inline
#else
	#define TOML_ANON_NAMESPACE_START			static_assert(TOML_IMPLEMENTATION);	\
												using namespace toml;				\
												namespace
	#define TOML_ANON_NAMESPACE_END				static_assert(true)
	#define TOML_ANON_NAMESPACE
	#define TOML_EXTERNAL_LINKAGE
	#define TOML_INTERNAL_LINKAGE				static
#endif

#ifdef NDEBUG
	#undef TOML_ASSERT
	#define TOML_ASSERT(expr)	static_assert(true)
#endif
#ifndef TOML_ASSERT
	#ifndef assert
		TOML_DISABLE_WARNINGS;
		#include <cassert>
		TOML_ENABLE_WARNINGS;
	#endif
	#define TOML_ASSERT(expr)	assert(expr)
#endif
#ifdef NDEBUG
	#define TOML_ASSERT_ASSUME(expr)	TOML_ASSUME(expr)
#else
	#define TOML_ASSERT_ASSUME(expr)	TOML_ASSERT(expr)
#endif

#if TOML_SIMPLE_STATIC_ASSERT_MESSAGES

	#define TOML_SA_NEWLINE		" "
	#define TOML_SA_LIST_SEP	", "
	#define TOML_SA_LIST_BEG	" ("
	#define TOML_SA_LIST_END	")"
	#define TOML_SA_LIST_NEW	" "
	#define TOML_SA_LIST_NXT	", "

#else

	#define TOML_SA_NEWLINE			"\n| "
	#define TOML_SA_LIST_SEP		TOML_SA_NEWLINE "  - "
	#define TOML_SA_LIST_BEG		TOML_SA_LIST_SEP
	#define TOML_SA_LIST_END
	#define TOML_SA_LIST_NEW		TOML_SA_NEWLINE TOML_SA_NEWLINE
	#define TOML_SA_LIST_NXT		TOML_SA_LIST_NEW

#endif

#define TOML_SA_NATIVE_VALUE_TYPE_LIST							\
	TOML_SA_LIST_BEG	"std::string"							\
	TOML_SA_LIST_SEP	"int64_t"								\
	TOML_SA_LIST_SEP	"double"								\
	TOML_SA_LIST_SEP	"bool"									\
	TOML_SA_LIST_SEP	"toml::date"							\
	TOML_SA_LIST_SEP	"toml::time"							\
	TOML_SA_LIST_SEP	"toml::date_time"						\
	TOML_SA_LIST_END

#define TOML_SA_NODE_TYPE_LIST									\
	TOML_SA_LIST_BEG	"toml::table"							\
	TOML_SA_LIST_SEP	"toml::array"							\
	TOML_SA_LIST_SEP	"toml::value<std::string>"				\
	TOML_SA_LIST_SEP	"toml::value<int64_t>"					\
	TOML_SA_LIST_SEP	"toml::value<double>"					\
	TOML_SA_LIST_SEP	"toml::value<bool>"						\
	TOML_SA_LIST_SEP	"toml::value<toml::date>"				\
	TOML_SA_LIST_SEP	"toml::value<toml::time>"				\
	TOML_SA_LIST_SEP	"toml::value<toml::date_time>"			\
	TOML_SA_LIST_END

#define TOML_SA_UNWRAPPED_NODE_TYPE_LIST						\
	TOML_SA_LIST_NEW	"A native TOML value type"				\
	TOML_SA_NATIVE_VALUE_TYPE_LIST								\
																\
	TOML_SA_LIST_NXT	"A TOML node type"						\
	TOML_SA_NODE_TYPE_LIST

// clang-format on

TOML_PUSH_WARNINGS;
TOML_DISABLE_SPAM_WARNINGS;
TOML_DISABLE_SWITCH_WARNINGS;
TOML_DISABLE_SUGGEST_ATTR_WARNINGS;

// misc warning false-positives
#if TOML_MSVC
#pragma warning(disable : 5031) // #pragma warning(pop): likely mismatch
#if TOML_SHARED_LIB
#pragma warning(disable : 4251) // dll exports for std lib types
#endif
#elif TOML_CLANG
#pragma clang diagnostic ignored "-Wheader-hygiene"
#if TOML_CLANG >= 12
#pragma clang diagnostic ignored "-Wc++20-extensions"
#endif
#if (TOML_CLANG == 13) && !defined(__APPLE__)
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#endif

//********  impl/std_new.h  ********************************************************************************************

TOML_DISABLE_WARNINGS;
#include <new>
TOML_ENABLE_WARNINGS;

#if TOML_CLANG >= 8 || TOML_GCC >= 7 || TOML_ICC >= 1910 || TOML_MSVC >= 1914
#define TOML_LAUNDER(x) __builtin_launder(x)
#elif defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
#define TOML_LAUNDER(x) std::launder(x)
#else
#define TOML_LAUNDER(x) x
#endif

//********  impl/std_string.h  *****************************************************************************************

TOML_DISABLE_WARNINGS;
#include <string_view>
#include <string>
TOML_ENABLE_WARNINGS;

#if defined(DOXYGEN)                                                                                                   \
	|| (defined(__cpp_char8_t) && __cpp_char8_t >= 201811 && defined(__cpp_lib_char8_t)                                \
		&& __cpp_lib_char8_t >= 201907)
#define TOML_HAS_CHAR8 1
#else
#define TOML_HAS_CHAR8 0
#endif

namespace toml // non-abi namespace; this is not an error
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
}

#if TOML_ENABLE_WINDOWS_COMPAT

TOML_IMPL_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::string narrow(std::wstring_view);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::wstring widen(std::string_view);

#if TOML_HAS_CHAR8

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::wstring widen(std::u8string_view);

#endif
}
TOML_IMPL_NAMESPACE_END;

#endif // TOML_ENABLE_WINDOWS_COMPAT

//********  impl/std_optional.h  ***************************************************************************************

TOML_DISABLE_WARNINGS;
#if !TOML_HAS_CUSTOM_OPTIONAL_TYPE
#include <optional>
#endif
TOML_ENABLE_WARNINGS;

TOML_NAMESPACE_START
{
#if TOML_HAS_CUSTOM_OPTIONAL_TYPE

	template <typename T>
	using optional = TOML_OPTIONAL_TYPE<T>;

#else

	template <typename T>
	using optional = std::optional<T>;

#endif
}
TOML_NAMESPACE_END;

//********  impl/forward_declarations.h  *******************************************************************************

TOML_DISABLE_WARNINGS;
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cfloat>
#include <climits>
#include <cmath>
#include <limits>
#include <memory>
#include <iosfwd>
#include <type_traits>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#ifndef TOML_DISABLE_ENVIRONMENT_CHECKS
#define TOML_ENV_MESSAGE                                                                                               \
	"If you're seeing this error it's because you're building toml++ for an environment that doesn't conform to "      \
	"one of the 'ground truths' assumed by the library. Essentially this just means that I don't have the "            \
	"resources to test on more platforms, but I wish I did! You can try disabling the checks by defining "             \
	"TOML_DISABLE_ENVIRONMENT_CHECKS, but your mileage may vary. Please consider filing an issue at "                  \
	"https://github.com/marzer/tomlplusplus/issues to help me improve support for your target environment. "           \
	"Thanks!"

static_assert(CHAR_BIT == 8, TOML_ENV_MESSAGE);
static_assert(FLT_RADIX == 2, TOML_ENV_MESSAGE);
static_assert('A' == 65, TOML_ENV_MESSAGE);
static_assert(sizeof(double) == 8, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::is_iec559, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits == 53, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits10 == 15, TOML_ENV_MESSAGE);

#undef TOML_ENV_MESSAGE
#endif // !TOML_DISABLE_ENVIRONMENT_CHECKS

// undocumented forward declarations are hidden from doxygen because they fuck it up =/

namespace toml // non-abi namespace; this is not an error
{
	using ::std::size_t;
	using ::std::intptr_t;
	using ::std::uintptr_t;
	using ::std::ptrdiff_t;
	using ::std::nullptr_t;
	using ::std::int8_t;
	using ::std::int16_t;
	using ::std::int32_t;
	using ::std::int64_t;
	using ::std::uint8_t;
	using ::std::uint16_t;
	using ::std::uint32_t;
	using ::std::uint64_t;
	using ::std::uint_least32_t;
	using ::std::uint_least64_t;
}

TOML_NAMESPACE_START
{
	struct date;
	struct time;
	struct time_offset;

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);
	struct date_time;
	TOML_ABI_NAMESPACE_END;

	struct source_position;
	struct source_region;

	class node;
	template <typename>
	class node_view;

	class key;
	class array;
	class table;
	template <typename>
	class value;

	class toml_formatter;
	class json_formatter;
	class yaml_formatter;

	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);
#if TOML_EXCEPTIONS
	using parse_result = table;
#else
	class parse_result;
#endif
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	using node_ptr = std::unique_ptr<node>;

	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);
	class parser;
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	// clang-format off

	inline constexpr std::string_view control_char_escapes[] =
	{
		"\\u0000"sv,
		"\\u0001"sv,
		"\\u0002"sv,
		"\\u0003"sv,
		"\\u0004"sv,
		"\\u0005"sv,
		"\\u0006"sv,
		"\\u0007"sv,
		"\\b"sv,
		"\\t"sv,
		"\\n"sv,
		"\\u000B"sv,
		"\\f"sv,
		"\\r"sv,
		"\\u000E"sv,
		"\\u000F"sv,
		"\\u0010"sv,
		"\\u0011"sv,
		"\\u0012"sv,
		"\\u0013"sv,
		"\\u0014"sv,
		"\\u0015"sv,
		"\\u0016"sv,
		"\\u0017"sv,
		"\\u0018"sv,
		"\\u0019"sv,
		"\\u001A"sv,
		"\\u001B"sv,
		"\\u001C"sv,
		"\\u001D"sv,
		"\\u001E"sv,
		"\\u001F"sv,
	};

	inline constexpr std::string_view node_type_friendly_names[] =
	{
		"none"sv,
		"table"sv,
		"array"sv,
		"string"sv,
		"integer"sv,
		"floating-point"sv,
		"boolean"sv,
		"date"sv,
		"time"sv,
		"date-time"sv
	};

	// clang-format on
}
TOML_IMPL_NAMESPACE_END;

#if TOML_ABI_NAMESPACES
#if TOML_EXCEPTIONS
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::impl_ex::parser
#else
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::impl_noex::parser
#endif
#else
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::parser
#endif

namespace toml
{
}

TOML_NAMESPACE_START // abi namespace
{
	inline namespace literals
	{
	}

	enum class TOML_CLOSED_ENUM node_type : uint8_t
	{
		none,
		table,
		array,
		string,
		integer,
		floating_point,
		boolean,
		date,
		time,
		date_time
	};

	template <typename Char>
	inline std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& lhs, node_type rhs)
	{
		using underlying_t = std::underlying_type_t<node_type>;
		const auto str	   = impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
		if constexpr (std::is_same_v<Char, char>)
			return lhs << str;
		else
		{
			if constexpr (sizeof(Char) == 1)
				return lhs << std::basic_string_view<Char>{ reinterpret_cast<const Char*>(str.data()), str.length() };
			else
				return lhs << str.data();
		}
	}

	enum class TOML_OPEN_FLAGS_ENUM value_flags : uint16_t // being an "OPEN" flags enum is not an error
	{
		none,
		format_as_binary = 1,
		format_as_octal = 2,
		format_as_hexadecimal = 3,
	};
	TOML_MAKE_FLAGS(value_flags);

	inline constexpr value_flags preserve_source_value_flags =
		POXY_IMPLEMENTATION_DETAIL(value_flags{ static_cast<std::underlying_type_t<value_flags>>(-1) });

	enum class TOML_CLOSED_FLAGS_ENUM format_flags : uint64_t
	{
		none,
		quote_dates_and_times = (1ull << 0),
		quote_infinities_and_nans = (1ull << 1),
		allow_literal_strings = (1ull << 2),
		allow_multi_line_strings = (1ull << 3),
		allow_real_tabs_in_strings = (1ull << 4),
		allow_unicode_strings = (1ull << 5),
		allow_binary_integers = (1ull << 6),
		allow_octal_integers = (1ull << 7),
		allow_hexadecimal_integers = (1ull << 8),
		indent_sub_tables = (1ull << 9),
		indent_array_elements = (1ull << 10),
		indentation = indent_sub_tables | indent_array_elements,
		relaxed_float_precision = (1ull << 11),
	};
	TOML_MAKE_FLAGS(format_flags);

	template <typename T>
	struct TOML_TRIVIAL_ABI inserter
	{
		static_assert(std::is_reference_v<T>);

		T value;
	};
	template <typename T>
	inserter(T &&) -> inserter<T&&>;
	template <typename T>
	inserter(T&) -> inserter<T&>;

	using default_formatter = toml_formatter;
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename... T>
	using common_signed_type = std::common_type_t<std::make_signed_t<T>...>;

	template <typename T, typename... U>
	inline constexpr bool is_one_of = (false || ... || std::is_same_v<T, U>);

	template <typename... T>
	inline constexpr bool all_integral = (std::is_integral_v<T> && ...);

	template <typename T>
	inline constexpr bool is_cvref = std::is_reference_v<T> || std::is_const_v<T> || std::is_volatile_v<T>;

	template <typename T>
	inline constexpr bool is_wide_string =
		is_one_of<std::decay_t<T>, const wchar_t*, wchar_t*, std::wstring_view, std::wstring>;

	template <typename T>
	inline constexpr bool value_retrieval_is_nothrow = !std::is_same_v<remove_cvref<T>, std::string>
#if TOML_HAS_CHAR8
													&& !std::is_same_v<remove_cvref<T>, std::u8string>
#endif

													&& !is_wide_string<T>;

	template <typename, typename>
	struct copy_ref_;
	template <typename Dest, typename Src>
	using copy_ref = typename copy_ref_<Dest, Src>::type;

	template <typename Dest, typename Src>
	struct copy_ref_
	{
		using type = Dest;
	};

	template <typename Dest, typename Src>
	struct copy_ref_<Dest, Src&>
	{
		using type = std::add_lvalue_reference_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_ref_<Dest, Src&&>
	{
		using type = std::add_rvalue_reference_t<Dest>;
	};

	template <typename, typename>
	struct copy_cv_;
	template <typename Dest, typename Src>
	using copy_cv = typename copy_cv_<Dest, Src>::type;

	template <typename Dest, typename Src>
	struct copy_cv_
	{
		using type = Dest;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, const Src>
	{
		using type = std::add_const_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, volatile Src>
	{
		using type = std::add_volatile_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, const volatile Src>
	{
		using type = std::add_cv_t<Dest>;
	};

	template <typename Dest, typename Src>
	using copy_cvref =
		copy_ref<copy_ref<copy_cv<std::remove_reference_t<Dest>, std::remove_reference_t<Src>>, Dest>, Src>;

	template <typename T>
	inline constexpr bool dependent_false = false;

	template <typename T, typename... U>
	inline constexpr bool first_is_same = false;
	template <typename T, typename... U>
	inline constexpr bool first_is_same<T, T, U...> = true;

	// general value traits
	// (as they relate to their equivalent native TOML type)
	template <typename T>
	struct value_traits
	{
		using native_type										  = void;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = false;
		static constexpr bool can_represent_native				  = false;
		static constexpr bool can_partially_represent_native	  = false;
		static constexpr auto type								  = node_type::none;
	};

	template <typename T>
	struct value_traits<const T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<volatile T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<const volatile T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T&> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T&&> : value_traits<T>
	{};

	// integer value_traits specializations - standard types
	template <typename T>
	struct integer_value_limits
	{
		static constexpr auto min = (std::numeric_limits<T>::min)();
		static constexpr auto max = (std::numeric_limits<T>::max)();
	};
	template <typename T>
	struct integer_value_traits_base : integer_value_limits<T>
	{
		using native_type				= int64_t;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = static_cast<T>(-1) < T{}; // for impls not specializing std::is_signed<T>
		static constexpr auto type		= node_type::integer;
		static constexpr bool can_partially_represent_native = true;
	};
	template <typename T>
	struct unsigned_integer_value_traits : integer_value_traits_base<T>
	{
		static constexpr bool is_losslessly_convertible_to_native =
			integer_value_limits<T>::max <= 9223372036854775807ULL;
		static constexpr bool can_represent_native = false;
	};
	template <typename T>
	struct signed_integer_value_traits : integer_value_traits_base<T>
	{
		using native_type = int64_t;
		static constexpr bool is_losslessly_convertible_to_native =
			integer_value_limits<T>::min >= (-9223372036854775807LL - 1LL)
			&& integer_value_limits<T>::max <= 9223372036854775807LL;
		static constexpr bool can_represent_native = integer_value_limits<T>::min <= (-9223372036854775807LL - 1LL)
												  && integer_value_limits<T>::max >= 9223372036854775807LL;
	};
	template <typename T, bool S = integer_value_traits_base<T>::is_signed>
	struct integer_value_traits : signed_integer_value_traits<T>
	{};
	template <typename T>
	struct integer_value_traits<T, false> : unsigned_integer_value_traits<T>
	{};
	template <>
	struct value_traits<signed char> : integer_value_traits<signed char>
	{};
	template <>
	struct value_traits<unsigned char> : integer_value_traits<unsigned char>
	{};
	template <>
	struct value_traits<signed short> : integer_value_traits<signed short>
	{};
	template <>
	struct value_traits<unsigned short> : integer_value_traits<unsigned short>
	{};
	template <>
	struct value_traits<signed int> : integer_value_traits<signed int>
	{};
	template <>
	struct value_traits<unsigned int> : integer_value_traits<unsigned int>
	{};
	template <>
	struct value_traits<signed long> : integer_value_traits<signed long>
	{};
	template <>
	struct value_traits<unsigned long> : integer_value_traits<unsigned long>
	{};
	template <>
	struct value_traits<signed long long> : integer_value_traits<signed long long>
	{};
	template <>
	struct value_traits<unsigned long long> : integer_value_traits<unsigned long long>
	{};
	static_assert(value_traits<int64_t>::is_native);
	static_assert(value_traits<int64_t>::is_signed);
	static_assert(value_traits<int64_t>::is_losslessly_convertible_to_native);
	static_assert(value_traits<int64_t>::can_represent_native);
	static_assert(value_traits<int64_t>::can_partially_represent_native);

	// integer value_traits specializations - non-standard types
#ifdef TOML_INT128
	template <>
	struct integer_value_limits<TOML_INT128>
	{
		static constexpr TOML_INT128 max =
			static_cast<TOML_INT128>((TOML_UINT128{ 1u } << ((__SIZEOF_INT128__ * CHAR_BIT) - 1)) - 1);
		static constexpr TOML_INT128 min = -max - TOML_INT128{ 1 };
	};
	template <>
	struct integer_value_limits<TOML_UINT128>
	{
		static constexpr TOML_UINT128 min = TOML_UINT128{};
		static constexpr TOML_UINT128 max =
			(2u * static_cast<TOML_UINT128>(integer_value_limits<TOML_INT128>::max)) + 1u;
	};
	template <>
	struct value_traits<TOML_INT128> : integer_value_traits<TOML_INT128>
	{};
	template <>
	struct value_traits<TOML_UINT128> : integer_value_traits<TOML_UINT128>
	{};
#endif
#ifdef TOML_SMALL_INT_TYPE
	template <>
	struct value_traits<TOML_SMALL_INT_TYPE> : signed_integer_value_traits<TOML_SMALL_INT_TYPE>
	{};
#endif

	// floating-point value_traits specializations - standard types
	template <typename T>
	struct float_value_limits
	{
		static constexpr bool is_iec559 = std::numeric_limits<T>::is_iec559;
		static constexpr int digits		= std::numeric_limits<T>::digits;
		static constexpr int digits10	= std::numeric_limits<T>::digits10;
	};
	template <typename T>
	struct float_value_traits : float_value_limits<T>
	{
		using native_type				= double;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = true;

		static constexpr bool is_losslessly_convertible_to_native = float_value_limits<T>::is_iec559
																 && float_value_limits<T>::digits <= 53
																 && float_value_limits<T>::digits10 <= 15;

		static constexpr bool can_represent_native = float_value_limits<T>::is_iec559
												  && float_value_limits<T>::digits >= 53	// DBL_MANT_DIG
												  && float_value_limits<T>::digits10 >= 15; // DBL_DIG

		static constexpr bool can_partially_represent_native // 32-bit float values
			= float_value_limits<T>::is_iec559				 //
		   && float_value_limits<T>::digits >= 24			 //
		   && float_value_limits<T>::digits10 >= 6;

		static constexpr auto type = node_type::floating_point;
	};
	template <>
	struct value_traits<float> : float_value_traits<float>
	{};
	template <>
	struct value_traits<double> : float_value_traits<double>
	{};
	template <>
	struct value_traits<long double> : float_value_traits<long double>
	{};
	template <int mant_dig, int dig>
	struct extended_float_value_limits
	{
		static constexpr bool is_iec559 = true;
		static constexpr int digits		= mant_dig;
		static constexpr int digits10	= dig;
	};
	static_assert(value_traits<double>::is_native);
	static_assert(value_traits<double>::is_losslessly_convertible_to_native);
	static_assert(value_traits<double>::can_represent_native);
	static_assert(value_traits<double>::can_partially_represent_native);

	// floating-point value_traits specializations - non-standard types
#ifdef TOML_FP16
	template <>
	struct float_value_limits<TOML_FP16> : extended_float_value_limits<__FLT16_MANT_DIG__, __FLT16_DIG__>
	{};
	template <>
	struct value_traits<TOML_FP16> : float_value_traits<TOML_FP16>
	{};
#endif
#ifdef TOML_FLOAT16
	template <>
	struct float_value_limits<TOML_FLOAT16> : extended_float_value_limits<__FLT16_MANT_DIG__, __FLT16_DIG__>
	{};
	template <>
	struct value_traits<TOML_FLOAT16> : float_value_traits<TOML_FLOAT16>
	{};
#endif
#ifdef TOML_FLOAT128
	template <>
	struct float_value_limits<TOML_FLOAT128> : extended_float_value_limits<__FLT128_MANT_DIG__, __FLT128_DIG__>
	{};
	template <>
	struct value_traits<TOML_FLOAT128> : float_value_traits<TOML_FLOAT128>
	{};
#endif
#ifdef TOML_SMALL_FLOAT_TYPE
	template <>
	struct value_traits<TOML_SMALL_FLOAT_TYPE> : float_value_traits<TOML_SMALL_FLOAT_TYPE>
	{};
#endif

	// string value_traits specializations - char-based strings
	template <typename T>
	struct string_value_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = std::is_same_v<T, native_type>;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native =
			!std::is_array_v<T> && (!std::is_pointer_v<T> || std::is_const_v<std::remove_pointer_t<T>>);
		static constexpr bool can_partially_represent_native = can_represent_native;
		static constexpr auto type							 = node_type::string;
	};
	template <>
	struct value_traits<std::string> : string_value_traits<std::string>
	{};
	template <>
	struct value_traits<std::string_view> : string_value_traits<std::string_view>
	{};
	template <>
	struct value_traits<const char*> : string_value_traits<const char*>
	{};
	template <size_t N>
	struct value_traits<const char[N]> : string_value_traits<const char[N]>
	{};
	template <>
	struct value_traits<char*> : string_value_traits<char*>
	{};
	template <size_t N>
	struct value_traits<char[N]> : string_value_traits<char[N]>
	{};

	// string value_traits specializations - char8_t-based strings
#if TOML_HAS_CHAR8
	template <>
	struct value_traits<std::u8string> : string_value_traits<std::u8string>
	{};
	template <>
	struct value_traits<std::u8string_view> : string_value_traits<std::u8string_view>
	{};
	template <>
	struct value_traits<const char8_t*> : string_value_traits<const char8_t*>
	{};
	template <size_t N>
	struct value_traits<const char8_t[N]> : string_value_traits<const char8_t[N]>
	{};
	template <>
	struct value_traits<char8_t*> : string_value_traits<char8_t*>
	{};
	template <size_t N>
	struct value_traits<char8_t[N]> : string_value_traits<char8_t[N]>
	{};
#endif

	// string value_traits specializations - wchar_t-based strings on Windows
#if TOML_ENABLE_WINDOWS_COMPAT
	template <typename T>
	struct wstring_value_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = true;							 // narrow
		static constexpr bool can_represent_native				  = std::is_same_v<T, std::wstring>; // widen
		static constexpr bool can_partially_represent_native	  = can_represent_native;
		static constexpr auto type								  = node_type::string;
	};
	template <>
	struct value_traits<std::wstring> : wstring_value_traits<std::wstring>
	{};
	template <>
	struct value_traits<std::wstring_view> : wstring_value_traits<std::wstring_view>
	{};
	template <>
	struct value_traits<const wchar_t*> : wstring_value_traits<const wchar_t*>
	{};
	template <size_t N>
	struct value_traits<const wchar_t[N]> : wstring_value_traits<const wchar_t[N]>
	{};
	template <>
	struct value_traits<wchar_t*> : wstring_value_traits<wchar_t*>
	{};
	template <size_t N>
	struct value_traits<wchar_t[N]> : wstring_value_traits<wchar_t[N]>
	{};
#endif

	// other 'native' value_traits specializations
	template <typename T, node_type NodeType>
	struct native_value_traits
	{
		using native_type										  = T;
		static constexpr bool is_native							  = true;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native				  = true;
		static constexpr bool can_partially_represent_native	  = true;
		static constexpr auto type								  = NodeType;
	};
	template <>
	struct value_traits<bool> : native_value_traits<bool, node_type::boolean>
	{};
	template <>
	struct value_traits<date> : native_value_traits<date, node_type::date>
	{};
	template <>
	struct value_traits<time> : native_value_traits<time, node_type::time>
	{};
	template <>
	struct value_traits<date_time> : native_value_traits<date_time, node_type::date_time>
	{};

	// native value category queries
	template <typename T>
	using native_type_of = typename value_traits<T>::native_type;
	template <typename T>
	inline constexpr bool is_native = value_traits<T>::is_native;
	template <typename T>
	inline constexpr bool can_represent_native = value_traits<T>::can_represent_native;
	template <typename T>
	inline constexpr bool can_partially_represent_native = value_traits<T>::can_partially_represent_native;
	template <typename T>
	inline constexpr bool is_losslessly_convertible_to_native = value_traits<T>::is_losslessly_convertible_to_native;
	template <typename T, typename... U>
	inline constexpr bool is_natively_one_of = is_one_of<native_type_of<T>, U...>;

	// native value types => nodes
	template <typename T>
	struct node_wrapper
	{
		using type = T;
	};
	template <typename T>
	struct node_wrapper<const T>
	{
		using type = std::add_const_t<typename node_wrapper<T>::type>;
	};
	template <typename T>
	struct node_wrapper<volatile T>
	{
		using type = std::add_volatile_t<typename node_wrapper<T>::type>;
	};
	template <typename T>
	struct node_wrapper<const volatile T>
	{
		using type = std::add_const_t<std::add_volatile_t<typename node_wrapper<T>::type>>;
	};
	template <>
	struct node_wrapper<std::string>
	{
		using type = value<std::string>;
	};
	template <>
	struct node_wrapper<int64_t>
	{
		using type = value<int64_t>;
	};
	template <>
	struct node_wrapper<double>
	{
		using type = value<double>;
	};
	template <>
	struct node_wrapper<bool>
	{
		using type = value<bool>;
	};
	template <>
	struct node_wrapper<date>
	{
		using type = value<date>;
	};
	template <>
	struct node_wrapper<time>
	{
		using type = value<time>;
	};
	template <>
	struct node_wrapper<date_time>
	{
		using type = value<date_time>;
	};
	template <typename T>
	using wrap_node = typename node_wrapper<std::remove_reference_t<T>>::type;

	// nodes => native value types
	template <typename T>
	struct node_unwrapper
	{
		using type = T;
	};
	template <typename T>
	struct node_unwrapper<value<T>>
	{
		using type = T;
	};
	template <typename T>
	struct node_unwrapper<const value<T>>
	{
		using type = std::add_const_t<T>;
	};
	template <typename T>
	struct node_unwrapper<volatile value<T>>
	{
		using type = std::add_volatile_t<T>;
	};
	template <typename T>
	struct node_unwrapper<const volatile value<T>>
	{
		using type = std::add_volatile_t<std::add_const_t<T>>;
	};
	template <typename T>
	using unwrap_node = typename node_unwrapper<std::remove_reference_t<T>>::type;

	template <typename T>
	struct node_type_getter
	{
		static constexpr auto value = value_traits<T>::type;
	};
	template <>
	struct node_type_getter<table>
	{
		static constexpr auto value = node_type::table;
	};
	template <>
	struct node_type_getter<array>
	{
		static constexpr auto value = node_type::array;
	};
	template <>
	struct node_type_getter<void>
	{
		static constexpr auto value = node_type::none;
	};
	template <typename T>
	inline constexpr node_type node_type_of = node_type_getter<unwrap_node<remove_cvref<T>>>::value;
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref<T>, table>;

	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref<T>, array>;

	template <typename T>
	inline constexpr bool is_container = is_table<T> || is_array<T>;

	template <typename T>
	inline constexpr bool is_string = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<std::string>>;

	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<int64_t>>;

	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<		//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<double>>;

	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;

	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<bool>>;

	template <typename T>
	inline constexpr bool is_date = std::is_same_v<					//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<date>>;

	template <typename T>
	inline constexpr bool is_time = std::is_same_v<					//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<time>>;

	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<			//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<date_time>>;

	template <typename T>
	inline constexpr bool is_chronological = is_date<T> || is_time<T> || is_date_time<T>;

	template <typename T>
	inline constexpr bool is_value = is_string<T> || is_number<T> || is_boolean<T> || is_chronological<T>;

	template <typename T>
	inline constexpr bool is_node =
		std::is_same_v<toml::node, impl::remove_cvref<T>> || std::is_base_of_v<toml::node, impl::remove_cvref<T>>;

	template <typename T>
	inline constexpr bool is_node_view = impl::is_one_of<impl::remove_cvref<T>, node_view<node>, node_view<const node>>;
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_CONST_INLINE_GETTER
	constexpr std::underlying_type_t<T> unwrap_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use std::fpclassify?"
	// A: Because it gets broken by -ffast-math and friends
	enum class TOML_CLOSED_ENUM fp_class : unsigned
	{
		ok,
		neg_inf,
		pos_inf,
		nan
	};

	TOML_PURE_GETTER
	inline fp_class fpclassify(const double& val) noexcept
	{
		static_assert(sizeof(uint64_t) == sizeof(double));

		static constexpr uint64_t sign	   = 0b1000000000000000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t exponent = 0b0111111111110000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t mantissa = 0b0000000000001111111111111111111111111111111111111111111111111111ull;

		uint64_t val_bits;
		std::memcpy(&val_bits, &val, sizeof(val));
		if ((val_bits & exponent) != exponent)
			return fp_class::ok;
		if ((val_bits & mantissa))
			return fp_class::nan;
		return (val_bits & sign) ? fp_class::neg_inf : fp_class::pos_inf;
	}

	// Q: "why not use std::find and std::min?"
	// A: Because <algorithm> is _huge_ and these would be the only things I used from it.
	//    I don't want to impose such a heavy compile-time burden on users.

	template <typename Iterator, typename T>
	TOML_PURE_GETTER
	inline auto find(Iterator start, Iterator end, const T& needle) noexcept //
		->decltype(&(*start))
	{
		for (; start != end; start++)
			if (*start == needle)
				return &(*start);
		return nullptr;
	}

	template <typename T>
	TOML_PURE_GETTER
	constexpr const T& min(const T& a, const T& b) noexcept //
	{
		return a < b ? a : b;
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/print_to_stream.h  ************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm supporting C++20's char8_t as well; wrapping streams allows switching string modes transparently.
	//    - I'm using <charconv> to format numerics. Faster and locale-independent.
	//    - I can (potentially) avoid forcing users to drag in <sstream> and <iomanip>.
	//    - Strings in C++. Honestly.

	TOML_EXPORTED_FREE_FUNCTION
	TOML_ATTR(nonnull)
	void print_to_stream(std::ostream&, const char*, size_t);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, std::string_view);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const std::string&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, char);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, int8_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, int16_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, int32_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, int64_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, uint8_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, uint16_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, uint32_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, uint64_t, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, float, value_flags = {}, bool relaxed_precision = false);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, double, value_flags = {}, bool relaxed_precision = false);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, bool);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const toml::date&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const toml::time&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const toml::time_offset&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const toml::date_time&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const source_position&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const source_region&);

#if TOML_ENABLE_FORMATTERS

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const array&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const table&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<std::string>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<int64_t>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<double>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<bool>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<date>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<time>&);

	TOML_EXPORTED_FREE_FUNCTION
	void print_to_stream(std::ostream&, const value<date_time>&);

#endif

	template <typename T, typename U>
	inline void print_to_stream_bookended(std::ostream & stream, const T& val, const U& bookend)
	{
		print_to_stream(stream, bookend);
		print_to_stream(stream, val);
		print_to_stream(stream, bookend);
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/source_region.h  **************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	using source_index = uint32_t;

	using source_path_ptr = std::shared_ptr<const std::string>;

	struct TOML_TRIVIAL_ABI source_position
	{
		source_index line;

		source_index column;

		TOML_NODISCARD
		explicit constexpr operator bool() const noexcept
		{
			return line > source_index{} && column > source_index{};
		}

		TOML_NODISCARD
		friend constexpr bool operator==(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line && lhs.column == rhs.column;
		}

		TOML_NODISCARD
		friend constexpr bool operator!=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line != rhs.line || lhs.column != rhs.column;
		}

		TOML_NODISCARD
		friend constexpr bool operator<(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column < rhs.column);
		}

		TOML_NODISCARD
		friend constexpr bool operator<=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column <= rhs.column);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const source_position& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct source_region
	{
		source_position begin;

		source_position end;

		source_path_ptr path;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		optional<std::wstring> wide_path() const
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, const source_region& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/date_time.h  ******************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	struct TOML_TRIVIAL_ABI date
	{
		uint16_t year;

		uint8_t month;

		uint8_t day;

		TOML_NODISCARD_CTOR
		date() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<Y, M, D>), typename Y, typename M, typename D)
		TOML_NODISCARD_CTOR
		constexpr date(Y y, M m, D d) noexcept //
			: year{ static_cast<uint16_t>(y) },
			  month{ static_cast<uint8_t>(m) },
			  day{ static_cast<uint8_t>(d) }
		{}

		TOML_PURE_GETTER
		friend constexpr bool operator==(const date& lhs, const date& rhs) noexcept
		{
			return lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator!=(const date& lhs, const date& rhs) noexcept
		{
			return lhs.year != rhs.year || lhs.month != rhs.month || lhs.day != rhs.day;
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint32_t pack(const date& d) noexcept
		{
			return (static_cast<uint32_t>(d.year) << 16) | (static_cast<uint32_t>(d.month) << 8)
				 | static_cast<uint32_t>(d.day);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(const date& lhs, const date& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const date& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct TOML_TRIVIAL_ABI time
	{
		uint8_t hour;

		uint8_t minute;

		uint8_t second;

		uint32_t nanosecond;

		TOML_NODISCARD_CTOR
		time() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M, S, NS>),
								  typename H,
								  typename M,
								  typename S  = uint8_t,
								  typename NS = uint32_t)
		TOML_NODISCARD_CTOR
		constexpr time(H h, M m, S s = S{}, NS ns = NS{}) noexcept //
			: hour{ static_cast<uint8_t>(h) },
			  minute{ static_cast<uint8_t>(m) },
			  second{ static_cast<uint8_t>(s) },
			  nanosecond{ static_cast<uint32_t>(ns) }
		{}

		TOML_PURE_GETTER
		friend constexpr bool operator==(const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour && lhs.minute == rhs.minute && lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator!=(const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint64_t pack(const time& t) noexcept
		{
			return static_cast<uint64_t>(t.hour) << 48 | static_cast<uint64_t>(t.minute) << 40
				 | static_cast<uint64_t>(t.second) << 32 | static_cast<uint64_t>(t.nanosecond);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(const time& lhs, const time& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct TOML_TRIVIAL_ABI time_offset
	{
		int16_t minutes;

		TOML_NODISCARD_CTOR
		time_offset() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M>), typename H, typename M)
		TOML_NODISCARD_CTOR
		constexpr time_offset(H h, M m) noexcept //
			: minutes{ static_cast<int16_t>(static_cast<impl::common_signed_type<H, M>>(h)
												* impl::common_signed_type<H, M>{ 60 }
											+ static_cast<impl::common_signed_type<H, M>>(m)) }
		{}

		TOML_PURE_GETTER
		friend constexpr bool operator==(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator!=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes < rhs.minutes;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes <= rhs.minutes;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes > rhs.minutes;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes >= rhs.minutes;
		}

		friend std::ostream& operator<<(std::ostream& lhs, const time_offset& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);

	struct date_time
	{
		toml::date date;

		toml::time time;

		optional<toml::time_offset> offset;

		TOML_NODISCARD_CTOR
		date_time() noexcept = default;

		TOML_NODISCARD_CTOR
		constexpr date_time(const toml::date& d, const toml::time& t) noexcept //
			: date{ d },
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		explicit constexpr date_time(const toml::date& d) noexcept //
			: date{ d },
			  time{},
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		explicit constexpr date_time(const toml::time& t) noexcept //
			: date{},
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		constexpr date_time(const toml::date& d, const toml::time& t, const toml::time_offset& off) noexcept
			: date{ d },
			  time{ t },
			  offset{ off }
		{}

		TOML_PURE_GETTER
		constexpr bool is_local() const noexcept
		{
			return !offset.has_value();
		}

		TOML_PURE_GETTER
		friend constexpr bool operator==(const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date && lhs.time == rhs.time && lhs.offset == rhs.offset;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator!=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset < rhs.offset;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset <= rhs.offset;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs <= rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs < rhs);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const date_time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END; // TOML_HAS_CUSTOM_OPTIONAL_TYPE
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/at_path.h  ********************************************************************************************

TOML_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> at_path(node & root, std::string_view path) noexcept;

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> at_path(const node& root, std::string_view path) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> at_path(node & root, std::wstring_view path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> at_path(const node& root, std::wstring_view path);

#endif
}
TOML_NAMESPACE_END;

//********  impl/node.h  ***********************************************************************************************

TOML_DISABLE_WARNINGS;
#include <utility>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_ABSTRACT_BASE TOML_EXPORTED_CLASS node
	{
	  private:

		friend class TOML_PARSER_TYPENAME;
		source_region source_{};

		template <typename T>
		TOML_NODISCARD
		decltype(auto) get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T, typename N>
		using ref_type_ = std::conditional_t<													//
			std::is_reference_v<T>,																//
			impl::copy_ref<impl::copy_cv<impl::unwrap_node<T>, std::remove_reference_t<N>>, T>, //
			impl::copy_cvref<impl::unwrap_node<T>, N>											//
			>;

		template <typename T, typename N>
		using ref_type = std::conditional_t<			 //
			std::is_reference_v<N>,						 //
			ref_type_<T, N>,							 //
			ref_type_<T, std::add_lvalue_reference_t<N>> //
			>;

		template <typename T, typename N>
		TOML_PURE_GETTER
		static ref_type<T, N&&> do_ref(N&& n) noexcept
		{
			using unwrapped_type = impl::unwrap_node<T>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::ref() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			TOML_ASSERT_ASSUME(
				n.template is<unwrapped_type>()
				&& "template type argument provided to toml::node::ref() didn't match the node's actual type");

			using node_ref = std::remove_volatile_t<std::remove_reference_t<N>>&;
			using val_type = std::remove_volatile_t<unwrapped_type>;
			using out_ref  = ref_type<T, N&&>;
			static_assert(std::is_reference_v<out_ref>);

			if constexpr (toml::is_value<unwrapped_type>)
				return static_cast<out_ref>(const_cast<node_ref>(n).template ref_cast<val_type>().get());
			else
				return static_cast<out_ref>(const_cast<node_ref>(n).template ref_cast<val_type>());
		}

	  protected:
		TOML_EXPORTED_MEMBER_FUNCTION
		node() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(node&&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(node&&) noexcept;

		template <typename T, typename N>
		using ref_cast_type_ = std::conditional_t<											  //
			std::is_reference_v<T>,															  //
			impl::copy_ref<impl::copy_cv<impl::wrap_node<T>, std::remove_reference_t<N>>, T>, //
			impl::copy_cvref<impl::wrap_node<T>, N>											  //
			>;

		template <typename T, typename N>
		using ref_cast_type = std::conditional_t<			  //
			std::is_reference_v<N>,							  //
			ref_cast_type_<T, N>,							  //
			ref_cast_type_<T, std::add_lvalue_reference_t<N>> //
			>;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, node&> ref_cast() & noexcept
		{
			using out_ref  = ref_cast_type<T, node&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, node&&> ref_cast() && noexcept
		{
			using out_ref  = ref_cast_type<T, node&&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, const node&> ref_cast() const& noexcept
		{
			using out_ref  = ref_cast_type<T, const node&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, const node&&> ref_cast() const&& noexcept
		{
			using out_ref  = ref_cast_type<T, const node&&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

	  public:
		TOML_EXPORTED_MEMBER_FUNCTION
		virtual ~node() noexcept;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype) const noexcept = 0;

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<ElemType>>;
			static_assert(std::is_void_v<unwrapped_type> //
							  || (toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>),
						  "The template type argument of node::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<unwrapped_type>);
		}

		TOML_PURE_GETTER
		virtual node_type type() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_table() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_array_of_tables() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_value() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_number() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		bool is() const noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::is() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return is_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return is_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return is_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return is_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return is_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return is_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return is_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return is_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return is_date_time();
		}

		TOML_PURE_GETTER
		virtual table* as_table() noexcept = 0;

		TOML_PURE_GETTER
		virtual array* as_array() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<std::string>* as_string() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<int64_t>* as_integer() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<double>* as_floating_point() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<bool>* as_boolean() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date>* as_date() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<time>* as_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date_time>* as_date_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual const table* as_table() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const array* as_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<std::string>* as_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<int64_t>* as_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<double>* as_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<bool>* as_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date>* as_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<time>* as_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date_time>* as_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		impl::wrap_node<T>* as() noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::as() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return as_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return as_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return as_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return as_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return as_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return as_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return as_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return as_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return as_date_time();
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		const impl::wrap_node<T>* as() const noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::as() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return as_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return as_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return as_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return as_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return as_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return as_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return as_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return as_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return as_date_time();
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() & noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() && noexcept
		{
			return do_ref<T>(std::move(*this));
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() const& noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() const&& noexcept
		{
			return do_ref<T>(std::move(*this));
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

	  private:

		// clang-format off

		template <typename Func, typename N, typename T>
		static constexpr bool can_visit = std::is_invocable_v<Func, ref_cast_type<T, N>>;

		template <typename Func, typename N>
		static constexpr bool can_visit_any =
			can_visit<Func, N, table>
			|| can_visit<Func, N, array>
			|| can_visit<Func, N, std::string>
			|| can_visit<Func, N, int64_t>
			|| can_visit<Func, N, double>
			|| can_visit<Func, N, bool>
			|| can_visit<Func, N, date>
			|| can_visit<Func, N, time>
			|| can_visit<Func, N, date_time>;

		template <typename Func, typename N>
		static constexpr bool can_visit_all =
			can_visit<Func, N, table>
			&& can_visit<Func, N, array>
			&& can_visit<Func, N, std::string>
			&& can_visit<Func, N, int64_t>
			&& can_visit<Func, N, double>
			&& can_visit<Func, N, bool>
			&& can_visit<Func, N, date>
			&& can_visit<Func, N, time>
			&& can_visit<Func, N, date_time>;

		template <typename Func, typename N, typename T>
		static constexpr bool visit_is_nothrow_one =
			!can_visit<Func, N, T>
			|| std::is_nothrow_invocable_v<Func, ref_cast_type<T, N>>;

		template <typename Func, typename N>
		static constexpr bool visit_is_nothrow =
			visit_is_nothrow_one<Func, N, table>
			&& visit_is_nothrow_one<Func, N, array>
			&& visit_is_nothrow_one<Func, N, std::string>
			&& visit_is_nothrow_one<Func, N, int64_t>
			&& visit_is_nothrow_one<Func, N, double>
			&& visit_is_nothrow_one<Func, N, bool>
			&& visit_is_nothrow_one<Func, N, date>
			&& visit_is_nothrow_one<Func, N, time>
			&& visit_is_nothrow_one<Func, N, date_time>;

		// clang-format on

		template <typename Func, typename N, typename T, bool = can_visit<Func, N, T>>
		struct visit_return_type_
		{
			using type = decltype(std::declval<Func>()(std::declval<ref_cast_type<T, N>>()));
		};
		template <typename Func, typename N, typename T>
		struct visit_return_type_<Func, N, T, false>
		{
			using type = void;
		};

		template <typename Func, typename N, typename T>
		using visit_return_type = typename visit_return_type_<Func, N, T>::type;

		template <typename A, typename B>
		using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

		template <typename N, typename Func>
		static decltype(auto) do_visit(N&& n, Func&& visitor) noexcept(visit_is_nothrow<Func&&, N&&>)
		{
			static_assert(can_visit_any<Func&&, N&&>,
						  "TOML node visitors must be invocable for at least one of the toml::node "
						  "specializations:" TOML_SA_NODE_TYPE_LIST);

			switch (n.type())
			{
				case node_type::table:
					if constexpr (can_visit<Func&&, N&&, table>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<table>());
					break;

				case node_type::array:
					if constexpr (can_visit<Func&&, N&&, array>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<array>());
					break;

				case node_type::string:
					if constexpr (can_visit<Func&&, N&&, std::string>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<std::string>());
					break;

				case node_type::integer:
					if constexpr (can_visit<Func&&, N&&, int64_t>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<int64_t>());
					break;

				case node_type::floating_point:
					if constexpr (can_visit<Func&&, N&&, double>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<double>());
					break;

				case node_type::boolean:
					if constexpr (can_visit<Func&&, N&&, bool>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<bool>());
					break;

				case node_type::date:
					if constexpr (can_visit<Func&&, N&&, date>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<date>());
					break;

				case node_type::time:
					if constexpr (can_visit<Func&&, N&&, time>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<time>());
					break;

				case node_type::date_time:
					if constexpr (can_visit<Func&&, N&&, date_time>)
						return static_cast<Func&&>(visitor)(static_cast<N&&>(n).template ref_cast<date_time>());
					break;

				case node_type::none: TOML_UNREACHABLE;
				default: TOML_UNREACHABLE;
			}

			if constexpr (can_visit_all<Func&&, N&&>)
				TOML_UNREACHABLE;
			else
			{
				// clang-format off

				using return_type =
					nonvoid<visit_return_type<Func&&, N&&, table>,
					nonvoid<visit_return_type<Func&&, N&&, array>,
					nonvoid<visit_return_type<Func&&, N&&, std::string>,
					nonvoid<visit_return_type<Func&&, N&&, int64_t>,
					nonvoid<visit_return_type<Func&&, N&&, double>,
					nonvoid<visit_return_type<Func&&, N&&, bool>,
					nonvoid<visit_return_type<Func&&, N&&, date>,
					nonvoid<visit_return_type<Func&&, N&&, time>,
							visit_return_type<Func&&, N&&, date_time>
				>>>>>>>>;

				// clang-format on

				if constexpr (!std::is_void_v<return_type>)
				{
					static_assert(std::is_default_constructible_v<return_type>,
								  "Non-exhaustive visitors must return a default-constructible type, or void");
					return return_type{};
				}
			}
		}

	  public:

		template <typename Func>
		decltype(auto) visit(Func&& visitor) & noexcept(visit_is_nothrow<Func&&, node&>)
		{
			return do_visit(*this, static_cast<Func&&>(visitor));
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) && noexcept(visit_is_nothrow<Func&&, node&&>)
		{
			return do_visit(static_cast<node&&>(*this), static_cast<Func&&>(visitor));
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const& noexcept(visit_is_nothrow<Func&&, const node&>)
		{
			return do_visit(*this, static_cast<Func&&>(visitor));
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const&& noexcept(visit_is_nothrow<Func&&, const node&&>)
		{
			return do_visit(static_cast<const node&&>(*this), static_cast<Func&&>(visitor));
		}

		TOML_NODISCARD
		explicit operator node_view<node>() noexcept;

		TOML_NODISCARD
		explicit operator node_view<const node>() const noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::string_view path) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::string_view path) const noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::wstring_view path);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::wstring_view path) const;

#endif // TOML_ENABLE_WINDOWS_COMPAT
	};
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_EXPORTED_FREE_FUNCTION
	bool node_deep_equality(const node*, const node*) noexcept;
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/node_view.h  ******************************************************************************************

TOML_DISABLE_WARNINGS;
#include <vector>
#include <iterator>
#include <initializer_list>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_DISABLE_ARITHMETIC_WARNINGS;

TOML_NAMESPACE_START
{
	template <typename ViewedType>
	class TOML_TRIVIAL_ABI node_view
	{
		static_assert(impl::is_one_of<ViewedType, toml::node, const toml::node>,
					  "A toml::node_view<> must wrap toml::node or const toml::node.");

	  public:

		using viewed_type = ViewedType;

	  private:
		template <typename T>
		friend class node_view;

		mutable viewed_type* node_ = nullptr;

		template <typename Func>
		static constexpr bool visit_is_nothrow = noexcept(std::declval<viewed_type*>()->visit(std::declval<Func&&>()));

	  public:

		TOML_NODISCARD_CTOR
		node_view() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type* node) noexcept //
			: node_{ node }
		{}

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type& node) noexcept //
			: node_{ &node }
		{}

		TOML_NODISCARD_CTOR
		node_view(const node_view&) noexcept = default;

		TOML_NODISCARD_CTOR
		node_view(node_view&&) noexcept = default;

		node_view& operator=(const node_view&) & noexcept = default;

		node_view& operator=(node_view&&) & noexcept = default;

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return node_ != nullptr;
		}

		TOML_PURE_INLINE_GETTER
		viewed_type* node() const noexcept
		{
			return node_;
		}

		TOML_PURE_GETTER
		node_type type() const noexcept
		{
			return node_ ? node_->type() : node_type::none;
		}

		TOML_PURE_GETTER
		bool is_table() const noexcept
		{
			return node_ && node_->is_table();
		}

		TOML_PURE_GETTER
		bool is_array() const noexcept
		{
			return node_ && node_->is_array();
		}

		TOML_PURE_GETTER
		bool is_value() const noexcept
		{
			return node_ && node_->is_value();
		}

		TOML_PURE_GETTER
		bool is_string() const noexcept
		{
			return node_ && node_->is_string();
		}

		TOML_PURE_GETTER
		bool is_integer() const noexcept
		{
			return node_ && node_->is_integer();
		}

		TOML_PURE_GETTER
		bool is_floating_point() const noexcept
		{
			return node_ && node_->is_floating_point();
		}

		TOML_PURE_GETTER
		bool is_number() const noexcept
		{
			return node_ && node_->is_number();
		}

		TOML_PURE_GETTER
		bool is_boolean() const noexcept
		{
			return node_ && node_->is_boolean();
		}

		TOML_PURE_GETTER
		bool is_date() const noexcept
		{
			return node_ && node_->is_date();
		}

		TOML_PURE_GETTER
		bool is_time() const noexcept
		{
			return node_ && node_->is_time();
		}

		TOML_PURE_GETTER
		bool is_date_time() const noexcept
		{
			return node_ && node_->is_date_time();
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept
		{
			return node_ && node_->is_array_of_tables();
		}

		template <typename T>
		TOML_PURE_GETTER
		bool is() const noexcept
		{
			return node_ ? node_->template is<impl::unwrap_node<impl::remove_cvref<T>>>() : false;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, viewed_type*& first_nonmatch) const noexcept
		{
			if (!node_)
			{
				first_nonmatch = {};
				return false;
			}
			return node_->is_homogeneous(ntype, first_nonmatch);
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype) const noexcept
		{
			return node_ ? node_->is_homogeneous(ntype) : false;
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			return node_ ? node_->template is_homogeneous<impl::unwrap_node<impl::remove_cvref<ElemType>>>() : false;
		}

		template <typename T>
		TOML_PURE_GETTER
		auto* as() const noexcept
		{
			return node_ ? node_->template as<T>() : nullptr;
		}

		TOML_PURE_GETTER
		auto* as_table() const noexcept
		{
			return as<table>();
		}

		TOML_PURE_GETTER
		auto* as_array() const noexcept
		{
			return as<array>();
		}

		TOML_PURE_GETTER
		auto* as_string() const noexcept
		{
			return as<std::string>();
		}

		TOML_PURE_GETTER
		auto* as_integer() const noexcept
		{
			return as<int64_t>();
		}

		TOML_PURE_GETTER
		auto* as_floating_point() const noexcept
		{
			return as<double>();
		}

		TOML_PURE_GETTER
		auto* as_boolean() const noexcept
		{
			return as<bool>();
		}

		TOML_PURE_GETTER
		auto* as_date() const noexcept
		{
			return as<date>();
		}

		TOML_PURE_GETTER
		auto* as_time() const noexcept
		{
			return as<time>();
		}

		TOML_PURE_GETTER
		auto* as_date_time() const noexcept
		{
			return as<date_time>();
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value_exact<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			using namespace ::toml::impl;

			static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Retrieving values as wide-character strings is only "
						  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_wide_string<T>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT

				if (node_)
					return node_->value_or(static_cast<T&&>(default_value));
				return std::wstring{ static_cast<T&&>(default_value) };

#else

				static_assert(impl::dependent_false<T>, "Evaluated unreachable branch!");

#endif
			}
			else
			{
				using value_type =
					std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
									   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
									   std::decay_t<T>>;

				if (node_)
					return node_->value_or(static_cast<T&&>(default_value));
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() const noexcept
		{
			TOML_ASSERT_ASSUME(node_ && "toml::node_view::ref() called on a node_view that did not reference a node");
			return node_->template ref<T>();
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const noexcept(visit_is_nothrow<Func&&>)
		{
			using return_type = decltype(node_->visit(static_cast<Func&&>(visitor)));
			if (node_)
				return node_->visit(static_cast<Func&&>(visitor));
			if constexpr (!std::is_void_v<return_type>)
				return return_type{};
		}

	  public:

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return impl::node_deep_equality(lhs.node_, rhs.node_);
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator!=(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return !impl::node_deep_equality(lhs.node_, rhs.node_);
		}

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const table& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto tbl = lhs.as<table>();
			return tbl && *tbl == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const table&, );

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const array& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const array&, );

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const toml::value<T>& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto val = lhs.as<T>();
			return val && *val == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const toml::value<T>&, template <typename T>);

		TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>, typename T)
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const T& rhs) noexcept(!impl::is_wide_string<T>)
		{
			static_assert(!impl::is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Comparison with wide-character strings is only "
						  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (impl::is_wide_string<T>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return lhs == impl::narrow(rhs);
#else
				static_assert(impl::dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto val = lhs.as<impl::native_type_of<T>>();
				return val && *val == rhs;
			}
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&,
									   const T&,
									   TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>,
																 typename T));

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs,
							   const std::initializer_list<T>& rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::initializer_list<T>&, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const std::vector<T>& rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::vector<T>&, template <typename T>);

		TOML_NODISCARD
		node_view operator[](std::string_view key) const noexcept
		{
			if (auto tbl = this->as_table())
				return node_view{ tbl->get(key) };
			return {};
		}

		TOML_NODISCARD
		node_view at_path(std::string_view path) const noexcept
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view operator[](std::wstring_view key) const
		{
			if (auto tbl = this->as_table())
				return node_view{ tbl->get(key) };
			return {};
		}

		TOML_NODISCARD
		node_view at_path(std::wstring_view path) const
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view operator[](size_t index) const noexcept
		{
			if (auto arr = this->as_array())
				return node_view{ arr->get(index) };
			return {};
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& os, const node_view& nv)
		{
			if (nv.node_)
				nv.node_->visit([&os](const auto& n) { os << n; });
			return os;
		}

#endif
	};

	template <typename T>
	node_view(const T&) -> node_view<const node>;

	template <typename T>
	node_view(const T*) -> node_view<const node>;

	template <typename T>
	node_view(T&) -> node_view<node>;

	template <typename T>
	node_view(T*) -> node_view<node>;
}
TOML_NAMESPACE_END;

TOML_NAMESPACE_START
{
	inline node::operator node_view<node>() noexcept
	{
		return node_view<node>{ this };
	}

	inline node::operator node_view<const node>() const noexcept
	{
		return node_view<const node>{ this };
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/value.h  **********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_DISABLE_ARITHMETIC_WARNINGS;

// clang-format off

#if TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_SA_VALUE_MESSAGE_WSTRING			TOML_SA_LIST_SEP "std::wstring"
#else
	#define TOML_SA_VALUE_MESSAGE_WSTRING
#endif
#if TOML_HAS_CHAR8
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW		TOML_SA_LIST_SEP "std::u8string_view"
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8		TOML_SA_LIST_SEP "const char8_t*"
#else
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#endif

#define TOML_SA_VALUE_EXACT_FUNC_MESSAGE(type_arg)															\
	"The " type_arg " must be one of:"																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "An immutable view type not requiring additional temporary storage"					\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
		TOML_SA_LIST_END

#define TOML_SA_VALUE_FUNC_MESSAGE(type_arg)																\
	"The " type_arg " must be one of:"																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"		\
	TOML_SA_LIST_BEG "any other integer type"																\
	TOML_SA_LIST_SEP "any floating-point type >= 32 bits"													\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "An immutable view type not requiring additional temporary storage"					\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
	TOML_SA_LIST_END

// clang-format on
TOML_IMPL_NAMESPACE_START
{
	template <typename T, typename...>
	struct native_value_maker
	{
		template <typename... Args>
		TOML_NODISCARD
		static T make(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
		{
			if constexpr (std::is_aggregate_v<T>)
				return T{ static_cast<Args&&>(args)... };
			else
				return T(static_cast<Args&&>(args)...);
		}
	};

	template <typename T>
	struct native_value_maker<T, T>
	{
		template <typename U>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static U&& make(U&& val) noexcept
		{
			return static_cast<U&&>(val);
		}
	};

#if TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	struct string_maker
	{
		template <typename T>
		TOML_NODISCARD
		static std::string make(T&& arg) noexcept
		{
			using arg_type = std::decay_t<T>;
#if TOML_HAS_CHAR8
			if constexpr (is_one_of<arg_type, char8_t*, const char8_t*>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg)));
			}
			if constexpr (is_one_of<arg_type, std::u8string, std::u8string_view>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg.data())),
								   arg.length());
			}
#endif

#if TOML_ENABLE_WINDOWS_COMPAT
			if constexpr (is_wide_string<arg_type>)
			{
				return narrow(static_cast<T&&>(arg));
			}
#endif
		}
	};

#if TOML_HAS_CHAR8
	template <>
	struct native_value_maker<std::string, char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string_view> : string_maker
	{};
#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT
	template <>
	struct native_value_maker<std::string, wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring_view> : string_maker
	{};
#endif // TOML_ENABLE_WINDOWS_COMPAT

#endif // TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_integer_cast(int64_t val) noexcept
	{
		static_assert(node_type_of<T> == node_type::integer);
		static_assert(!is_cvref<T>);

		using traits = value_traits<T>;
		if constexpr (!traits::is_signed)
		{
			if constexpr ((sizeof(T) * CHAR_BIT) < 63) // 63 bits == int64_max
			{
				using common_t = decltype(int64_t{} + T{});
				if (val < int64_t{} || static_cast<common_t>(val) > static_cast<common_t>(traits::max))
					return {};
			}
			else
			{
				if (val < int64_t{})
					return {};
			}
		}
		else
		{
			if (val < traits::min || val > traits::max)
				return {};
		}
		return { static_cast<T>(val) };
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename ValueType>
	class value : public node
	{
		static_assert(impl::is_native<ValueType> && !impl::is_cvref<ValueType>,
					  "A toml::value<> must model one of the native TOML value types:" TOML_SA_NATIVE_VALUE_TYPE_LIST);

	  private:

		friend class TOML_PARSER_TYPENAME;

		template <typename T, typename U>
		TOML_CONST_INLINE_GETTER
		static auto as_value([[maybe_unused]] U* ptr) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return ptr;
			else
				return nullptr;
		}

		ValueType val_;
		value_flags flags_ = value_flags::none;

	  public:

		using value_type = ValueType;

		using value_arg = POXY_IMPLEMENTATION_DETAIL(
			std::conditional_t<
				std::is_same_v<value_type, std::string>,
				std::string_view,
				std::conditional_t<impl::is_one_of<value_type, double, int64_t, bool>, value_type, const value_type&>>);

		template <typename... Args>
		TOML_NODISCARD_CTOR
		explicit value(Args&&... args) noexcept(noexcept(value_type(
			impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))))
			: val_(impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other, value_flags flags) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(value&& other) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(value&& other, value_flags flags) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		value& operator=(const value& rhs) noexcept
		{
			node::operator=(rhs);
			val_		  = rhs.val_;
			flags_		  = rhs.flags_;
			return *this;
		}

		value& operator=(value&& rhs) noexcept
		{
			if (&rhs != this)
			{
				node::operator=(std::move(rhs));
				val_		  = std::move(rhs.val_);
				flags_		  = rhs.flags_;
			}
			return *this;
		}

#if TOML_LIFETIME_HOOKS
		~value() noexcept
		{
			TOML_VALUE_DESTROYED;
		}
#endif

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype) const noexcept final
		{
			return ntype == node_type::none || ntype == impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final
		{
			if (ntype != node_type::none && ntype != impl::node_type_of<value_type>)
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final
		{
			if (ntype != node_type::none && ntype != impl::node_type_of<value_type>)
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::unwrap_node<ElemType>;
			static_assert(
				std::is_void_v<
					type> || ((impl::is_native<type> || impl::is_one_of<type, table, array>)&&!impl::is_cvref<type>),
				"The template type argument of value::is_homogeneous() must be void or one "
				"of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			using type = impl::unwrap_node<ElemType>;
			if constexpr (std::is_void_v<type>)
				return true;
			else
				return impl::node_type_of<type> == impl::node_type_of<value_type>;
		}
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return std::is_same_v<value_type, std::string>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return std::is_same_v<value_type, int64_t>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return std::is_same_v<value_type, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return impl::is_one_of<value_type, int64_t, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return std::is_same_v<value_type, bool>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return std::is_same_v<value_type, date>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return std::is_same_v<value_type, time>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return std::is_same_v<value_type, date_time>;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		value<std::string>* as_string() noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<int64_t>* as_integer() noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<double>* as_floating_point() noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<bool>* as_boolean() noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date>* as_date() noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<time>* as_time() noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date_time>* as_date_time() noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const value<std::string>* as_string() const noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<int64_t>* as_integer() const noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<double>* as_floating_point() const noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<bool>* as_boolean() const noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date>* as_date() const noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<time>* as_time() const noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date_time>* as_date_time() const noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_PURE_INLINE_GETTER
		value_type& get() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& get() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& get() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& get() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		value_type& operator*() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& operator*() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& operator*() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& operator*() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&&() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&&() && noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		value_type* operator->() noexcept
		{
			return &val_;
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		const value_type* operator->() const noexcept
		{
			return &val_;
		}

		TOML_NODISCARD
		value_flags flags() const noexcept
		{
			return flags_;
		}

		value& flags(value_flags new_flags) noexcept
		{
			flags_ = new_flags;
			return *this;
		}

		value& operator=(value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, std::string>)
				val_.assign(rhs);
			else
				val_ = rhs;
			return *this;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_same_v<T, std::string>), typename T = value_type)
		value& operator=(std::string&& rhs) noexcept
		{
			val_ = std::move(rhs);
			return *this;
		}

		TOML_NODISCARD
		friend bool operator==(const value& lhs, value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, double>)
			{
				const auto lhs_nan = impl::fpclassify(lhs.val_) == impl::fp_class::nan;
				const auto rhs_nan = impl::fpclassify(rhs) == impl::fp_class::nan;
				if (lhs_nan != rhs_nan)
					return false;
				if (lhs_nan)
					return true;
			}
			return lhs.val_ == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, );

		TOML_NODISCARD
		friend bool operator<(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ < rhs;
		}

		TOML_NODISCARD
		friend bool operator<(value_arg lhs, const value& rhs) noexcept
		{
			return lhs < rhs.val_;
		}

		TOML_NODISCARD
		friend bool operator<=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ <= rhs;
		}

		TOML_NODISCARD
		friend bool operator<=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs <= rhs.val_;
		}

		TOML_NODISCARD
		friend bool operator>(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ > rhs;
		}

		TOML_NODISCARD
		friend bool operator>(value_arg lhs, const value& rhs) noexcept
		{
			return lhs > rhs.val_;
		}

		TOML_NODISCARD
		friend bool operator>=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ >= rhs;
		}

		TOML_NODISCARD
		friend bool operator>=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs >= rhs.val_;
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs == rhs.val_; // calls asymmetrical value-equality operator defined above
			else
				return false;
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator!=(const value& lhs, const value<T>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator<(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ < rhs.val_;
			else
				return impl::node_type_of<value_type> < impl::node_type_of<T>;
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator<=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ <= rhs.val_;
			else
				return impl::node_type_of<value_type> <= impl::node_type_of<T>;
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator>(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ > rhs.val_;
			else
				return impl::node_type_of<value_type> > impl::node_type_of<T>;
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator>=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ >= rhs.val_;
			else
				return impl::node_type_of<value_type> >= impl::node_type_of<T>;
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const value& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};

	template <typename T>
	value(T) -> value<impl::native_type_of<impl::remove_cvref<T>>>;

	template <typename T>
	TOML_NODISCARD
	inline decltype(auto) node::get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(node_type_of<T> != node_type::none);
		static_assert(node_type_of<T> != node_type::table);
		static_assert(node_type_of<T> != node_type::array);
		static_assert(is_native<T> || can_represent_native<T>);
		static_assert(!is_cvref<T>);
		TOML_ASSERT(this->type() == node_type_of<T>);

		if constexpr (node_type_of<T> == node_type::string)
		{
			const auto& str = *ref_cast<std::string>();
			if constexpr (std::is_same_v<T, std::string>)
				return str;
			else if constexpr (std::is_same_v<T, std::string_view>)
				return T{ str };
			else if constexpr (std::is_same_v<T, const char*>)
				return str.c_str();

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return widen(str);
#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}

#if TOML_HAS_CHAR8

			// char -> char8_t (potentially unsafe - the feature is 'experimental'!)
			else if constexpr (is_one_of<T, std::u8string, std::u8string_view>)
				return T(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char8_t*>)
				return reinterpret_cast<const char8_t*>(str.c_str());
			else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");

#endif
		}
		else
			return static_cast<T>(*ref_cast<native_type_of<T>>());
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_exact() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

		static_assert((is_native<T> || can_represent_native<T>)&&!is_cvref<T>,
					  TOML_SA_VALUE_EXACT_FUNC_MESSAGE("return type of node::value_exact()"));

		// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
		if constexpr ((is_native<T> || can_represent_native<T>)&&!is_cvref<T>)
		{
			if (type() == node_type_of<T>)
				return { this->get_value_exact<T>() };
			else
				return {};
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");
		static_assert((is_native<T> || can_represent_native<T> || can_partially_represent_native<T>)&&!is_cvref<T>,
					  TOML_SA_VALUE_FUNC_MESSAGE("return type of node::value()"));

		// when asking for strings, dates, times and date_times there's no 'fuzzy' conversion
		// semantics to be mindful of so the exact retrieval is enough.
		if constexpr (is_natively_one_of<T, std::string, time, date, date_time>)
		{
			if (type() == node_type_of<T>)
				return { this->get_value_exact<T>() };
			else
				return {};
		}

		// everything else requires a bit of logicking.
		else
		{
			switch (type())
			{
				// int -> *
				case node_type::integer:
				{
					// int -> int
					if constexpr (is_natively_one_of<T, int64_t>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return static_cast<T>(*ref_cast<int64_t>());
						else
							return node_integer_cast<T>(*ref_cast<int64_t>());
					}

					// int -> float
					else if constexpr (is_natively_one_of<T, double>)
					{
						const int64_t val = *ref_cast<int64_t>();
						if constexpr (std::numeric_limits<T>::digits < 64)
						{
							constexpr auto largest_whole_float = (int64_t{ 1 } << std::numeric_limits<T>::digits);
							if (val < -largest_whole_float || val > largest_whole_float)
								return {};
						}
						return static_cast<T>(val);
					}

					// int -> bool
					else if constexpr (is_natively_one_of<T, bool>)
						return static_cast<bool>(*ref_cast<int64_t>());

					// int -> anything else
					else
						return {};
				}

				// float -> *
				case node_type::floating_point:
				{
					// float -> float
					if constexpr (is_natively_one_of<T, double>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return { static_cast<T>(*ref_cast<double>()) };
						else
						{
							const double val = *ref_cast<double>();
							if (impl::fpclassify(val) == fp_class::ok
								&& (val < (std::numeric_limits<T>::lowest)() || val > (std::numeric_limits<T>::max)()))
								return {};
							return { static_cast<T>(val) };
						}
					}

					// float -> int
					else if constexpr (is_natively_one_of<T, int64_t>)
					{
						const double val = *ref_cast<double>();
						if (impl::fpclassify(val) == fp_class::ok
							&& static_cast<double>(static_cast<int64_t>(val)) == val)
							return node_integer_cast<T>(static_cast<int64_t>(val));
						else
							return {};
					}

					// float -> anything else
					else
						return {};
				}

				// bool -> *
				case node_type::boolean:
				{
					// bool -> bool
					if constexpr (is_natively_one_of<T, bool>)
						return { *ref_cast<bool>() };

					// bool -> int
					else if constexpr (is_natively_one_of<T, int64_t>)
						return { static_cast<T>(*ref_cast<bool>()) };

					// bool -> anything else
					else
						return {};
				}
			}

			// non-values, or 'exact' types covered above
			return {};
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline auto node::value_or(T && default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_or() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

		if constexpr (is_wide_string<T>)
		{
#if TOML_ENABLE_WINDOWS_COMPAT

			if (type() == node_type::string)
				return widen(*ref_cast<std::string>());
			return std::wstring{ static_cast<T&&>(default_value) };

#else

			static_assert(dependent_false<T>, "Evaluated unreachable branch!");

#endif
		}
		else
		{
			using value_type =
				std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
								   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
								   std::decay_t<T>>;
			using traits = value_traits<value_type>;

			// clang-format off

			static_assert(
				traits::is_native || traits::can_represent_native || traits::can_partially_represent_native,
				"The default value type of node::value_or() must be one of:"
				TOML_SA_LIST_NEW "A native TOML value type"
				TOML_SA_NATIVE_VALUE_TYPE_LIST

				TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"
				TOML_SA_LIST_BEG "std::string"
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring"
				#endif
				TOML_SA_LIST_SEP "any signed integer type >= 64 bits"
				TOML_SA_LIST_SEP "any floating-point type >= 64 bits"
				TOML_SA_LIST_END

				TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"
				TOML_SA_LIST_BEG "any other integer type"
				TOML_SA_LIST_SEP "any floating-point type >= 32 bits"
				TOML_SA_LIST_END

				TOML_SA_LIST_NXT "A compatible view type"
				TOML_SA_LIST_BEG "std::string_view"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "std::u8string_view"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring_view"
				#endif
				TOML_SA_LIST_SEP "const char*"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "const char8_t*"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "const wchar_t*"
				#endif
				TOML_SA_LIST_END
			);

			// clang-format on

			// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
			if constexpr (traits::is_native || traits::can_represent_native || traits::can_partially_represent_native)
			{
				if constexpr (traits::is_native)
				{
					if (type() == node_type_of<value_type>)
						return *ref_cast<typename traits::native_type>();
				}
				if (auto val = this->value<value_type>())
					return *val;
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/make_node.h  ******************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(returns_nonnull)
	auto* make_node_impl_specialized(T && val, [[maybe_unused]] value_flags flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		static_assert(!std::is_same_v<unwrapped_type, node>);
		static_assert(!is_node_view<unwrapped_type>);

		// arrays + tables - invoke copy/move ctor
		if constexpr (is_one_of<unwrapped_type, array, table>)
		{
			return new unwrapped_type(static_cast<T&&>(val));
		}

		// values
		else
		{
			using native_type = native_type_of<unwrapped_type>;
			using value_type  = value<native_type>;

			value_type* out;

			// copy/move ctor
			if constexpr (std::is_same_v<remove_cvref<T>, value_type>)
			{
				out = new value_type{ static_cast<T&&>(val) };
			}

			// creating from raw value
			else
			{
				static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
							  "Instantiating values from wide-character strings is only "
							  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

				if constexpr (!is_losslessly_convertible_to_native<unwrapped_type>)
				{
					if constexpr (std::is_same_v<native_type, int64_t>)
						static_assert(dependent_false<T>,
									  "Integral value initializers must be losslessly convertible to int64_t");
					else if constexpr (std::is_same_v<native_type, double>)
						static_assert(dependent_false<T>,
									  "Floating-point value initializers must be losslessly convertible to double");
					else
						static_assert(
							dependent_false<T>,
							"Value initializers must be losslessly convertible to one of the TOML value types");
				}

				if constexpr (is_wide_string<T>)
				{
#if TOML_ENABLE_WINDOWS_COMPAT
					out = new value_type{ narrow(static_cast<T&&>(val)) };
#else
					static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
				}
				else
					out = new value_type{ static_cast<T&&>(val) };
			}

			if (flags != preserve_source_value_flags)
				out->flags(flags);

			return out;
		}
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(T && val, value_flags flags = preserve_source_value_flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		if constexpr (std::is_same_v<unwrapped_type, node> || is_node_view<unwrapped_type>)
		{
			if constexpr (is_node_view<unwrapped_type>)
			{
				if (!val)
					return static_cast<toml::node*>(nullptr);
			}

			return static_cast<T&&>(val).visit(
				[flags](auto&& concrete) {
					return static_cast<toml::node*>(
						make_node_impl_specialized(static_cast<decltype(concrete)&&>(concrete), flags));
				});
		}
		else
			return make_node_impl_specialized(static_cast<T&&>(val), flags);
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(inserter<T> && val, value_flags flags = preserve_source_value_flags)
	{
		return make_node_impl(static_cast<T&&>(val.value), flags);
	}

	template <typename T, bool = (is_node<T> || is_node_view<T> || is_value<T> || can_partially_represent_native<T>)>
	struct inserted_type_of_
	{
		using type = std::remove_pointer_t<decltype(make_node_impl(std::declval<T>()))>;
	};

	template <typename T>
	struct inserted_type_of_<inserter<T>, false>
	{
		using type = typename inserted_type_of_<T>::type;
	};

	template <typename T>
	struct inserted_type_of_<T, false>
	{
		using type = void;
	};

	template <typename T>
	TOML_NODISCARD
	node_ptr make_node(T && val, value_flags flags = preserve_source_value_flags)
	{
		return node_ptr{ make_node_impl(static_cast<T&&>(val), flags) };
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename T>
	using inserted_type_of = POXY_IMPLEMENTATION_DETAIL(typename impl::inserted_type_of_<impl::remove_cvref<T>>::type);
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/array.h  **********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	class TOML_TRIVIAL_ABI array_iterator
	{
	  private:
		template <bool>
		friend class array_iterator;

		using mutable_vector_iterator = std::vector<node_ptr>::iterator;
		using const_vector_iterator	  = std::vector<node_ptr>::const_iterator;
		using vector_iterator		  = std::conditional_t<IsConst, const_vector_iterator, mutable_vector_iterator>;

		mutable vector_iterator iter_;

	  public:
		using value_type		= std::conditional_t<IsConst, const node, node>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= ptrdiff_t;
		using iterator_category = typename std::iterator_traits<vector_iterator>::iterator_category;

		TOML_NODISCARD_CTOR
		array_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit array_iterator(mutable_vector_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit array_iterator(const_vector_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator<false>& other) noexcept //
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator&) noexcept = default;

		array_iterator& operator=(const array_iterator&) noexcept = default;

		array_iterator& operator++() noexcept // ++pre
		{
			++iter_;
			return *this;
		}

		array_iterator operator++(int) noexcept // post++
		{
			array_iterator out{ iter_ };
			++iter_;
			return out;
		}

		array_iterator& operator--() noexcept // --pre
		{
			--iter_;
			return *this;
		}

		array_iterator operator--(int) noexcept // post--
		{
			array_iterator out{ iter_ };
			--iter_;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const vector_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const const_vector_iterator() const noexcept
		{
			return iter_;
		}

		array_iterator& operator+=(ptrdiff_t rhs) noexcept
		{
			iter_ += rhs;
			return *this;
		}

		array_iterator& operator-=(ptrdiff_t rhs) noexcept
		{
			iter_ -= rhs;
			return *this;
		}

		TOML_NODISCARD
		friend array_iterator operator+(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ + rhs };
		}

		TOML_NODISCARD
		friend array_iterator operator+(ptrdiff_t lhs, const array_iterator& rhs) noexcept
		{
			return array_iterator{ rhs.iter_ + lhs };
		}

		TOML_NODISCARD
		friend array_iterator operator-(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ - rhs };
		}

		TOML_PURE_INLINE_GETTER
		friend ptrdiff_t operator-(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ - rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ < rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ <= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ > rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ >= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		reference operator[](ptrdiff_t idx) const noexcept
		{
			return *(iter_ + idx)->get();
		}
	};

	struct array_init_elem
	{
		mutable node_ptr value;

		template <typename T>
		TOML_NODISCARD_CTOR
		array_init_elem(T&& val, value_flags flags = preserve_source_value_flags) //
			: value{ make_node(static_cast<T&&>(val), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	using array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<false>);

	using const_array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<true>);

	class TOML_EXPORTED_CLASS array : public node
	{
	  private:

		using vector_type			= std::vector<impl::node_ptr>;
		using vector_iterator		= typename vector_type::iterator;
		using const_vector_iterator = typename vector_type::const_iterator;
		vector_type elems_;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const impl::array_init_elem*, const impl::array_init_elem*);

		TOML_NODISCARD_CTOR
		array(std::false_type, std::initializer_list<impl::array_init_elem> elems) //
			: array{ elems.begin(), elems.end() }
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		void preinsertion_resize(size_t idx, size_t count);

		TOML_EXPORTED_MEMBER_FUNCTION
		void insert_at_back(impl::node_ptr&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		vector_iterator insert_at(const_vector_iterator, impl::node_ptr&&);

		template <typename T>
		void emplace_back_if_not_empty_view(T&& val, value_flags flags)
		{
			if constexpr (is_node_view<T>)
			{
				if (!val)
					return;
			}
			insert_at_back(impl::make_node(static_cast<T&&>(val), flags));
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		size_t total_leaf_count() const noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void flatten_child(array&& child, size_t& dest_index) noexcept;

	  public:
		using value_type	  = node;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using reference		  = node&;
		using const_reference = const node&;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		~array() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const array&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(array&& other) noexcept;

		TOML_CONSTRAINED_TEMPLATE((sizeof...(ElemTypes) > 0 || !std::is_same_v<impl::remove_cvref<ElemType>, array>),
								  typename ElemType,
								  typename... ElemTypes)
		TOML_NODISCARD_CTOR
		explicit array(ElemType&& val, ElemTypes&&... vals)
			: array{ std::false_type{},
					 std::initializer_list<impl::array_init_elem>{ static_cast<ElemType&&>(val),
																   static_cast<ElemTypes&&>(vals)... } }
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(const array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(array&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::array;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<ElemType>>;
			static_assert(std::is_void_v<unwrapped_type> //
							  || (impl::is_native<unwrapped_type> || impl::is_one_of<unwrapped_type, table, array>),
						  "The template type argument of array::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<unwrapped_type>);
		}
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return true;
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return is_homogeneous(node_type::table);
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		TOML_PURE_INLINE_GETTER
		node* get(size_t index) noexcept
		{
			return index < elems_.size() ? elems_[index].get() : nullptr;
		}

		TOML_PURE_INLINE_GETTER
		const node* get(size_t index) const noexcept
		{
			return const_cast<array&>(*this).get(index);
		}

		template <typename ElemType>
		TOML_NODISCARD
		impl::wrap_node<ElemType>* get_as(size_t index) noexcept
		{
			if (auto val = get(index))
				return val->template as<ElemType>();
			return nullptr;
		}

		template <typename ElemType>
		TOML_NODISCARD
		const impl::wrap_node<ElemType>* get_as(size_t index) const noexcept
		{
			return const_cast<array&>(*this).template get_as<ElemType>(index);
		}

		TOML_NODISCARD
		node& operator[](size_t index) noexcept
		{
			return *elems_[index];
		}

		TOML_NODISCARD
		const node& operator[](size_t index) const noexcept
		{
			return *elems_[index];
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(size_t index);

		TOML_NODISCARD
		const node& at(size_t index) const
		{
			return const_cast<array&>(*this).at(index);
		}

		TOML_NODISCARD
		node& front() noexcept
		{
			return *elems_.front();
		}

		TOML_NODISCARD
		const node& front() const noexcept
		{
			return *elems_.front();
		}

		TOML_NODISCARD
		node& back() noexcept
		{
			return *elems_.back();
		}

		TOML_NODISCARD
		const node& back() const noexcept
		{
			return *elems_.back();
		}

		using iterator = array_iterator;

		using const_iterator = const_array_iterator;

		TOML_NODISCARD
		iterator begin() noexcept
		{
			return iterator{ elems_.begin() };
		}

		TOML_NODISCARD
		const_iterator begin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_NODISCARD
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_NODISCARD
		iterator end() noexcept
		{
			return iterator{ elems_.end() };
		}

		TOML_NODISCARD
		const_iterator end() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

		TOML_NODISCARD
		const_iterator cend() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

		TOML_NODISCARD
		bool empty() const noexcept
		{
			return elems_.empty();
		}

		TOML_NODISCARD
		size_t size() const noexcept
		{
			return elems_.size();
		}

		TOML_NODISCARD
		size_t max_size() const noexcept
		{
			return elems_.max_size();
		}

		TOML_NODISCARD
		size_t capacity() const noexcept
		{
			return elems_.capacity();
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void reserve(size_t new_capacity);

		TOML_EXPORTED_MEMBER_FUNCTION
		void shrink_to_fit();

		TOML_EXPORTED_MEMBER_FUNCTION
		void truncate(size_t new_size);

		template <typename ElemType>
		void resize(size_t new_size,
					ElemType&& default_init_val,
					value_flags default_init_flags = preserve_source_value_flags)
		{
			static_assert(!is_node_view<ElemType>,
						  "The default element type argument to toml::array::resize may not be toml::node_view.");

			if (!new_size)
				clear();
			else if (new_size > elems_.size())
				insert(cend(), new_size - elems_.size(), static_cast<ElemType&&>(default_init_val), default_init_flags);
			else
				truncate(new_size);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator pos) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator first, const_iterator last) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		array& flatten() &;

		array&& flatten() &&
		{
			return static_cast<toml::array&&>(this->flatten());
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& prune(bool recursive = true) & noexcept;

		array&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::array&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void pop_back() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		template <typename ElemType>
		iterator insert(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}
			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::make_node(static_cast<ElemType&&>(val), flags)) };
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						size_t count,
						ElemType&& val,
						value_flags flags = preserve_source_value_flags)
		{
			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}
			switch (count)
			{
				case 0: return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
				case 1: return insert(pos, static_cast<ElemType&&>(val), flags);
				default:
				{
					const auto start_idx = static_cast<size_t>(const_vector_iterator{ pos } - elems_.cbegin());
					preinsertion_resize(start_idx, count);
					size_t i = start_idx;
					for (size_t e = start_idx + count - 1u; i < e; i++)
						elems_[i] = impl::make_node(val, flags);

					elems_[i] = impl::make_node(static_cast<ElemType&&>(val), flags);
					return iterator{ elems_.begin() + static_cast<ptrdiff_t>(start_idx) };
				}
			}
		}

		template <typename Iter>
		iterator insert(const_iterator pos, Iter first, Iter last, value_flags flags = preserve_source_value_flags)
		{
			const auto distance = std::distance(first, last);
			if (distance <= 0)
				return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
			else
			{
				auto count		 = distance;
				using deref_type = decltype(*first);
				if constexpr (is_node_view<deref_type>)
				{
					for (auto it = first; it != last; it++)
						if (!(*it))
							count--;
					if (!count)
						return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
				}
				const auto start_idx = static_cast<size_t>(const_vector_iterator{ pos } - elems_.cbegin());
				preinsertion_resize(start_idx, static_cast<size_t>(count));
				size_t i = start_idx;
				for (auto it = first; it != last; it++)
				{
					if constexpr (is_node_view<deref_type>)
					{
						if (!(*it))
							continue;
					}
					if constexpr (std::is_rvalue_reference_v<deref_type>)
						elems_[i++] = impl::make_node(std::move(*it), flags);
					else
						elems_[i++] = impl::make_node(*it, flags);
				}
				return iterator{ elems_.begin() + static_cast<ptrdiff_t>(start_idx) };
			}
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						std::initializer_list<ElemType> ilist,
						value_flags flags = preserve_source_value_flags)
		{
			return insert(pos, ilist.begin(), ilist.end(), flags);
		}

		template <typename ElemType, typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			using type = impl::unwrap_node<ElemType>;
			static_assert((impl::is_native<type> || impl::is_one_of<type, table, array>)&&!impl::is_cvref<type>,
						  "Emplacement type parameter must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::node_ptr{ new impl::wrap_node<type>{ static_cast<Args&&>(args)... } }) };
		}

		template <typename ElemType>
		iterator replace(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			TOML_ASSERT(pos >= cbegin() && pos < cend());

			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}

			const auto it = elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin());
			*it			  = impl::make_node(static_cast<ElemType&&>(val), flags);
			return iterator{ it };
		}

		template <typename ElemType>
		void push_back(ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			emplace_back_if_not_empty_view(static_cast<ElemType&&>(val), flags);
		}

		template <typename ElemType, typename... ElemArgs>
		decltype(auto) emplace_back(ElemArgs&&... args)
		{
			static_assert(!impl::is_cvref<ElemType>, "ElemType may not be const, volatile, or a reference.");

			static constexpr auto moving_node_ptr = std::is_same_v<ElemType, impl::node_ptr> //
												 && sizeof...(ElemArgs) == 1u				 //
												 && impl::first_is_same<impl::node_ptr&&, ElemArgs&&...>;

			using unwrapped_type = impl::unwrap_node<ElemType>;

			static_assert(
				moving_node_ptr										  //
					|| impl::is_native<unwrapped_type>				  //
					|| impl::is_one_of<unwrapped_type, table, array>, //
				"ElemType argument of array::emplace_back() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (moving_node_ptr)
			{
				insert_at_back(static_cast<ElemArgs&&>(args)...);
				return *elems_.back();
			}
			else
			{
				auto ptr = new impl::wrap_node<unwrapped_type>{ static_cast<ElemArgs&&>(args)... };
				insert_at_back(impl::node_ptr{ ptr });
				return *ptr;
			}
		}

	  private:

		TOML_NODISCARD
		TOML_EXPORTED_STATIC_FUNCTION
		static bool equal(const array&, const array&) noexcept;

		template <typename T>
		TOML_NODISCARD
		static bool equal_to_container(const array& lhs, const T& rhs) noexcept
		{
			using element_type = std::remove_const_t<typename T::value_type>;
			static_assert(impl::is_losslessly_convertible_to_native<element_type>,
						  "Container element type must be losslessly convertible one of the native TOML value types");

			if (lhs.size() != rhs.size())
				return false;
			if (rhs.size() == 0u)
				return true;

			size_t i{};
			for (auto& list_elem : rhs)
			{
				const auto elem = lhs.get_as<impl::native_type_of<element_type>>(i++);
				if (!elem || *elem != list_elem)
					return false;
			}

			return true;
		}

	  public:

		TOML_NODISCARD
		friend bool operator==(const array& lhs, const array& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const array& lhs, const array& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, const std::initializer_list<T>& rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::initializer_list<T>&, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, const std::vector<T>& rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>);

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const array& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/key.h  ************************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class key
	{
	  private:
		std::string key_;
		source_region source_;

	  public:

		TOML_NODISCARD_CTOR
		key() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, source_region&& src = {}) //
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, const source_region& src) //
			: key_{ k },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, source_region&& src = {}) noexcept //
			: key_{ std::move(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, const source_region& src) noexcept //
			: key_{ std::move(k) },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, source_region&& src = {}) //
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, const source_region& src) //
			: key_{ k },
			  source_{ src }
		{}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, source_region&& src = {}) //
			: key_{ impl::narrow(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, const source_region& src) //
			: key_{ impl::narrow(k) },
			  source_{ src }
		{}

#endif

		TOML_PURE_INLINE_GETTER
		std::string_view str() const noexcept
		{
			return std::string_view{ key_ };
		}

		TOML_PURE_INLINE_GETTER
		/*implicit*/ operator std::string_view() const noexcept
		{
			return str();
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return key_.empty();
		}

		TOML_PURE_INLINE_GETTER
		const char* data() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		size_t length() const noexcept
		{
			return key_.length();
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ >= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ == rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ != rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ < rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ <= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ > rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ >= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs >= rhs.key_;
		}

		using const_iterator = const char*;

		using iterator = const_iterator;

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return key_.data() + key_.length();
		}

		friend std::ostream& operator<<(std::ostream& lhs, const key& rhs)
		{
			impl::print_to_stream(lhs, rhs.key_);
			return lhs;
		}
	};

	template <typename T>
	inline constexpr bool is_key = std::is_same_v<impl::remove_cvref<T>, toml::key>;

	template <typename T>
	inline constexpr bool is_key_or_convertible =
		is_key<T> || std::is_constructible_v<toml::key, T> || std::is_convertible_v<T, toml::key>;
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/table.h  **********************************************************************************************

TOML_DISABLE_WARNINGS;
#include <map>
#include <iterator>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	struct table_proxy_pair
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const toml::key& first;
		value_type& second;
	};

	template <bool IsConst>
	class table_iterator
	{
	  private:
		template <bool>
		friend class table_iterator;

		using proxy_type		   = table_proxy_pair<IsConst>;
		using mutable_map_iterator = std::map<toml::key, node_ptr, std::less<>>::iterator;
		using const_map_iterator   = std::map<toml::key, node_ptr, std::less<>>::const_iterator;
		using map_iterator		   = std::conditional_t<IsConst, const_map_iterator, mutable_map_iterator>;

		mutable map_iterator iter_;
		mutable std::aligned_storage_t<sizeof(proxy_type), alignof(proxy_type)> proxy_;
		mutable bool proxy_instantiated_ = false;

		TOML_NODISCARD
		proxy_type* get_proxy() const noexcept
		{
			if (!proxy_instantiated_)
			{
				auto p = ::new (static_cast<void*>(&proxy_)) proxy_type{ iter_->first, *iter_->second.get() };
				proxy_instantiated_ = true;
				return p;
			}
			else
				return TOML_LAUNDER(reinterpret_cast<proxy_type*>(&proxy_));
		}

	  public:
		TOML_NODISCARD_CTOR
		table_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit table_iterator(mutable_map_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit table_iterator(const_map_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator<false>& other) noexcept //
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator& other) noexcept //
			: iter_{ other.iter_ }
		{}

		table_iterator& operator=(const table_iterator& rhs) noexcept
		{
			iter_				= rhs.iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		using value_type		= table_proxy_pair<IsConst>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= typename std::iterator_traits<map_iterator>::difference_type;
		using iterator_category = typename std::iterator_traits<map_iterator>::iterator_category;

		table_iterator& operator++() noexcept // ++pre
		{
			++iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		table_iterator operator++(int) noexcept // post++
		{
			table_iterator out{ iter_ };
			++iter_;
			proxy_instantiated_ = false;
			return out;
		}

		table_iterator& operator--() noexcept // --pre
		{
			--iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		table_iterator operator--(int) noexcept // post--
		{
			table_iterator out{ iter_ };
			--iter_;
			proxy_instantiated_ = false;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const map_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const const_map_iterator() const noexcept
		{
			return iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}
	};

	struct table_init_pair
	{
		mutable toml::key key;
		mutable node_ptr value;

		template <typename K, typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(K&& k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ static_cast<K&&>(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	using table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<false>);

	using const_table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<true>);

	class TOML_EXPORTED_CLASS table : public node
	{
	  private:

		using map_type			 = std::map<toml::key, impl::node_ptr, std::less<>>;
		using map_iterator		 = typename map_type::iterator;
		using const_map_iterator = typename map_type::const_iterator;
		map_type map_;

		bool inline_ = false;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const impl::table_init_pair*, const impl::table_init_pair*);

	  public:

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		~table() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const table&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(table&& other) noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit table(std::initializer_list<impl::table_init_pair> kvps) //
			: table(kvps.begin(), kvps.end())
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(const table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(table&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::table;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::unwrap_node<ElemType>;

			static_assert(
				std::is_void_v<
					type> || ((impl::is_native<type> || impl::is_one_of<type, table, array>)&&!impl::is_cvref<type>),
				"The template type argument of table::is_homogeneous() must be void or one "
				"of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		TOML_PURE_INLINE_GETTER
		bool is_inline() const noexcept
		{
			return inline_;
		}

		void is_inline(bool val) noexcept
		{
			inline_ = val;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		node* get(std::string_view key) noexcept;

		TOML_PURE_INLINE_GETTER
		const node* get(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).get(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node* get(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get(impl::narrow(key));
		}

		TOML_NODISCARD
		const node* get(std::wstring_view key) const
		{
			return const_cast<table&>(*this).get(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		template <typename T>
		TOML_PURE_GETTER
		impl::wrap_node<T>* get_as(std::string_view key) noexcept
		{
			const auto n = this->get(key);
			return n ? n->template as<T>() : nullptr;
		}

		template <typename T>
		TOML_PURE_GETTER
		const impl::wrap_node<T>* get_as(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		template <typename T>
		TOML_NODISCARD
		impl::wrap_node<T>* get_as(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get_as<T>(impl::narrow(key));
		}

		template <typename T>
		TOML_NODISCARD
		const impl::wrap_node<T>* get_as(std::wstring_view key) const
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(std::string_view key);

		TOML_NODISCARD
		const node& at(std::string_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node& at(std::wstring_view key)
		{
			return at(impl::narrow(key));
		}

		TOML_NODISCARD
		const node& at(std::wstring_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		using iterator = toml::table_iterator;

		using const_iterator = toml::const_table_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return iterator{ map_.begin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return iterator{ map_.end() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return map_.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return map_.size();
		}

	  private:

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator get_lower_bound(std::string_view) noexcept;

	  public:

		TOML_PURE_GETTER
		iterator lower_bound(std::string_view key) noexcept
		{
			return iterator{ get_lower_bound(key) };
		}

		TOML_PURE_GETTER
		const_iterator lower_bound(std::string_view key) const noexcept
		{
			return const_iterator{ const_cast<table&>(*this).get_lower_bound(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator lower_bound(std::wstring_view key)
		{
			if (empty())
				return end();

			return lower_bound(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator lower_bound(std::wstring_view key) const
		{
			if (empty())
				return end();

			return lower_bound(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		iterator find(std::string_view key) noexcept;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		const_iterator find(std::string_view key) const noexcept;

		TOML_PURE_GETTER
		bool contains(std::string_view key) const noexcept
		{
			return get(key) != nullptr;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator find(std::wstring_view key)
		{
			if (empty())
				return end();

			return find(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator find(std::wstring_view key) const
		{
			return find(impl::narrow(key));
		}

		TOML_NODISCARD
		bool contains(std::wstring_view key) const
		{
			return contains(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	  private:

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator, const_map_iterator) noexcept;

	  public:

		iterator erase(iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator begin, const_iterator end) noexcept
		{
			return iterator{ erase(const_map_iterator{ begin }, const_map_iterator{ end }) };
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		size_t erase(std::string_view key) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		size_t erase(std::wstring_view key)
		{
			if (empty())
				return false;

			return erase(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		table& prune(bool recursive = true) & noexcept;

		table&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::table&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

	  private:

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator insert_with_hint(const_iterator, key&&, impl::node_ptr&&);

	  public:

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType,
								  typename KeyType,
								  typename... ValueArgs)
		iterator emplace_hint(const_iterator hint, KeyType&& key, ValueArgs&&... args)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Emplacement using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			static_assert(!impl::is_cvref<ValueType>, "ValueType may not be const, volatile, or a reference.");

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return emplace_hint<ValueType>(hint,
											   impl::narrow(static_cast<KeyType&&>(key)),
											   static_cast<ValueArgs&&>(args)...);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				static constexpr auto moving_node_ptr = std::is_same_v<ValueType, impl::node_ptr> //
													 && sizeof...(ValueArgs) == 1u				  //
													 && impl::first_is_same<impl::node_ptr&&, ValueArgs&&...>;
				using unwrapped_type = impl::unwrap_node<ValueType>;

				static_assert(moving_node_ptr										//
								  || impl::is_native<unwrapped_type>				//
								  || impl::is_one_of<unwrapped_type, table, array>, //
							  "ValueType argument of table::emplace_hint() must be one "
							  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

				map_iterator ipos = insert_with_hint(hint, toml::key{ static_cast<KeyType&&>(key) }, nullptr);

				// if second is nullptr then we successully claimed the key and inserted the empty sentinel,
				// so now we have to construct the actual value
				if (!ipos->second)
				{
					if constexpr (moving_node_ptr)
						ipos->second = std::move(static_cast<ValueArgs&&>(args)...);
					else
					{
#if TOML_COMPILER_EXCEPTIONS
						try
						{
#endif
							ipos->second.reset(
								new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... });
#if TOML_COMPILER_EXCEPTIONS
						}
						catch (...)
						{
							erase(const_map_iterator{ ipos }); // strong exception guarantee
							throw;
						}
#endif
					}
				}
				return iterator{ ipos };
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert(KeyType&& key,
										 ValueType&& val,
										 value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert(impl::narrow(static_cast<KeyType&&>(key)), static_cast<ValueType&&>(val), flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto key_view = std::string_view{ key };
				map_iterator ipos	= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(const_iterator{ ipos },
											toml::key{ static_cast<KeyType&&>(key) },
											impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		TOML_CONSTRAINED_TEMPLATE((!is_key_or_convertible<Iter> && !impl::is_wide_string<Iter>), typename Iter)
		void insert(Iter begin, Iter end, value_flags flags = preserve_source_value_flags)
		{
			if (begin == end)
				return;
			for (auto it = begin; it != end; it++)
			{
				if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
					insert(std::move((*it).first), std::move((*it).second), flags);
				else
					insert((*it).first, (*it).second, flags);
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert_or_assign(KeyType&& key,
												   ValueType&& val,
												   value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert_or_assign(impl::narrow(static_cast<KeyType&&>(key)),
										static_cast<ValueType&&>(val),
										flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto key_view = std::string_view{ key };
				map_iterator ipos	= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(const_iterator{ ipos },
											toml::key{ static_cast<KeyType&&>(key) },
											impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				else
				{
					(*ipos).second = impl::make_node(static_cast<ValueType&&>(val), flags);
					return { iterator{ ipos }, false };
				}
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType,
								  typename KeyType,
								  typename... ValueArgs)
		std::pair<iterator, bool> emplace(KeyType&& key, ValueArgs&&... args)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Emplacement using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			static_assert(!impl::is_cvref<ValueType>, "ValueType may not be const, volatile, or a reference.");

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return emplace<ValueType>(impl::narrow(static_cast<KeyType&&>(key)), static_cast<ValueArgs&&>(args)...);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				using unwrapped_type = impl::unwrap_node<ValueType>;
				static_assert((impl::is_native<unwrapped_type> || impl::is_one_of<unwrapped_type, table, array>),
							  "ValueType argument of table::emplace() must be one "
							  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

				const auto key_view = std::string_view{ key };
				auto ipos			= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(
						const_iterator{ ipos },
						toml::key{ static_cast<KeyType&&>(key) },
						impl::node_ptr{ new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... } });
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	  private:

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool equal(const table&, const table&) noexcept;

	  public:

		TOML_NODISCARD
		friend bool operator==(const table& lhs, const table& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const table& lhs, const table& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const table& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/unicode_autogenerated.h  ******************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#if TOML_GCC && TOML_GCC < 9
#pragma GCC push_options
#pragma GCC optimize("O1") // codegen bugs
#endif

// the functions in this namespace block are automatically generated by a tool - they are not meant to be hand-edited

TOML_IMPL_NAMESPACE_START
{
	TOML_CONST_GETTER
	constexpr bool is_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		return c == U'\t' || c == U' ';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		// 20 code units from 8 ranges (spanning a search area of 65120)

		if (c < U'\xA0' || c > U'\uFEFF')
			return false;

		const auto child_index_0 = (static_cast<uint_least64_t>(c) - 0xA0ull) / 0x3FAull;
		if ((1ull << child_index_0) & 0x7FFFFFFFFFFFF75Eull)
			return false;
		if (c == U'\xA0' || c == U'\u3000' || c == U'\uFEFF')
			return true;
		switch (child_index_0)
		{
			case 0x05: return c == U'\u1680' || c == U'\u180E';
			case 0x07:
				return (U'\u2000' <= c && c <= U'\u200B') || (U'\u205F' <= c && c <= U'\u2060') || c == U'\u202F';
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_CONST_GETTER
	constexpr bool is_horizontal_whitespace(char32_t c) noexcept
	{
		return is_ascii_horizontal_whitespace(c) || is_non_ascii_horizontal_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return c >= U'\n' && c <= U'\r';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return (U'\u2028' <= c && c <= U'\u2029') || c == U'\x85';
	}

	TOML_CONST_GETTER
	constexpr bool is_vertical_whitespace(char32_t c) noexcept
	{
		return is_ascii_vertical_whitespace(c) || is_non_ascii_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_whitespace(char32_t c) noexcept
	{
		return is_horizontal_whitespace(c) || is_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_bare_key_character(char32_t c) noexcept
	{
#if TOML_LANG_UNRELEASED // toml/issues/644 ('+' in bare keys)
		if (c == U'+')
			return true;
#endif
		// 64 code units from 5 ranges (spanning a search area of 78)

		if (c < U'-' || c > U'z')
			return false;

		return (((static_cast<uint_least64_t>(c) - 0x2Dull) / 0x40ull) != 0ull)
			|| ((1ull << (static_cast<uint_least64_t>(c) - 0x2Dull)) & 0xFFF43FFFFFF01FF9ull);
	}

#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_letter(char32_t c) noexcept
	{
		// 131704 code units from 646 ranges (spanning a search area of 201377)

		if (c < U'\xAA' || c > U'\U0003134A')
			return false;

		const auto child_index_0 = (static_cast<uint_least64_t>(c) - 0xAAull) / 0xC4Bull;
		if ((1ull << child_index_0) & 0x26180C0000ull)
			return false;
		if ((1ull << child_index_0) & 0x8A7FFC004001DFA0ull)
			return true;
		switch (child_index_0)
		{
			case 0x00: // [0] 00AA - 0CF4
			{
				// 1957 code units from 126 ranges (spanning a search area of 3145)

				if (c > U'\u0CF2')
					return false;
				TOML_ASSUME(c >= U'\xAA');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFDFFFFFC10801u, 0xFFFFFFFFFFFFDFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x07C000FFF0FFFFFFu, 0x0000000000000014u, 0x0000000000000000u, 0xFEFFFFF5D02F37C0u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFEFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFF00FFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFC09FFFFFFFFFBFu, 0x000000007FFFFFFFu,
					0xFFFFFFC000000000u, 0xFFC00000000001E1u, 0x00000001FFFFFFFFu, 0xFFFFFFFFFFFFFFB0u,
					0x18000BFFFFFFFFFFu, 0xFFFFFF4000270030u, 0xFFFFFFF80000003Fu, 0x0FFFFFFFFFFFFFFFu,
					0xFFFFFFFF00000080u, 0x44010FFFFFC10C01u, 0xFFC07FFFFFC00000u, 0xFFC0001FBFFFFFC1u,
					0x00000000FFFFFFFFu, 0xFFFFFFFFFC000000u, 0x00FFC0400008FFFFu, 0x7FFFFE67F87FFF80u,
					0x00EC00100008F17Fu, 0x7FFFFE61F80400C0u, 0x001780000000DB7Fu, 0x7FFFFEEFF8000700u,
					0x00C000400008FB7Fu, 0x7FFFFE67F8008000u, 0x00EC00000008FB7Fu, 0xC6358F71FA000080u,
					0x000000400000FFF1u, 0x7FFFFF77F8000000u, 0x00C9C0000008FFFFu, 0x7FFFFF77F8400000u,
					0x00D800000008FBFFu, 0x0000000000000180u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xAAull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xAAull) % 0x40ull));
			}
			case 0x01: // [1] 0CF5 - 193F
			{
				// 2241 code units from 82 ranges (spanning a search area of 3099)

				if (c < U'\u0D04' || c > U'\u191E')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x027FFFFFFFFFDDFFu, 0x0FC0000038070400u, 0xF2FFBFFFFFC7FFFEu, 0xE000000000000007u,
					0xF000DFFFFFFFFFFFu, 0x6000000000000007u, 0xF200DFFAFFFFFF7Du, 0x100000000F000005u,
					0xF000000000000000u, 0x000001FFFFFFFFEFu, 0x00000000000001F0u, 0xF000000000000000u,
					0x0800007FFFFFFFFFu, 0x3FFE1C0623C3F000u, 0xFFFFFFFFF0000400u, 0xFF7FFFFFFFFFF20Bu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFF3D7F3DFu, 0xD7F3DFFFFFFFF3DFu, 0xFFFFFFFFFFF7FFF3u,
					0xFFFFFFFFFFF3DFFFu, 0xF0000000007FFFFFu, 0xFFFFFFFFF0000FFFu, 0xE3F3FFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xEFFFF9FFFFFFFFFFu, 0xFFFFFFFFF07FFFFFu, 0xF01FE07FFFFFFFFFu,
					0xF0003FFFF8003FFFu, 0xF0001DFFF0003FFFu, 0x0000FFFFFFFFFFFFu, 0x0000000001080000u,
					0xFFFFFFFFF0000000u, 0xF01FFFFFFFFFFFFFu, 0xFFFFF05FFFFFFFF9u, 0xF003FFFFFFFFFFFFu,
					0x0000000007FFFFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xD04ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xD04ull) % 0x40ull));
			}
			case 0x02: // [2] 1940 - 258A
			{
				// 1185 code units from 59 ranges (spanning a search area of 2101)

				if (c < U'\u1950' || c > U'\u2184')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFF001F3FFFFFFFu, 0x03FFFFFF0FFFFFFFu, 0xFFFF000000000000u, 0xFFFFFFFFFFFF007Fu,
					0x000000000000001Fu, 0x0000000000800000u, 0xFFE0000000000000u, 0x1FE0000FFFFFFFFFu,
					0xFFF8000000000000u, 0xFFFFFC00C001FFFFu, 0xFFFF0000003FFFFFu, 0xE0000000000FFFFFu,
					0x01FF3FFFFFFFFC00u, 0x0000E7FFFFFFFFFFu, 0xFFFF046FDE000000u, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0x0000FFFFFFFFFFFFu, 0xFFFF000000000000u, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x3F3FFFFFFFFF3F3Fu,
					0xFFFF3FFFFFFFAAFFu, 0x1FDC5FDFFFFFFFFFu, 0x00001FDC1FFF0FCFu, 0x0000000000000000u,
					0x0000800200000000u, 0x0000000000001FFFu, 0xFC84000000000000u, 0x43E0F3FFBD503E2Fu,
					0x0018000000000000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x1950ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x1950ull) % 0x40ull));
			}
			case 0x03: // [3] 258B - 31D5
			{
				// 773 code units from 28 ranges (spanning a search area of 1472)

				if (c < U'\u2C00' || c > U'\u31BF')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x000C781FFFFFFFFFu,
					0xFFFF20BFFFFFFFFFu, 0x000080FFFFFFFFFFu, 0x7F7F7F7F007FFFFFu, 0x000000007F7F7F7Fu,
					0x0000800000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x183E000000000060u, 0xFFFFFFFFFFFFFFFEu, 0xFFFFFFFEE07FFFFFu, 0xF7FFFFFFFFFFFFFFu,
					0xFFFEFFFFFFFFFFE0u, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFF00007FFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x2C00ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x04: return (U'\u31F0' <= c && c <= U'\u31FF') || U'\u3400' <= c;
			case 0x06: return c <= U'\u4DBF' || U'\u4E00' <= c;
			case 0x0D: // [13] A079 - ACC3
			{
				// 2567 code units from 54 ranges (spanning a search area of 3147)

				TOML_ASSUME(c >= U'\uA079' && c <= U'\uACC3');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x00000000000FFFFFu, 0xFFFFFFFFFF800000u, 0xFFFFFFFFFFFFFF9Fu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x0006007FFF8FFFFFu, 0x003FFFFFFFFFFF80u,
					0xFFFFFF9FFFFFFFC0u, 0x00001FFFFFFFFFFFu, 0xFFFFFE7FC0000000u, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFCFFFFu, 0xFE000001F583FFFFu, 0x000003FFFFFBDDFFu, 0x07FFFFFFFFFFFF80u,
					0x07FFFFFFFFFFFE00u, 0x7E00000000000000u, 0xFF801FFFFFFE0034u, 0xFFFFFF8000003FFFu,
					0x03FFFFFFFFFFF80Fu, 0x007FEF8000400000u, 0x0000FFFFFFFFFFBEu, 0x3FFFFF800007FB80u,
					0x317FFFFFFFFFFFE2u, 0x0E03FF9C0000029Fu, 0xFFBFBF803F3F3F00u, 0xFF81FFFBFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0x000003FFFFFFFFFFu, 0xFFFFFFFFFFFFFF80u, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0x00000000000007FFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xA079ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xA079ull) % 0x40ull));
			}
			case 0x11:
				return c <= U'\uD7A3' || (U'\uD7B0' <= c && c <= U'\uD7C6') || (U'\uD7CB' <= c && c <= U'\uD7FB');
			case 0x14: // [20] F686 - 102D0
			{
				// 1710 code units from 34 ranges (spanning a search area of 2513)

				if (c < U'\uF900')
					return false;
				TOML_ASSUME(c <= U'\U000102D0');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFF3FFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x0000000003FFFFFFu,
					0x5F7FFDFFA0F8007Fu, 0xFFFFFFFFFFFFFFDBu, 0x0003FFFFFFFFFFFFu, 0xFFFFFFFFFFF80000u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x3FFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFF0000u, 0xFFFFFFFFFFFCFFFFu, 0x0FFF0000000000FFu,
					0x0000000000000000u, 0xFFDF000000000000u, 0xFFFFFFFFFFFFFFFFu, 0x1FFFFFFFFFFFFFFFu,
					0x07FFFFFE00000000u, 0xFFFFFFC007FFFFFEu, 0x7FFFFFFFFFFFFFFFu, 0x000000001CFCFCFCu,
					0xB7FFFF7FFFFFEFFFu, 0x000000003FFF3FFFu, 0xFFFFFFFFFFFFFFFFu, 0x07FFFFFFFFFFFFFFu,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0xFFFFFFFF1FFFFFFFu, 0x000000000001FFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xF900ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x15: // [21] 102D1 - 10F1B
			{
				// 1747 code units from 59 ranges (spanning a search area of 3100)

				if (c < U'\U00010300')
					return false;
				TOML_ASSUME(c <= U'\U00010F1B');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFE000FFFFFFFFu, 0x003FFFFFFFFF03FDu, 0xFFFFFFFF3FFFFFFFu, 0x000000000000FF0Fu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFF00003FFFFFFFu, 0x0FFFFFFFFF0FFFFFu,
					0xFFFF00FFFFFFFFFFu, 0xF7FF000FFFFFFFFFu, 0x1BFBFFFBFFB7F7FFu, 0x0000000000000000u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x007FFFFFFFFFFFFFu, 0x000000FF003FFFFFu, 0x07FDFFFFFFFFFFBFu, 0x0000000000000000u,
					0x91BFFFFFFFFFFD3Fu, 0x007FFFFF003FFFFFu, 0x000000007FFFFFFFu, 0x0037FFFF00000000u,
					0x03FFFFFF003FFFFFu, 0x0000000000000000u, 0xC0FFFFFFFFFFFFFFu, 0x0000000000000000u,
					0x003FFFFFFEEF0001u, 0x1FFFFFFF00000000u, 0x000000001FFFFFFFu, 0x0000001FFFFFFEFFu,
					0x003FFFFFFFFFFFFFu, 0x0007FFFF003FFFFFu, 0x000000000003FFFFu, 0x0000000000000000u,
					0xFFFFFFFFFFFFFFFFu, 0x00000000000001FFu, 0x0007FFFFFFFFFFFFu, 0x0007FFFFFFFFFFFFu,
					0x0000000FFFFFFFFFu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x000303FFFFFFFFFFu, 0x0000000000000000u,
					0x000000000FFFFFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x10300ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x16: // [22] 10F1C - 11B66
			{
				// 1174 code units from 71 ranges (spanning a search area of 3037)

				if (c > U'\U00011AF8')
					return false;
				TOML_ASSUME(c >= U'\U00010F1C');

				constexpr uint_least64_t bitmask_table_1[] = {
					0x000003FFFFF00801u, 0x0000003FFFF00000u, 0x000001FFFFF00000u, 0xFFFFFF8007FFFFF0u,
					0x000000000FFFFFFFu, 0xFFFFFF8002600000u, 0xFFF00000000FFFFFu, 0xFFFFFF8000001FFFu,
					0xFFF00900000007FFu, 0xFFFFFF80047FFFFFu, 0x400001E0007FFFFFu, 0xFFBFFFF000000001u,
					0x000000000000FFFFu, 0xFFFBD7F000000000u, 0xFFFFFFFFFFF01FFBu, 0xFF99FE0000000007u,
					0x001000023EDFDFFFu, 0x000000000000003Eu, 0x0000000000000000u, 0xFFFFFFF000000000u,
					0x0000780001FFFFFFu, 0xFFFFFFF000000038u, 0x00000B00000FFFFFu, 0x0000000000000000u,
					0x0000000000000000u, 0xFFFFFFF000000000u, 0xF00000000007FFFFu, 0xFFFFFFF000000000u,
					0x00000100000FFFFFu, 0xFFFFFFF000000000u, 0x0000000010007FFFu, 0x7FFFFFF000000000u,
					0x000007F000000000u, 0x0000000000000000u, 0x0000000000000000u, 0xFFFFFFF000000000u,
					0x000000000000FFFFu, 0x0000000000000000u, 0xFFFFFFFFFFFFFFF0u, 0xF6FF27F80000000Fu,
					0x00000028000FFFFFu, 0x0000000000000000u, 0x001FFFFFFFFFCFF0u, 0xFFFF8010000000A0u,
					0x00100000407FFFFFu, 0x00003FFFFFFFFFFFu, 0xFFFFFFFFFFF00002u, 0x000000001FFFFFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x10F1Cull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x10F1Cull) % 0x40ull));
			}
			case 0x17: // [23] 11B67 - 127B1
			{
				// 1304 code units from 16 ranges (spanning a search area of 2372)

				if (c < U'\U00011C00' || c > U'\U00012543')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00007FFFFFFFFDFFu, 0xFFFC000000000001u, 0x000000000000FFFFu, 0x0000000000000000u,
					0x0001FFFFFFFFFB7Fu, 0xFFFFFDBF00000040u, 0x00000000010003FFu, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0007FFFF00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0001000000000000u, 0x0000000000000000u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x0000000003FFFFFFu, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0x000000000000000Fu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x11C00ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x18: return (U'\U00012F90' <= c && c <= U'\U00012FF0') || U'\U00013000' <= c;
			case 0x19: return c <= U'\U0001342E';
			case 0x1A: return c >= U'\U00014400' && c <= U'\U00014646';
			case 0x1D: // [29] 16529 - 17173
			{
				// 1329 code units from 15 ranges (spanning a search area of 2420)

				if (c < U'\U00016800')
					return false;
				TOML_ASSUME(c <= U'\U00017173');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x01FFFFFFFFFFFFFFu, 0xFFFF00007FFFFFFFu, 0x7FFFFFFFFFFFFFFFu, 0x00003FFFFFFF0000u,
					0x0000FFFFFFFFFFFFu, 0xE0FFFFF80000000Fu, 0x000000000000FFFFu, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0xFFFFFFFFFFFFFFFFu, 0x0000000000000000u, 0x0000000000000000u,
					0xFFFFFFFFFFFFFFFFu, 0x00000000000107FFu, 0x00000000FFF80000u, 0x0000000B00000000u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0x000FFFFFFFFFFFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x16800ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x1F: return c <= U'\U000187F7' || U'\U00018800' <= c;
			case 0x20: return c <= U'\U00018CD5' || (U'\U00018D00' <= c && c <= U'\U00018D08');
			case 0x23: // [35] 1AEEB - 1BB35
			{
				// 707 code units from 7 ranges (spanning a search area of 780)

				if (c < U'\U0001AFF0' || c > U'\U0001B2FB')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFF6FEFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x0007FFFFFFFFFFFFu, 0x00F0000700000000u, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0x0000000000000FFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x1AFF0ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x1AFF0ull) % 0x40ull));
			}
			case 0x24: // [36] 1BB36 - 1C780
			{
				// 139 code units from 4 ranges (spanning a search area of 154)

				if (c < U'\U0001BC00' || c > U'\U0001BC99')
					return false;

				switch ((static_cast<uint_least64_t>(c) - 0x1BC00ull) / 0x40ull)
				{
					case 0x01:
						return c <= U'\U0001BC7C'
							&& (1ull << (static_cast<uint_least64_t>(c) - 0x1BC40ull)) & 0x1FFF07FFFFFFFFFFull;
					case 0x02: return (1u << (static_cast<uint_least32_t>(c) - 0x1BC80u)) & 0x3FF01FFu;
					default: return true;
				}

				TOML_UNREACHABLE;
			}
			case 0x26: // [38] 1D3CC - 1E016
			{
				// 967 code units from 31 ranges (spanning a search area of 2847)

				if (c < U'\U0001D400' || c > U'\U0001DF1E')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFDFFFFFu, 0xEBFFDE64DFFFFFFFu, 0xFFFFFFFFFFFFFFEFu,
					0x7BFFFFFFDFDFE7BFu, 0xFFFFFFFFFFFDFC5Fu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFF3FFFFFFFFFu, 0xF7FFFFFFF7FFFFFDu,
					0xFFDFFFFFFFDFFFFFu, 0xFFFF7FFFFFFF7FFFu, 0xFFFFFDFFFFFFFDFFu, 0x0000000000000FF7u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x000000007FFFFFFFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x1D400ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x27: // [39] 1E017 - 1EC61
			{
				// 421 code units from 12 ranges (spanning a search area of 2124)

				if (c < U'\U0001E100' || c > U'\U0001E94B')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x3F801FFFFFFFFFFFu, 0x0000000000004000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x00003FFFFFFF0000u, 0x00000FFFFFFFFFFFu,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x7FFF6F7F00000000u,
					0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0xFFFFFFFFFFFFFFFFu, 0x000000000000001Fu,
					0xFFFFFFFFFFFFFFFFu, 0x000000000000080Fu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x1E100ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x28: // [40] 1EC62 - 1F8AC
			{
				// 141 code units from 33 ranges (spanning a search area of 188)

				if (c < U'\U0001EE00' || c > U'\U0001EEBB')
					return false;

				switch ((static_cast<uint_least64_t>(c) - 0x1EE00ull) / 0x40ull)
				{
					case 0x00:
						return c <= U'\U0001EE3B'
							&& (1ull << (static_cast<uint_least64_t>(c) - 0x1EE00ull)) & 0xAF7FE96FFFFFFEFull;
					case 0x01:
						return c >= U'\U0001EE42' && c <= U'\U0001EE7E'
							&& (1ull << (static_cast<uint_least64_t>(c) - 0x1EE42ull)) & 0x17BDFDE5AAA5BAA1ull;
					case 0x02: return (1ull << (static_cast<uint_least64_t>(c) - 0x1EE80ull)) & 0xFFFFBEE0FFFFBFFull;
					default: TOML_UNREACHABLE;
				}

				TOML_UNREACHABLE;
			}
			case 0x29: return c >= U'\U00020000';
			case 0x37: return c <= U'\U0002A6DF' || U'\U0002A700' <= c;
			case 0x38: return c <= U'\U0002B738' || (U'\U0002B740' <= c && c <= U'\U0002B81D') || U'\U0002B820' <= c;
			case 0x3A: return c <= U'\U0002CEA1' || U'\U0002CEB0' <= c;
			case 0x3C: return c <= U'\U0002EBE0';
			case 0x3D: return c >= U'\U0002F800' && c <= U'\U0002FA1D';
			case 0x3E: return c >= U'\U00030000';
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_number(char32_t c) noexcept
	{
		// 886 code units from 73 ranges (spanning a search area of 128410)

		if (c < U'\u0660' || c > U'\U0001FBF9')
			return false;

		const auto child_index_0 = (static_cast<uint_least64_t>(c) - 0x660ull) / 0x7D7ull;
		if ((1ull << child_index_0) & 0x47FFDFE07FCFFFD0ull)
			return false;
		switch (child_index_0)
		{
			case 0x00: // [0] 0660 - 0E36
			{
				// 130 code units from 13 ranges (spanning a search area of 1936)

				if (c > U'\u0DEF')
					return false;
				TOML_ASSUME(c >= U'\u0660');

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00000000000003FFu, 0x0000000000000000u, 0x0000000003FF0000u, 0x0000000000000000u,
					0x0000000000000000u, 0x000003FF00000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x000000000000FFC0u, 0x0000000000000000u, 0x000000000000FFC0u, 0x0000000000000000u,
					0x000000000000FFC0u, 0x0000000000000000u, 0x000000000000FFC0u, 0x0000000000000000u,
					0x000000000000FFC0u, 0x0000000000000000u, 0x000000000000FFC0u, 0x0000000000000000u,
					0x000000000000FFC0u, 0x0000000000000000u, 0x000000000000FFC0u, 0x0000000000000000u,
					0x000000000000FFC0u, 0x0000000000000000u, 0x000000000000FFC0u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x660ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x660ull) % 0x40ull));
			}
			case 0x01: // [1] 0E37 - 160D
			{
				// 50 code units from 5 ranges (spanning a search area of 586)

				if (c < U'\u0E50' || c > U'\u1099')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00000000000003FFu, 0x0000000000000000u, 0x00000000000003FFu, 0x0000000003FF0000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x03FF000000000000u,
					0x0000000000000000u, 0x00000000000003FFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xE50ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xE50ull) % 0x40ull));
			}
			case 0x02: // [2] 160E - 1DE4
			{
				// 103 code units from 11 ranges (spanning a search area of 1388)

				if (c < U'\u16EE' || c > U'\u1C59')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x0000000000000007u, 0x0000000000000000u, 0x0000000000000000u, 0x0FFC000000000000u,
					0x00000FFC00000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x00000003FF000000u, 0x0000000000000000u, 0x00000FFC00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x00000FFC0FFC0000u, 0x0000000000000000u,
					0x0000000000000000u, 0x00000FFC00000000u, 0x0000000000000000u, 0x0000000000000FFCu,
					0x0000000000000000u, 0x00000FFC0FFC0000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x16EEull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x16EEull) % 0x40ull));
			}
			case 0x03: return (U'\u2160' <= c && c <= U'\u2182') || (U'\u2185' <= c && c <= U'\u2188');
			case 0x05:
				return (U'\u3021' <= c && c <= U'\u3029') || (U'\u3038' <= c && c <= U'\u303A') || c == U'\u3007';
			case 0x14: // [20] A32C - AB02
			{
				// 70 code units from 7 ranges (spanning a search area of 1082)

				if (c < U'\uA620' || c > U'\uAA59')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00000000000003FFu, 0x0000000000000000u, 0x0000000000000000u, 0x000000000000FFC0u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x03FF000000000000u, 0x000003FF00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x03FF000000000000u, 0x0000000003FF0000u,
					0x03FF000000000000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xA620ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xA620ull) % 0x40ull));
			}
			case 0x15: return c >= U'\uABF0' && c <= U'\uABF9';
			case 0x1F: return c >= U'\uFF10' && c <= U'\uFF19';
			case 0x20: // [32] 10140 - 10916
			{
				// 70 code units from 5 ranges (spanning a search area of 874)

				if (c > U'\U000104A9')
					return false;
				TOML_ASSUME(c >= U'\U00010140');

				constexpr uint_least64_t bitmask_table_1[] = {
					0x001FFFFFFFFFFFFFu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000402u, 0x0000000000000000u, 0x00000000003E0000u, 0x0000000000000000u,
					0x0000000000000000u, 0x000003FF00000000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x10140ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x21: return (U'\U00010D30' <= c && c <= U'\U00010D39') || (U'\U00011066' <= c && c <= U'\U0001106F');
			case 0x22: // [34] 110EE - 118C4
			{
				// 90 code units from 9 ranges (spanning a search area of 1610)

				if (c < U'\U000110F0' || c > U'\U00011739')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00000000000003FFu, 0x000000000000FFC0u, 0x0000000000000000u, 0x000003FF00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x00000000000003FFu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x000003FF00000000u, 0x0000000000000000u, 0x000003FF00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x000003FF00000000u, 0x0000000000000000u, 0x0000000003FF0000u,
					0x0000000000000000u, 0x00000000000003FFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x110F0ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x110F0ull) % 0x40ull));
			}
			case 0x23: // [35] 118C5 - 1209B
			{
				// 50 code units from 5 ranges (spanning a search area of 1226)

				if (c < U'\U000118E0' || c > U'\U00011DA9')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x00000000000003FFu, 0x03FF000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x03FF000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x03FF000000000000u, 0x0000000000000000u, 0x00000000000003FFu,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x118E0ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x118E0ull) % 0x40ull));
			}
			case 0x24: return c >= U'\U00012400' && c <= U'\U0001246E';
			case 0x2D:
				return (U'\U00016A60' <= c && c <= U'\U00016A69') || (U'\U00016AC0' <= c && c <= U'\U00016AC9')
					|| (U'\U00016B50' <= c && c <= U'\U00016B59');
			case 0x3B: return c >= U'\U0001D7CE' && c <= U'\U0001D7FF';
			case 0x3C: return (U'\U0001E140' <= c && c <= U'\U0001E149') || (U'\U0001E2F0' <= c && c <= U'\U0001E2F9');
			case 0x3D: return c >= U'\U0001E950' && c <= U'\U0001E959';
			case 0x3F: return c >= U'\U0001FBF0';
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_CONST_GETTER
	constexpr bool is_combining_mark(char32_t c) noexcept
	{
		// 2395 code units from 302 ranges (spanning a search area of 917232)

		if (c < U'\u0300' || c > U'\U000E01EF')
			return false;

		const auto child_index_0 = (static_cast<uint_least64_t>(c) - 0x300ull) / 0x37FCull;
		if ((1ull << child_index_0) & 0x7FFFFFFFFFFFFE02ull)
			return false;
		switch (child_index_0)
		{
			case 0x00: // [0] 0300 - 3AFB
			{
				// 1141 code units from 158 ranges (spanning a search area of 11675)

				if (c > U'\u309A')
					return false;
				TOML_ASSUME(c >= U'\u0300');

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFFFFFFFFFFFFFFu, 0x0000FFFFFFFFFFFFu, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x00000000000000F8u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0xBFFFFFFFFFFE0000u, 0x00000000000000B6u,
					0x0000000007FF0000u, 0x00010000FFFFF800u, 0x0000000000000000u, 0x00003D9F9FC00000u,
					0xFFFF000000020000u, 0x00000000000007FFu, 0x0001FFC000000000u, 0x200FF80000000000u,
					0x00003EEFFBC00000u, 0x000000000E000000u, 0x00000000FF000000u, 0xFFFFFFFBFFFFFC00u,
					0xDC0000000000000Fu, 0x0000000C00FEFFFFu, 0xD00000000000000Eu, 0x4000000C0080399Fu,
					0xD00000000000000Eu, 0x0023000000023987u, 0xD00000000000000Eu, 0xFC00000C00003BBFu,
					0xD00000000000000Eu, 0x0000000C00E0399Fu, 0xC000000000000004u, 0x0000000000803DC7u,
					0xD00000000000001Fu, 0x0000000C00603DDFu, 0xD00000000000000Eu, 0x0000000C00603DDFu,
					0xD80000000000000Fu, 0x0000000C00803DDFu, 0x000000000000000Eu, 0x000C0000FF5F8400u,
					0x07F2000000000000u, 0x0000000000007F80u, 0x1FF2000000000000u, 0x0000000000003F00u,
					0xC2A0000003000000u, 0xFFFE000000000000u, 0x1FFFFFFFFEFFE0DFu, 0x0000000000000040u,
					0x7FFFF80000000000u, 0x001E3F9DC3C00000u, 0x000000003C00BFFCu, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x00000000E0000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x001C0000003C0000u, 0x000C0000000C0000u, 0xFFF0000000000000u, 0x00000000200FFFFFu,
					0x000000000000B800u, 0x0000000000000000u, 0x0000020000000060u, 0x0000000000000000u,
					0x0FFF0FFF00000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x000000000F800000u, 0x9FFFFFFF7FE00000u, 0xBFFF000000000000u, 0x0000000000007FFFu,
					0xFFF000000000001Fu, 0x000FF8000000001Fu, 0x00003FFE00000007u, 0x000FFFC000000000u,
					0x00FFFFF000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x039021FFFFF70000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0xFFFFFFFFFFFFFFFFu,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0001FFE21FFF0000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0003800000000000u,
					0x0000000000000000u, 0x8000000000000000u, 0x0000000000000000u, 0xFFFFFFFF00000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000FC0000000000u, 0x0000000000000000u, 0x0000000006000000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x300ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x02: // [2] 72F8 - AAF3
			{
				// 137 code units from 28 ranges (spanning a search area of 1153)

				if (c < U'\uA66F' || c > U'\uAAEF')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x0001800000007FE1u, 0x0000000000000000u, 0x0000000000000006u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x21F0000010880000u, 0x0000000000000000u,
					0x0000000000060000u, 0xFFFE0000007FFFE0u, 0x7F80000000010007u, 0x0000001FFF000000u,
					0x00000000001E0000u, 0x004000000003FFF0u, 0xFC00000000000000u, 0x00000000601000FFu,
					0x0000000000007000u, 0xF00000000005833Au, 0x0000000000000001u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xA66Full) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xA66Full) % 0x40ull));
			}
			case 0x03:
				return (U'\uAAF5' <= c && c <= U'\uAAF6') || (U'\uABE3' <= c && c <= U'\uABEA')
					|| (U'\uABEC' <= c && c <= U'\uABED');
			case 0x04: // [4] E2F0 - 11AEB
			{
				// 410 code units from 67 ranges (spanning a search area of 8060)

				if (c < U'\uFB1E' || c > U'\U00011A99')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x0000000000000001u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0003FFFC00000000u,
					0x000000000003FFFCu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000080000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000004u,
					0x0000000000000000u, 0x000000001F000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0003C1B800000000u,
					0x000000021C000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000180u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x00000000000003C0u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000006000u, 0x0000000000000000u,
					0x0007FF0000000000u, 0x000000F000000000u, 0x0000000000000000u, 0x0000001C00000000u,
					0x000001FFFC000000u, 0x0000001E00640000u, 0x000000101FFC0000u, 0x0000001C00000000u,
					0x00000180007FFE00u, 0x0000001C00200000u, 0x00037807FFE00000u, 0x0000000000000000u,
					0x0000000103FFC000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000003C00001FFEu,
					0x0200E67F60000000u, 0x00000000007C7F30u, 0x0000000000000000u, 0x0000000000000000u,
					0x000001FFFF800000u, 0x0000000000000001u, 0x0000003FFFFC0000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0xC0000007FCFE0000u, 0x0000000000000000u,
					0x00000007FFFC0000u, 0x0000000000000000u, 0x0000000003FFE000u, 0x8000000000000000u,
					0x0000000000003FFFu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x000000001FFFC000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x00000035E6FC0000u, 0x0000000000000000u, 0xF3F8000000000000u, 0x00001FF800000047u,
					0x3FF80201EFE00000u, 0x0FFFF00000000000u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0xFB1Eull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0xFB1Eull) % 0x40ull));
			}
			case 0x05: // [5] 11AEC - 152E7
			{
				// 85 code units from 13 ranges (spanning a search area of 712)

				if (c < U'\U00011C2F' || c > U'\U00011EF6')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x000000000001FEFFu, 0xFDFFFFF800000000u, 0x00000000000000FFu, 0x0000000000000000u,
					0x00000000017F68FCu, 0x000001F6F8000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x00000000000000F0u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x11C2Full) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x11C2Full) % 0x40ull));
			}
			case 0x06: // [6] 152E8 - 18AE3
			{
				// 75 code units from 7 ranges (spanning a search area of 1282)

				if (c < U'\U00016AF0' || c > U'\U00016FF1')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0x000000000000001Fu, 0x000000000000007Fu, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0xFFFFFFFE80000000u, 0x0000000780FFFFFFu, 0x0010000000000000u,
					0x0000000000000003u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x16AF0ull) / 0x40ull]
					 & (0x1ull << ((static_cast<uint_least64_t>(c) - 0x16AF0ull) % 0x40ull));
			}
			case 0x07: return c >= U'\U0001BC9D' && c <= U'\U0001BC9E';
			case 0x08: // [8] 1C2E0 - 1FADB
			{
				// 293 code units from 24 ranges (spanning a search area of 6731)

				if (c < U'\U0001CF00' || c > U'\U0001E94A')
					return false;

				constexpr uint_least64_t bitmask_table_1[] = {
					0xFFFF3FFFFFFFFFFFu, 0x000000000000007Fu, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0xF807E3E000000000u, 0x00003C0000000FE7u, 0x0000000000000000u,
					0x0000000000000000u, 0x000000000000001Cu, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0xF87FFFFFFFFFFFFFu, 0x00201FFFFFFFFFFFu, 0x0000FFFEF8000010u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x000007DBF9FFFF7Fu, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x007F000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000400000000000u, 0x0000F00000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u,
					0x0000000000000000u, 0x0000000000000000u, 0x0000000000000000u, 0x00000000007F0000u,
					0x0000000000000000u, 0x00000000000007F0u,
				};
				return bitmask_table_1[(static_cast<uint_least64_t>(c) - 0x1CF00ull) / 0x40ull]
					 & (0x1ull << (static_cast<uint_least64_t>(c) % 0x40ull));
			}
			case 0x3F: return c >= U'\U000E0100';
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_bare_key_character(char32_t c) noexcept
	{
		return is_non_ascii_letter(c) || is_non_ascii_number(c) || is_combining_mark(c);
	}

#endif // TOML_LANG_UNRELEASED
}
TOML_IMPL_NAMESPACE_END;

#if TOML_GCC && TOML_GCC < 9
#pragma GCC pop_options
#endif

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/unicode.h  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_CONST_GETTER
	constexpr bool is_string_delimiter(char32_t c) noexcept
	{
		return c == U'"' || c == U'\'';
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_letter(char32_t c) noexcept
	{
		return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
	}

	TOML_CONST_GETTER
	constexpr bool is_binary_digit(char32_t c) noexcept
	{
		return c == U'0' || c == U'1';
	}

	TOML_CONST_GETTER
	constexpr bool is_octal_digit(char32_t c) noexcept
	{
		return (c >= U'0' && c <= U'7');
	}

	TOML_CONST_GETTER
	constexpr bool is_decimal_digit(char32_t c) noexcept
	{
		return (c >= U'0' && c <= U'9');
	}

	TOML_CONST_GETTER
	constexpr bool is_hexadecimal_digit(char32_t c) noexcept
	{
		return U'0' <= c && c <= U'f' && (1ull << (static_cast<uint_least64_t>(c) - 0x30u)) & 0x7E0000007E03FFull;
	}

	template <typename T>
	TOML_CONST_GETTER
	constexpr uint_least32_t hex_to_dec(const T c) noexcept
	{
		if constexpr (std::is_same_v<remove_cvref<T>, uint_least32_t>)
			return c >= 0x41u					 // >= 'A'
					 ? 10u + (c | 0x20u) - 0x61u // - 'a'
					 : c - 0x30u				 // - '0'
				;
		else
			return hex_to_dec(static_cast<uint_least32_t>(c));
	}

	TOML_CONST_GETTER
	constexpr bool is_bare_key_character(char32_t c) noexcept
	{
		return is_ascii_bare_key_character(c)
#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
			|| is_non_ascii_bare_key_character(c)
#endif
			;
	}

	TOML_CONST_GETTER
	constexpr bool is_value_terminator(char32_t c) noexcept
	{
		return is_whitespace(c) || c == U']' || c == U'}' || c == U',' || c == U'#';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char c) noexcept
	{
		return c <= '\u001F' || c == '\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char32_t c) noexcept
	{
		return c <= U'\u001F' || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_nontab_control_character(char32_t c) noexcept
	{
		return c <= U'\u0008' || (c >= U'\u000A' && c <= U'\u001F') || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_unicode_surrogate(char32_t c) noexcept
	{
		return c >= 0xD800u && c <= 0xDFFF;
	}

	struct utf8_decoder
	{
		// utf8_decoder based on this: https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
		// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

		uint_least32_t state{};
		char32_t codepoint{};

		static constexpr uint8_t state_table[]{
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,
			1,	1,	1,	1,	1,	1,	1,	1,	1,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	7,	7,
			7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
			7,	7,	7,	8,	8,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
			2,	2,	2,	2,	2,	2,	2,	2,	10, 3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	4,	3,	3,	11, 6,	6,
			6,	5,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,
			0,	12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0,	12,
			12, 12, 12, 12, 0,	12, 0,	12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12,
			12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36, 12,
			36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12
		};

		TOML_PURE_INLINE_GETTER
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool has_code_point() const noexcept
		{
			return state == uint_least32_t{};
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool needs_more_input() const noexcept
		{
			return !has_code_point() && !error();
		}

		constexpr void operator()(uint8_t byte) noexcept
		{
			TOML_ASSERT_ASSUME(!error());

			const auto type = state_table[byte];

			codepoint = static_cast<char32_t>(has_code_point() ? (uint_least32_t{ 255u } >> type) & byte
															   : (byte & uint_least32_t{ 63u })
																	 | (static_cast<uint_least32_t>(codepoint) << 6));

			state = state_table[state + uint_least32_t{ 256u } + type];
		}

		TOML_ALWAYS_INLINE
		constexpr void operator()(char c) noexcept
		{
			operator()(static_cast<uint8_t>(c));
		}

		TOML_ALWAYS_INLINE
		constexpr void reset() noexcept
		{
			state = {};
		}
	};

	TOML_PURE_GETTER
	TOML_ATTR(nonnull)
	bool is_ascii(const char* str, size_t len) noexcept;
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/parse_error.h  ****************************************************************************************

#if TOML_ENABLE_PARSER

TOML_DISABLE_WARNINGS;
#if TOML_EXCEPTIONS
#include <stdexcept>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#if defined(DOXYGEN) || !TOML_EXCEPTIONS
#define TOML_PARSE_ERROR_BASE
#else
#define TOML_PARSE_ERROR_BASE	 : public std::runtime_error
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	class parse_error TOML_PARSE_ERROR_BASE
	{
	  private:
#if !TOML_EXCEPTIONS
		std::string description_;
#endif
		source_region source_;

	  public:
#if TOML_EXCEPTIONS

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, source_region&& src) noexcept //
			: std::runtime_error{ desc },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_region& src) noexcept //
			: parse_error{ desc, source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ desc, source_region{ position, position, path } }
		{}

#else

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, source_region&& src) noexcept //
			: description_{ std::move(desc) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_region& src) noexcept //
			: parse_error{ std::move(desc), source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ std::move(desc), source_region{ position, position, path } }
		{}

#endif

		TOML_NODISCARD
		std::string_view description() const noexcept
		{
#if TOML_EXCEPTIONS
			return std::string_view{ what() };
#else
			return description_;
#endif
		}

		TOML_NODISCARD
		const source_region& source() const noexcept
		{
			return source_;
		}

		friend std::ostream& operator<<(std::ostream& lhs, const parse_error& rhs)
		{
			impl::print_to_stream(lhs, rhs.description());
			impl::print_to_stream(lhs, "\n\t(error occurred at "sv);
			impl::print_to_stream(lhs, rhs.source());
			impl::print_to_stream(lhs, ")"sv);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#undef TOML_PARSE_ERROR_BASE

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/parse_result.h  ***************************************************************************************

#if defined(DOXYGEN) || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_START(noex);

	class parse_result
	{
	  private:
		struct storage_t
		{
			static constexpr size_t size_ =
				(sizeof(toml::table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(toml::table));
			static constexpr size_t align_ =
				(alignof(toml::table) < alignof(parse_error) ? alignof(parse_error) : alignof(toml::table));

			alignas(align_) unsigned char bytes[size_];
		};

		mutable storage_t storage_;
		bool err_;

		template <typename Type>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static Type* get_as(storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<Type*>(s.bytes));
		}

		void destroy() noexcept
		{
			if (err_)
				get_as<parse_error>(storage_)->~parse_error();
			else
				get_as<toml::table>(storage_)->~table();
		}

	  public:

		TOML_NODISCARD_CTOR
		parse_result() noexcept //
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::string{}, source_region{} };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(toml::table&& tbl) noexcept //
			: err_{ false }
		{
			::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(tbl) };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(parse_error&& err) noexcept //
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(err) };
		}

		TOML_NODISCARD_CTOR
		parse_result(parse_result&& res) noexcept //
			: err_{ res.err_ }
		{
			if (err_)
				::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(res).error() };
			else
				::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(res).table() };
		}

		parse_result& operator=(parse_result&& rhs) noexcept
		{
			if (err_ != rhs.err_)
			{
				destroy();
				err_ = rhs.err_;
				if (err_)
					::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(rhs).error() };
				else
					::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(rhs).table() };
			}
			else
			{
				if (err_)
					error() = std::move(rhs).error();
				else
					table() = std::move(rhs).table();
			}
			return *this;
		}

		~parse_result() noexcept
		{
			destroy();
		}

		TOML_NODISCARD
		bool succeeded() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		bool failed() const noexcept
		{
			return err_;
		}

		TOML_NODISCARD
		explicit operator bool() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		toml::table& table() & noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<toml::table>(storage_);
		}

		TOML_NODISCARD
		toml::table&& table() && noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return static_cast<toml::table&&>(*get_as<toml::table>(storage_));
		}

		TOML_NODISCARD
		const toml::table& table() const& noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<const toml::table>(storage_);
		}

		TOML_NODISCARD
		/* implicit */ operator toml::table&() noexcept
		{
			return table();
		}

		TOML_NODISCARD
		/* implicit */ operator toml::table&&() noexcept
		{
			return std::move(table());
		}

		TOML_NODISCARD
		/* implicit */ operator const toml::table&() const noexcept
		{
			return table();
		}

		TOML_NODISCARD
		parse_error& error() & noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<parse_error>(storage_);
		}

		TOML_NODISCARD
		parse_error&& error() && noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return static_cast<parse_error&&>(*get_as<parse_error>(storage_));
		}

		TOML_NODISCARD
		const parse_error& error() const& noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<const parse_error>(storage_);
		}

		TOML_NODISCARD
		explicit operator parse_error&() noexcept
		{
			return error();
		}

		TOML_NODISCARD
		explicit operator parse_error&&() noexcept
		{
			return std::move(error());
		}

		TOML_NODISCARD
		explicit operator const parse_error&() const noexcept
		{
			return error();
		}

		using iterator = table_iterator;

		using const_iterator = const_table_iterator;

		TOML_NODISCARD
		table_iterator begin() noexcept
		{
			return err_ ? table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator begin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator cbegin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cbegin();
		}

		TOML_NODISCARD
		table_iterator end() noexcept
		{
			return err_ ? table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator end() const noexcept
		{
			return err_ ? const_table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator cend() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cend();
		}

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<node> at_path(std::string_view path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::string_view path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key) noexcept
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<node> at_path(std::wstring_view path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::wstring_view path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& os, const parse_result& result)
		{
			return result.err_ ? (os << result.error()) : (os << result.table());
		}

#endif
	};

	TOML_ABI_NAMESPACE_END;
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

//********  impl/parser.h  *********************************************************************************************

#if TOML_ENABLE_PARSER

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse_file(std::string_view file_path);

#if TOML_HAS_CHAR8

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::u8string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::u8string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse_file(std::u8string_view file_path);

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::string_view doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::istream & doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse_file(std::wstring_view file_path);

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::u8string_view doc, std::wstring_view source_path);

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::istream & doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result parse(std::istream & doc, std::string && source_path);

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	inline namespace literals
	{
		TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, lit_ex, lit_noex);

		TOML_NODISCARD
		inline parse_result operator"" _toml(const char* str, size_t len)
		{
			return parse(std::string_view{ str, len });
		}

#if TOML_HAS_CHAR8

		TOML_NODISCARD
		inline parse_result operator"" _toml(const char8_t* str, size_t len)
		{
			return parse(std::u8string_view{ str, len });
		}

#endif // TOML_HAS_CHAR8

		TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/formatter.h  ******************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	struct formatter_constants
	{
		format_flags mandatory_flags;
		format_flags ignored_flags;

		std::string_view float_pos_inf;
		std::string_view float_neg_inf;
		std::string_view float_nan;

		std::string_view bool_true;
		std::string_view bool_false;
	};

	struct formatter_config
	{
		format_flags flags;
		std::string_view indent;
	};

	class TOML_EXPORTED_CLASS formatter
	{
	  private:
		const node* source_;
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		const parse_result* result_;
#endif
		const formatter_constants* constants_;
		formatter_config config_;
		size_t indent_columns_;
		format_flags int_format_mask_;
		std::ostream* stream_; //
		int indent_;		   // these are set in attach()
		bool naked_newline_;   //

	  protected:
		TOML_PURE_INLINE_GETTER
		const node& source() const noexcept
		{
			return *source_;
		}

		TOML_PURE_INLINE_GETTER
		std::ostream& stream() const noexcept
		{
			return *stream_;
		}

		TOML_PURE_INLINE_GETTER
		int indent() const noexcept
		{
			return indent_;
		}

		void indent(int level) noexcept
		{
			indent_ = level;
		}

		void increase_indent() noexcept
		{
			indent_++;
		}

		void decrease_indent() noexcept
		{
			indent_--;
		}

		TOML_PURE_INLINE_GETTER
		size_t indent_columns() const noexcept
		{
			return indent_columns_;
		}

		TOML_PURE_INLINE_GETTER
		bool indent_array_elements() const noexcept
		{
			return !!(config_.flags & format_flags::indent_array_elements);
		}

		TOML_PURE_INLINE_GETTER
		bool indent_sub_tables() const noexcept
		{
			return !!(config_.flags & format_flags::indent_sub_tables);
		}

		TOML_PURE_INLINE_GETTER
		bool literal_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_literal_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool multi_line_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_multi_line_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool real_tabs_in_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_real_tabs_in_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool unicode_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_unicode_strings);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void attach(std::ostream& stream) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void detach() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_newline(bool force = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_indent();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(char);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(std::string_view);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_string(std::string_view str, bool allow_multi_line = true, bool allow_bare = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<int64_t>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<double>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<bool>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date_time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_value(const node&, node_type);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool dump_failed_parse_result();

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		formatter(const node*, const parse_result*, const formatter_constants&, const formatter_config&) noexcept;
	};
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/toml_formatter.h  *************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS toml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;
		std::vector<const key*> key_path_;
		bool pending_table_separator_ = false;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_pending_table_separator();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const key&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_inline(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = { format_flags::none, // mandatory
																 format_flags::none, // ignored
																 "inf"sv,
																 "-inf"sv,
																 "nan"sv,
																 "true"sv,
																 "false"sv };

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags				   //
													| format_flags::allow_literal_strings	   //
													| format_flags::allow_multi_line_strings   //
													| format_flags::allow_unicode_strings	   //
													| format_flags::allow_real_tabs_in_strings //
													| format_flags::allow_binary_integers	   //
													| format_flags::allow_octal_integers	   //
													| format_flags::allow_hexadecimal_integers //
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if defined(DOXYGEN) || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.key_path_.clear();
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/json_formatter.h  *************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS json_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = {
			format_flags::quote_dates_and_times,										  // mandatory
			format_flags::allow_literal_strings | format_flags::allow_multi_line_strings, // ignored
			"Infinity"sv,
			"-Infinity"sv,
			"NaN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags				  //
													| format_flags::quote_infinities_and_nans //
													| format_flags::allow_unicode_strings	  //
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if defined(DOXYGEN) || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/yaml_formatter.h  *************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS yaml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_yaml_string(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = {
			//
			format_flags::quote_dates_and_times | format_flags::indentation, // mandatory
			format_flags::allow_multi_line_strings,							 // ignored
			".inf"sv,
			"-.inf"sv,
			".NAN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags			  //
													| format_flags::allow_literal_strings //
													| format_flags::allow_unicode_strings //
													| format_flags::allow_octal_integers  //
													| format_flags::allow_hexadecimal_integers;

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "  "sv } }
		{}

#if defined(DOXYGEN) || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "  "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, yaml_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, yaml_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

#if TOML_IMPLEMENTATION

//********  impl/std_string.inl  ***************************************************************************************

#if TOML_ENABLE_WINDOWS_COMPAT

#ifndef _WINDOWS_
#if TOML_INCLUDE_WINDOWS_H
#include <Windows.h>
#else

extern "C" __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int CodePage,
																   unsigned long dwFlags,
																   const wchar_t* lpWideCharStr,
																   int cchWideChar,
																   char* lpMultiByteStr,
																   int cbMultiByte,
																   const char* lpDefaultChar,
																   int* lpUsedDefaultChar);

extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int CodePage,
																   unsigned long dwFlags,
																   const char* lpMultiByteStr,
																   int cbMultiByte,
																   wchar_t* lpWideCharStr,
																   int cchWideChar);

#endif // TOML_INCLUDE_WINDOWS_H
#endif // _WINDOWS_

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	std::string narrow(std::wstring_view str)
	{
		if (str.empty())
			return {};

		std::string s;
		const auto len =
			::WideCharToMultiByte(65001, 0, str.data(), static_cast<int>(str.length()), nullptr, 0, nullptr, nullptr);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::WideCharToMultiByte(65001,
								  0,
								  str.data(),
								  static_cast<int>(str.length()),
								  s.data(),
								  len,
								  nullptr,
								  nullptr);
		}
		return s;
	}

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::string_view str)
	{
		if (str.empty())
			return {};

		std::wstring s;
		const auto len = ::MultiByteToWideChar(65001, 0, str.data(), static_cast<int>(str.length()), nullptr, 0);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::MultiByteToWideChar(65001, 0, str.data(), static_cast<int>(str.length()), s.data(), len);
		}
		return s;
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::u8string_view str)
	{
		if (str.empty())
			return {};

		return widen(std::string_view{ reinterpret_cast<const char*>(str.data()), str.length() });
	}

#endif // TOML_HAS_CHAR8
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_WINDOWS_COMPAT

//********  impl/print_to_stream.inl  **********************************************************************************

TOML_DISABLE_WARNINGS;
#include <ostream>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
#include <sstream>
#endif
#if !TOML_INT_CHARCONV
#include <iomanip>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	inline constexpr size_t charconv_buffer_length = 0;

	template <>
	inline constexpr size_t charconv_buffer_length<int8_t> = 4; // strlen("-128")

	template <>
	inline constexpr size_t charconv_buffer_length<int16_t> = 6; // strlen("-32768")

	template <>
	inline constexpr size_t charconv_buffer_length<int32_t> = 11; // strlen("-2147483648")

	template <>
	inline constexpr size_t charconv_buffer_length<int64_t> = 20; // strlen("-9223372036854775808")

	template <>
	inline constexpr size_t charconv_buffer_length<uint8_t> = 3; // strlen("255")

	template <>
	inline constexpr size_t charconv_buffer_length<uint16_t> = 5; // strlen("65535")

	template <>
	inline constexpr size_t charconv_buffer_length<uint32_t> = 10; // strlen("4294967295")

	template <>
	inline constexpr size_t charconv_buffer_length<uint64_t> = 20; // strlen("18446744073709551615")

	template <>
	inline constexpr size_t charconv_buffer_length<float> = 64;

	template <>
	inline constexpr size_t charconv_buffer_length<double> = 64;

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_integer_to_stream(std::ostream & stream, T val, value_flags format = {}, size_t min_digits = 0)
	{
		if (!val)
		{
			if (!min_digits)
				min_digits = 1;

			for (size_t i = 0; i < min_digits; i++)
				stream.put('0');

			return;
		}

		static constexpr auto value_flags_mask =
			value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;
		format &= value_flags_mask;

		int base = 10;
		if (format != value_flags::none && val > T{})
		{
			switch (format)
			{
				case value_flags::format_as_binary: base = 2; break;
				case value_flags::format_as_octal: base = 8; break;
				case value_flags::format_as_hexadecimal: base = 16; break;
				default: break;
			}
		}

#if TOML_INT_CHARCONV

		char buf[(sizeof(T) * CHAR_BIT)];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val, base);
		const auto len = static_cast<size_t>(res.ptr - buf);
		for (size_t i = len; i < min_digits; i++)
			stream.put('0');
		if (base == 16)
		{
			for (size_t i = 0; i < len; i++)
				if (buf[i] >= 'a')
					buf[i] -= 32;
		}
		impl::print_to_stream(stream, buf, len);

#else

		using unsigned_type = std::conditional_t<(sizeof(T) > sizeof(unsigned)), std::make_unsigned_t<T>, unsigned>;
		using cast_type		= std::conditional_t<std::is_signed_v<T>, std::make_signed_t<unsigned_type>, unsigned_type>;

		if (base == 2)
		{
			const auto len = sizeof(T) * CHAR_BIT;
			for (size_t i = len; i < min_digits; i++)
				stream.put('0');

			bool found_one	   = false;
			const auto v	   = static_cast<unsigned_type>(val);
			unsigned_type mask = unsigned_type{ 1 } << (len - 1u);
			for (size_t i = 0; i < len; i++)
			{
				if ((v & mask))
				{
					stream.put('1');
					found_one = true;
				}
				else if (found_one)
					stream.put('0');
				mask >>= 1;
			}
		}
		else
		{
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			ss << std::uppercase << std::setbase(base);
			if (min_digits)
				ss << std::setfill('0') << std::setw(static_cast<int>(min_digits));
			ss << static_cast<cast_type>(val);
			const auto str = std::move(ss).str();
			impl::print_to_stream(stream, str);
		}

#endif
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_floating_point_to_stream(std::ostream & stream,
										T val,
										value_flags format,
										[[maybe_unused]] bool relaxed_precision)
	{
		switch (impl::fpclassify(val))
		{
			case impl::fp_class::neg_inf: impl::print_to_stream(stream, "-inf"sv); break;

			case impl::fp_class::pos_inf: impl::print_to_stream(stream, "inf"sv); break;

			case impl::fp_class::nan: impl::print_to_stream(stream, "nan"sv); break;

			case impl::fp_class::ok:
			{
				static constexpr auto needs_decimal_point = [](auto&& s) noexcept
				{
					for (auto c : s)
						if (c == '.' || c == 'E' || c == 'e')
							return false;
					return true;
				};

#if TOML_FLOAT_CHARCONV

				const auto hex = !!(format & value_flags::format_as_hexadecimal);
				char buf[charconv_buffer_length<T>];
				auto res = hex ? std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::hex)
							   : std::to_chars(buf, buf + sizeof(buf), val);
				auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };

				char buf2[charconv_buffer_length<T>];
				if (!hex && relaxed_precision)
				{
					res				= std::to_chars(buf2, buf2 + sizeof(buf2), val, std::chars_format::general, 6);
					const auto str2 = std::string_view{ buf2, static_cast<size_t>(res.ptr - buf2) };
					if (str2.length() < str.length())
						str = str2;
				}

				impl::print_to_stream(stream, str);
				if (!hex && needs_decimal_point(str))
					toml::impl::print_to_stream(stream, ".0"sv);

#else

				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				if (!relaxed_precision)
					ss.precision(std::numeric_limits<T>::max_digits10);
				if (!!(format & value_flags::format_as_hexadecimal))
					ss << std::hexfloat;
				ss << val;
				const auto str = std::move(ss).str();
				impl::print_to_stream(stream, str);
				if (!(format & value_flags::format_as_hexadecimal) && needs_decimal_point(str))
					impl::print_to_stream(stream, ".0"sv);

#endif
			}
			break;

			default: TOML_UNREACHABLE;
		}
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(nonnull)
	void print_to_stream(std::ostream & stream, const char* val, size_t len)
	{
		stream.write(val, static_cast<std::streamsize>(len));
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, std::string_view val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const std::string& val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, char val)
	{
		stream.put(val);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, int8_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, int16_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, int32_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, int64_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, uint8_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, uint16_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, uint32_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, uint64_t val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, float val, value_flags format, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, format, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, double val, value_flags format, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, format, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, bool val)
	{
		print_to_stream(stream, val ? "true"sv : "false"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const toml::date& val)
	{
		print_to_stream(stream, val.year, {}, 4);
		stream.put('-');
		print_to_stream(stream, val.month, {}, 2);
		stream.put('-');
		print_to_stream(stream, val.day, {}, 2);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const toml::time& val)
	{
		print_to_stream(stream, val.hour, {}, 2);
		stream.put(':');
		print_to_stream(stream, val.minute, {}, 2);
		stream.put(':');
		print_to_stream(stream, val.second, {}, 2);
		if (val.nanosecond && val.nanosecond <= 999999999u)
		{
			stream.put('.');
			auto ns		  = val.nanosecond;
			size_t digits = 9u;
			while (ns % 10u == 0u)
			{
				ns /= 10u;
				digits--;
			}
			print_to_stream(stream, ns, {}, digits);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const toml::time_offset& val)
	{
		if (!val.minutes)
		{
			stream.put('Z');
			return;
		}

		auto mins = static_cast<int>(val.minutes);
		if (mins < 0)
		{
			stream.put('-');
			mins = -mins;
		}
		else
			stream.put('+');
		const auto hours = mins / 60;
		if (hours)
		{
			print_to_stream(stream, static_cast<unsigned int>(hours), {}, 2);
			mins -= hours * 60;
		}
		else
			print_to_stream(stream, "00"sv);
		stream.put(':');
		print_to_stream(stream, static_cast<unsigned int>(mins), {}, 2);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const toml::date_time& val)
	{
		print_to_stream(stream, val.date);
		stream.put('T');
		print_to_stream(stream, val.time);
		if (val.offset)
			print_to_stream(stream, *val.offset);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const source_position& val)
	{
		print_to_stream(stream, "line "sv);
		print_to_stream(stream, val.line);
		print_to_stream(stream, ", column "sv);
		print_to_stream(stream, val.column);
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const source_region& val)
	{
		print_to_stream(stream, val.begin);
		if (val.path)
		{
			print_to_stream(stream, " of '"sv);
			print_to_stream(stream, *val.path);
			stream.put('\'');
		}
	}

#if TOML_ENABLE_FORMATTERS

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const array& arr)
	{
		stream << toml_formatter{ arr };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const table& tbl)
	{
		stream << toml_formatter{ tbl };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<std::string>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<int64_t>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<double>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<bool>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<date>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<time>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void print_to_stream(std::ostream & stream, const value<date_time>& val)
	{
		stream << toml_formatter{ val };
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/node.inl  *********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node::node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::~node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::node(node && other) noexcept //
		: source_{ std::exchange(other.source_, {}) }
	{}

	TOML_EXTERNAL_LINKAGE
	node::node(const node& /*other*/) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(const node& /*rhs*/) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577

		source_ = {};
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(node&& rhs) noexcept
	{
		if (&rhs != this)
			source_ = std::exchange(rhs.source_, {});
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::string_view path) noexcept
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::string_view path) const noexcept
	{
		return toml::at_path(*this, path);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::wstring_view path)
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::wstring_view path) const
	{
		return toml::at_path(*this, path);
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool node_deep_equality(const node* lhs, const node* rhs) noexcept
	{
		// both same or both null
		if (lhs == rhs)
			return true;

		// lhs null != rhs null or different types
		if ((!lhs != !rhs) || lhs->type() != rhs->type())
			return false;

		bool same;
		lhs->visit(
			[=, &same](auto& l) noexcept
			{
				using concrete_type = remove_cvref<decltype(l)>;

				same = (l == *(rhs->as<concrete_type>()));
			});

		return same;
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/at_path.inl  ******************************************************************************************

TOML_DISABLE_WARNINGS;
#if TOML_INT_CHARCONV
#include <charconv>
#else
#include <sstream>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	node* get_at_path(node & root, std::string_view path) noexcept
	{
		if (root.is_value()) // values don't have child nodes
			return nullptr;

		size_t pos					= 0;
		const auto end				= path.length();
		node* current				= &root;
		bool prev_was_array_indexer = false;
		bool prev_was_dot			= root.is_table(); // implicit '.' at the start for tables

		while (pos < end && current)
		{
			// start of an array indexer
			if (path[pos] == '[')
			{
				const auto current_array = current->as<array>();
				if (!current_array)
					return nullptr;

				// get array index substring
				const auto index_start = pos + 1u;					  // first position after '['
				const auto index_end   = path.find(']', index_start); // position of ']'
				if (index_end == std::string_view::npos || index_end == index_start)
					return nullptr;
				auto index_str = path.substr(index_start, index_end - index_start);

				// trim whitespace from either side of the index
				const auto first_non_ws = index_str.find_first_not_of(" \t"sv);
				const auto last_non_ws	= index_str.find_last_not_of(" \t"sv);
				if (first_non_ws == std::string_view::npos)
					return nullptr;
				TOML_ASSERT_ASSUME(last_non_ws != std::string_view::npos);
				index_str = index_str.substr(first_non_ws, (last_non_ws - first_non_ws) + 1u);

				// parse the actual array index
				size_t index;
				if (index_str.length() == 1u && index_str[0] >= '0' && index_str[0] <= '9')
					index = static_cast<size_t>(index_str[0] - '0');
				else
				{
#if TOML_INT_CHARCONV

					auto fc_result = std::from_chars(index_str.data(), index_str.data() + index_str.length(), index);
					if (fc_result.ec != std::errc{})
						return nullptr;

#else

					std::stringstream ss;
					ss.imbue(std::locale::classic());
					ss.write(index_str.data(), static_cast<std::streamsize>(index_str.length()));
					if (!(ss >> index))
						return nullptr;

#endif
				}

				current				   = current_array->get(index);
				pos					   = index_end + 1u;
				prev_was_dot		   = false;
				prev_was_array_indexer = true;
			}

			// start of a new table child
			else if (path[pos] == '.')
			{
				const auto current_table = current->as<table>();
				if (!current_table)
					return nullptr;

				// a dot immediately following another dot (or at the beginning of the string) is as if we'd asked
				// for an empty child in between, e.g.
				//
				//     foo..bar
				//
				// is equivalent to
				//
				//     "foo".""."bar"
				//
				if (prev_was_dot)
					current = current_table->get(""sv);

				pos++;
				prev_was_dot		   = true;
				prev_was_array_indexer = false;
			}

			// some regular subkey
			else
			{
				// get subkey text
				const auto subkey_start = pos;
				const auto subkey_len =
					impl::min(path.find_first_of(".["sv, subkey_start + 1u), path.length()) - subkey_start;
				const auto subkey = path.substr(subkey_start, subkey_len);

				// a regular subkey segment immediately after an array indexer is OK if it was all whitespace, e.g.:
				//
				//     "foo[0]  .bar"
				//            ^^ skip this
				//
				// otherwise its an error (since it would have to be preceeded by a dot)
				if (prev_was_array_indexer)
				{
					auto non_ws = subkey.find_first_not_of(" \t");
					if (non_ws == std::string_view::npos)
					{
						pos += subkey_len;
						prev_was_dot		   = false;
						prev_was_array_indexer = false;
						continue;
					}
					else
						return nullptr;
				}

				const auto current_table = current->as<table>();
				if (!current_table)
					return nullptr;

				current = current_table->get(subkey);
				pos += subkey_len;
				prev_was_dot		   = false;
				prev_was_array_indexer = false;
			}
		}

		// a dot at the end is as if we'd asked for an empty child at the end, e.g.
		//
		//     foo.bar.
		//
		// is equivalent to
		//
		//     "foo"."bar".""
		//
		if (current && prev_was_dot)
		{
			const auto current_table = current->as<table>();
			if (!current_table)
				return nullptr;

			current = current_table->get(""sv);
		}

		return current;
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_INTERNAL_LINKAGE
	node* get_at_path(node & root, std::wstring_view path) noexcept
	{
		if (auto tbl = root.as_table(); tbl && tbl->empty())
			return {};
		if (auto arr = root.as_array(); arr && arr->empty())
			return {};

		return get_at_path(root, impl::narrow(path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}

TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node_view<node> at_path(node & root, std::string_view path) noexcept
	{
		return node_view<node>{ TOML_ANON_NAMESPACE::get_at_path(root, path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> at_path(const node& root, std::string_view path) noexcept
	{
		return node_view<const node>{ TOML_ANON_NAMESPACE::get_at_path(const_cast<node&>(root), path) };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> at_path(node & root, std::wstring_view path)
	{
		return node_view<node>{ TOML_ANON_NAMESPACE::get_at_path(root, path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> at_path(const node& root, std::wstring_view path)
	{
		return node_view<const node>{ TOML_ANON_NAMESPACE::get_at_path(const_cast<node&>(root), path) };
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/array.inl  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	array::array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::~array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const impl::array_init_elem* b, const impl::array_init_elem* e)
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif

		TOML_ASSERT_ASSUME(b);
		TOML_ASSERT_ASSUME(e);
		TOML_ASSERT_ASSUME(b <= e);

		if TOML_UNLIKELY(b == e)
			return;

		size_t cap{};
		for (auto it = b; it != e; it++)
		{
			if (it->value)
				cap++;
		}
		if TOML_UNLIKELY(!cap)
			return;

		elems_.reserve(cap);
		for (; b != e; b++)
		{
			if (b->value)
				elems_.push_back(std::move(b->value));
		}
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const array& other) //
		: node(other)
	{
		elems_.reserve(other.elems_.size());
		for (const auto& elem : other)
			elems_.emplace_back(impl::make_node(elem));

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(array && other) noexcept //
		: node(std::move(other)),
		  elems_(std::move(other.elems_))
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(const array& rhs)
	{
		if (&rhs != this)
		{
			node::operator=(rhs);
			elems_.clear();
			elems_.reserve(rhs.elems_.size());
			for (const auto& elem : rhs)
				elems_.emplace_back(impl::make_node(elem));
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(array&& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(std::move(rhs));
			elems_		  = std::move(rhs.elems_);
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void array::preinsertion_resize(size_t idx, size_t count)
	{
		TOML_ASSERT(idx <= elems_.size());
		TOML_ASSERT_ASSUME(count >= 1u);
		const auto old_size			= elems_.size();
		const auto new_size			= old_size + count;
		const auto inserting_at_end = idx == old_size;
		elems_.resize(new_size);
		if (!inserting_at_end)
		{
			for (size_t left = old_size, right = new_size - 1u; left-- > idx; right--)
				elems_[right] = std::move(elems_[left]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void array::insert_at_back(impl::node_ptr && elem)
	{
		elems_.push_back(std::move(elem));
	}

	TOML_EXTERNAL_LINKAGE
	array::vector_iterator array::insert_at(const_vector_iterator pos, impl::node_ptr && elem)
	{
		return elems_.insert(pos, std::move(elem));
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype) const noexcept
	{
		if (elems_.empty())
			return false;

		if (ntype == node_type::none)
			ntype = elems_[0]->type();

		for (const auto& val : elems_)
			if (val->type() != ntype)
				return false;

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (elems_.empty())
		{
			first_nonmatch = {};
			return false;
		}
		if (ntype == node_type::none)
			ntype = elems_[0]->type();
		for (const auto& val : elems_)
		{
			if (val->type() != ntype)
			{
				first_nonmatch = val.get();
				return false;
			}
		}
		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* fnm		  = nullptr;
		const auto result = const_cast<array&>(*this).is_homogeneous(ntype, fnm);
		first_nonmatch	  = fnm;
		return result;
	}

	TOML_EXTERNAL_LINKAGE
	node& array::at(size_t index)
	{
#if TOML_COMPILER_EXCEPTIONS

		return *elems_.at(index);

#else

		auto n = get(index);
		TOML_ASSERT_ASSUME(n && "element index not found in array!");
		return *n;

#endif
	}

	TOML_EXTERNAL_LINKAGE
	void array::reserve(size_t new_capacity)
	{
		elems_.reserve(new_capacity);
	}

	TOML_EXTERNAL_LINKAGE
	void array::shrink_to_fit()
	{
		elems_.shrink_to_fit();
	}

	TOML_EXTERNAL_LINKAGE
	void array::truncate(size_t new_size)
	{
		if (new_size < elems_.size())
			elems_.resize(new_size);
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ pos }) };
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ first }, const_vector_iterator{ last }) };
	}

	TOML_EXTERNAL_LINKAGE
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (size_t i = 0, e = elems_.size(); i < e; i++)
		{
			auto arr = elems_[i]->as_array();
			leaves += arr ? arr->total_leaf_count() : size_t{ 1 };
		}
		return leaves;
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten_child(array && child, size_t & dest_index) noexcept
	{
		for (size_t i = 0, e = child.size(); i < e; i++)
		{
			auto type = child.elems_[i]->type();
			if (type == node_type::array)
			{
				array& arr = *reinterpret_cast<array*>(child.elems_[i].get());
				if (!arr.empty())
					flatten_child(std::move(arr), dest_index);
			}
			else
				elems_[dest_index++] = std::move(child.elems_[i]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	array& array::flatten()&
	{
		if (elems_.empty())
			return *this;

		bool requires_flattening	 = false;
		size_t size_after_flattening = elems_.size();
		for (size_t i = elems_.size(); i-- > 0u;)
		{
			auto arr = elems_[i]->as_array();
			if (!arr)
				continue;
			size_after_flattening--; // discount the array itself
			const auto leaf_count = arr->total_leaf_count();
			if (leaf_count > 0u)
			{
				requires_flattening = true;
				size_after_flattening += leaf_count;
			}
			else
				elems_.erase(elems_.cbegin() + static_cast<ptrdiff_t>(i));
		}

		if (!requires_flattening)
			return *this;

		elems_.reserve(size_after_flattening);

		size_t i = 0;
		while (i < elems_.size())
		{
			auto arr = elems_[i]->as_array();
			if (!arr)
			{
				i++;
				continue;
			}

			impl::node_ptr arr_storage = std::move(elems_[i]);
			const auto leaf_count	   = arr->total_leaf_count();
			if (leaf_count > 1u)
				preinsertion_resize(i + 1u, leaf_count - 1u);
			flatten_child(std::move(*arr), i); // increments i
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	array& array::prune(bool recursive)& noexcept
	{
		if (elems_.empty())
			return *this;

		for (size_t i = elems_.size(); i-- > 0u;)
		{
			if (auto arr = elems_[i]->as_array())
			{
				if (recursive)
					arr->prune(true);
				if (arr->empty())
					elems_.erase(elems_.cbegin() + static_cast<ptrdiff_t>(i));
			}
			else if (auto tbl = elems_[i]->as_table())
			{
				if (recursive)
					tbl->prune(true);
				if (tbl->empty())
					elems_.erase(elems_.cbegin() + static_cast<ptrdiff_t>(i));
			}
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void array::pop_back() noexcept
	{
		elems_.pop_back();
	}

	TOML_EXTERNAL_LINKAGE
	void array::clear() noexcept
	{
		elems_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	bool array::equal(const array& lhs, const array& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.elems_.size() != rhs.elems_.size())
			return false;
		for (size_t i = 0, e = lhs.elems_.size(); i < e; i++)
		{
			const auto lhs_type = lhs.elems_[i]->type();
			const node& rhs_	= *rhs.elems_[i];
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = lhs.elems_[i]->visit(
				[&](const auto& lhs_) noexcept
				{ return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_); });
			if (!equal)
				return false;
		}
		return true;
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/table.inl  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	table::table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::~table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const impl::table_init_pair* b, const impl::table_init_pair* e)
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif

		TOML_ASSERT_ASSUME(b);
		TOML_ASSERT_ASSUME(e);
		TOML_ASSERT_ASSUME(b <= e);

		if TOML_UNLIKELY(b == e)
			return;

		for (; b != e; b++)
		{
			if (!b->value) // empty node_views
				continue;

			map_.insert_or_assign(std::move(b->key), std::move(b->value));
		}
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const table& other) //
		: node(other),
		  inline_{ other.inline_ }
	{
		for (auto&& [k, v] : other.map_)
			map_.emplace_hint(map_.end(), k, impl::make_node(*v));

#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(table && other) noexcept //
		: node(std::move(other)),
		  map_{ std::move(other.map_) },
		  inline_{ other.inline_ }
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(const table& rhs)
	{
		if (&rhs != this)
		{
			node::operator=(rhs);
			map_.clear();
			for (auto&& [k, v] : rhs.map_)
				map_.emplace_hint(map_.end(), k, impl::make_node(*v));
			inline_ = rhs.inline_;
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(table&& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(std::move(rhs));
			map_		  = std::move(rhs.map_);
			inline_		  = rhs.inline_;
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype) const noexcept
	{
		if (map_.empty())
			return false;

		if (ntype == node_type::none)
			ntype = map_.cbegin()->second->type();

		for (auto&& [k, v] : map_)
		{
			TOML_UNUSED(k);
			if (v->type() != ntype)
				return false;
		}

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (map_.empty())
		{
			first_nonmatch = {};
			return false;
		}
		if (ntype == node_type::none)
			ntype = map_.cbegin()->second->type();
		for (const auto& [k, v] : map_)
		{
			TOML_UNUSED(k);
			if (v->type() != ntype)
			{
				first_nonmatch = v.get();
				return false;
			}
		}
		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* fnm		  = nullptr;
		const auto result = const_cast<table&>(*this).is_homogeneous(ntype, fnm);
		first_nonmatch	  = fnm;
		return result;
	}

	TOML_EXTERNAL_LINKAGE
	node* table::get(std::string_view key) noexcept
	{
		if (auto it = map_.find(key); it != map_.end())
			return it->second.get();
		return nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	node& table::at(std::string_view key)
	{
		auto n = get(key);

#if TOML_COMPILER_EXCEPTIONS

		if (!n)
		{
			auto err = "key '"s;
			err.append(key);
			err.append("' not found in table"sv);
			throw std::out_of_range{ err };
		}

#else

		TOML_ASSERT_ASSUME(n && "key not found in table!");

#endif

		return *n;
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::get_lower_bound(std::string_view key) noexcept
	{
		return map_.lower_bound(key);
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::find(std::string_view key) noexcept
	{
		return iterator{ map_.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	table::const_iterator table::find(std::string_view key) const noexcept
	{
		return const_iterator{ map_.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator pos) noexcept
	{
		return map_.erase(pos);
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator begin, const_map_iterator end) noexcept
	{
		return map_.erase(begin, end);
	}

	TOML_EXTERNAL_LINKAGE
	size_t table::erase(std::string_view key) noexcept
	{
		if (auto it = map_.find(key); it != map_.end())
		{
			map_.erase(it);
			return size_t{ 1 };
		}
		return size_t{};
	}

	TOML_EXTERNAL_LINKAGE
	table& table::prune(bool recursive)& noexcept
	{
		if (map_.empty())
			return *this;

		for (auto it = map_.begin(); it != map_.end();)
		{
			if (auto arr = it->second->as_array())
			{
				if (recursive)
					arr->prune(true);

				if (arr->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
			else if (auto tbl = it->second->as_table())
			{
				if (recursive)
					tbl->prune(true);

				if (tbl->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
			it++;
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void table::clear() noexcept
	{
		map_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::insert_with_hint(const_iterator hint, key && k, impl::node_ptr && v)
	{
		return map_.emplace_hint(const_map_iterator{ hint }, std::move(k), std::move(v));
	}

	TOML_EXTERNAL_LINKAGE
	bool table::equal(const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.map_.size() != rhs.map_.size())
			return false;

		for (auto l = lhs.map_.begin(), r = rhs.map_.begin(), e = lhs.map_.end(); l != e; l++, r++)
		{
			if (l->first != r->first)
				return false;

			const auto lhs_type = l->second->type();
			const node& rhs_	= *r->second;
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = l->second->visit(
				[&](const auto& lhs_) noexcept
				{ return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_); });
			if (!equal)
				return false;
		}
		return true;
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/unicode.inl  ******************************************************************************************

#if TOML_ENABLE_SIMD

#if defined(__SSE2__)                                                                                                  \
	|| (defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)))
#define TOML_HAS_SSE2 1
#endif

#if defined(__SSE4_1__) || (defined(_MSC_VER) && (defined(__AVX__) || defined(__AVX2__)))
#define TOML_HAS_SSE4_1 1
#endif

#endif // TOML_ENABLE_SIMD

#ifndef TOML_HAS_SSE2
#define TOML_HAS_SSE2 0
#endif
#ifndef TOML_HAS_SSE4_1
#define TOML_HAS_SSE4_1 0
#endif

TOML_DISABLE_WARNINGS;
#if TOML_HAS_SSE4_1
#include <smmintrin.h>
#endif
#if TOML_HAS_SSE2
#include <emmintrin.h>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool is_ascii(const char* str, size_t len) noexcept
	{
		const char* const end = str + len;

#if TOML_HAS_SSE2 && (128 % CHAR_BIT) == 0
		{
			constexpr size_t chars_per_vector = 128u / CHAR_BIT;

			if (const size_t simdable = len - (len % chars_per_vector))
			{
				__m128i mask = _mm_setzero_si128();
				for (const char* const e = str + simdable; str < e; str += chars_per_vector)
				{
					const __m128i current_bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str));
					mask						= _mm_or_si128(mask, current_bytes);
				}
				const __m128i has_error = _mm_cmpgt_epi8(_mm_setzero_si128(), mask);

#if TOML_HAS_SSE4_1
				if (!_mm_testz_si128(has_error, has_error))
					return false;
#else
				if (_mm_movemask_epi8(_mm_cmpeq_epi8(has_error, _mm_setzero_si128())) != 0xFFFF)
					return false;
#endif
			}
		}
#endif

		for (; str < end; str++)
			if (static_cast<unsigned char>(*str) > 127u)
				return false;

		return true;
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

//********  impl/parser.inl  *******************************************************************************************

#if TOML_ENABLE_PARSER

TOML_DISABLE_WARNINGS;
#include <istream>
#include <fstream>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
#include <sstream>
#endif
#if !TOML_INT_CHARCONV
#include <iomanip>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	class utf8_byte_stream;

	TOML_INTERNAL_LINKAGE
	constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	template <typename Char>
	class utf8_byte_stream<std::basic_string_view<Char>>
	{
		static_assert(sizeof(Char) == 1);

	  private:
		std::basic_string_view<Char> source_;
		size_t position_ = {};

	  public:
		TOML_NODISCARD_CTOR
		explicit constexpr utf8_byte_stream(std::basic_string_view<Char> sv) noexcept //
			: source_{ sv }
		{
			// skip bom
			if (source_.length() >= 3u && memcmp(utf8_byte_order_mark.data(), source_.data(), 3u) == 0)
				position_ += 3u;
		}

		TOML_CONST_INLINE_GETTER
		constexpr bool error() const noexcept
		{
			return false;
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool eof() const noexcept
		{
			return position_ >= source_.length();
		}

		TOML_PURE_INLINE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return !eof();
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool peek_eof() const noexcept
		{
			return eof();
		}

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(void* dest, size_t num) noexcept
		{
			TOML_ASSERT_ASSUME(!eof());

			num = impl::min(position_ + num, source_.length()) - position_;
			std::memcpy(dest, source_.data() + position_, num);
			position_ += num;
			return num;
		}
	};

	template <>
	class utf8_byte_stream<std::istream>
	{
	  private:
		std::istream* source_;

	  public:
		TOML_NODISCARD_CTOR
		explicit utf8_byte_stream(std::istream& stream) noexcept(!TOML_COMPILER_EXCEPTIONS) //
			: source_{ &stream }
		{
			if (!*this) // eof, bad
				return;

			const auto initial_pos = source_->tellg();
			char bom[3];
			source_->read(bom, 3);
			if (source_->bad() || (source_->gcount() == 3 && memcmp(utf8_byte_order_mark.data(), bom, 3u) == 0))
				return;

			source_->clear();
			source_->seekg(initial_pos, std::istream::beg);
		}

		TOML_PURE_INLINE_GETTER
		bool error() const noexcept
		{
			return !!(source_->rdstate() & std::istream::badbit);
		}

		TOML_PURE_INLINE_GETTER
		bool eof() const noexcept
		{
			return !!(source_->rdstate() & std::istream::eofbit);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return !(source_->rdstate() & (std::istream::badbit | std::istream::eofbit));
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			return eof() || source_->peek() == std::istream::traits_type::eof();
		}

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(void* dest, size_t num) noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			TOML_ASSERT(*this);

			source_->read(static_cast<char*>(dest), static_cast<std::streamsize>(num));
			return static_cast<size_t>(source_->gcount());
		}
	};

	struct utf8_codepoint
	{
		char32_t value;
		char bytes[4];
		size_t count;
		source_position position;

		TOML_PURE_INLINE_GETTER
		constexpr operator const char32_t&() const noexcept
		{
			return value;
		}

		TOML_PURE_INLINE_GETTER
		constexpr const char32_t& operator*() const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	struct TOML_ABSTRACT_BASE utf8_reader_interface
	{
		TOML_NODISCARD
		virtual const source_path_ptr& source_path() const noexcept = 0;

		TOML_NODISCARD
		virtual const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS) = 0;

		TOML_NODISCARD
		virtual bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS) = 0;

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		virtual optional<parse_error>&& error() noexcept = 0;

#endif

		virtual ~utf8_reader_interface() noexcept = default;
	};

#if TOML_EXCEPTIONS
#define utf8_reader_error(...)				throw parse_error(__VA_ARGS__)
#define utf8_reader_return_after_error(...) static_assert(true)
#define utf8_reader_error_check(...)		static_assert(true)
#else
#define utf8_reader_error(...)				err_.emplace(__VA_ARGS__)
#define utf8_reader_return_after_error(...) return __VA_ARGS__
#define utf8_reader_error_check(...)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(err_)                                                                                         \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#endif

#if defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__) // because, honestly, what the fuck macOS & MinGW??
#define TOML_OVERALIGNED
#else
#define TOML_OVERALIGNED alignas(32)
#endif

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final : public utf8_reader_interface
	{
	  private:
		static constexpr size_t block_capacity = 32;
		utf8_byte_stream<T> stream_;
		source_position next_pos_ = { 1, 1 };

		impl::utf8_decoder decoder_;
		struct currently_decoding_t
		{
			char bytes[4];
			size_t count;
		} currently_decoding_;

		struct codepoints_t
		{
			TOML_OVERALIGNED utf8_codepoint buffer[block_capacity];
			size_t current;
			size_t count;
		} codepoints_;

		source_path_ptr source_path_;

#if !TOML_EXCEPTIONS
		optional<parse_error> err_;
#endif

		bool read_next_block() noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			TOML_ASSERT(stream_);

			TOML_OVERALIGNED char raw_bytes[block_capacity];
			size_t raw_bytes_read;

			// read the next raw (encoded) block in from the stream
			if constexpr (noexcept(stream_(raw_bytes, block_capacity)) || !TOML_EXCEPTIONS)
			{
				raw_bytes_read = stream_(raw_bytes, block_capacity);
			}
#if TOML_EXCEPTIONS
			else
			{
				try
				{
					raw_bytes_read = stream_(raw_bytes, block_capacity);
				}
				catch (const std::exception& exc)
				{
					throw parse_error{ exc.what(), next_pos_, source_path_ };
				}
				catch (...)
				{
					throw parse_error{ "An unspecified error occurred", next_pos_, source_path_ };
				}
			}
#endif // TOML_EXCEPTIONS

			// handle a zero-byte read
			if TOML_UNLIKELY(!raw_bytes_read)
			{
				if (stream_.eof())
				{
					// EOF only sets the error state if the decoder wants more input, otherwise
					// a zero-byte read might have just caused the underlying stream to realize it's exhaused and set
					// the EOF flag, and that's totally fine
					if (decoder_.needs_more_input())
						utf8_reader_error("Encountered EOF during incomplete utf-8 code point sequence",
										  next_pos_,
										  source_path_);
				}
				else
				{
					utf8_reader_error("Reading from the underlying stream failed - zero bytes read",
									  next_pos_,
									  source_path_);
				}
				return false;
			}

			TOML_ASSERT_ASSUME(raw_bytes_read);
			std::memset(&codepoints_, 0, sizeof(codepoints_));

			// helper for calculating decoded codepoint line+cols
			const auto calc_positions = [&]() noexcept
			{
				for (size_t i = 0; i < codepoints_.count; i++)
				{
					auto& cp	= codepoints_.buffer[i];
					cp.position = next_pos_;

					if (cp == U'\n')
					{
						next_pos_.line++;
						next_pos_.column = source_index{ 1 };
					}
					else
						next_pos_.column++;
				}
			};

			// decide whether we need to use the UTF-8 decoder or if we can treat this block as plain ASCII
			const auto ascii_fast_path = !decoder_.needs_more_input() && impl::is_ascii(raw_bytes, raw_bytes_read);

			// ASCII fast-path
			if (ascii_fast_path)
			{
				decoder_.reset();
				currently_decoding_.count = {};

				codepoints_.count = raw_bytes_read;
				for (size_t i = 0; i < codepoints_.count; i++)
				{
					auto& cp	= codepoints_.buffer[i];
					cp.value	= static_cast<char32_t>(raw_bytes[i]);
					cp.bytes[0] = raw_bytes[i];
					cp.count	= 1u;
				}
			}

			// UTF-8 slow-path
			else
			{
				// helper for getting precise error location
				const auto error_pos = [&]() noexcept -> const source_position&
				{ //
					return codepoints_.count ? codepoints_.buffer[codepoints_.count - 1u].position : next_pos_;
				};

				for (size_t i = 0; i < raw_bytes_read; i++)
				{
					decoder_(static_cast<uint8_t>(raw_bytes[i]));
					if TOML_UNLIKELY(decoder_.error())
					{
						calc_positions();
						utf8_reader_error("Encountered invalid utf-8 sequence", error_pos(), source_path_);
						utf8_reader_return_after_error(false);
					}

					currently_decoding_.bytes[currently_decoding_.count++] = raw_bytes[i];

					if (decoder_.has_code_point())
					{
						auto& cp = codepoints_.buffer[codepoints_.count++];

						cp.value = decoder_.codepoint;
						cp.count = currently_decoding_.count;
						std::memcpy(cp.bytes, currently_decoding_.bytes, currently_decoding_.count);
						currently_decoding_.count = {};
					}
					else if TOML_UNLIKELY(currently_decoding_.count == 4u)
					{
						calc_positions();
						utf8_reader_error("Encountered overlong utf-8 sequence", error_pos(), source_path_);
						utf8_reader_return_after_error(false);
					}
				}
				if TOML_UNLIKELY(decoder_.needs_more_input() && stream_.eof())
				{
					calc_positions();
					utf8_reader_error("Encountered EOF during incomplete utf-8 code point sequence",
									  error_pos(),
									  source_path_);
					utf8_reader_return_after_error(false);
				}
			}

			TOML_ASSERT_ASSUME(codepoints_.count);
			calc_positions();

			// handle general I/O errors
			// (down here so the next_pos_ benefits from calc_positions())
			if TOML_UNLIKELY(stream_.error())
			{
				utf8_reader_error("An I/O error occurred while reading from the underlying stream",
								  next_pos_,
								  source_path_);
				utf8_reader_return_after_error(false);
			}

			return true;
		}

	  public:
		template <typename U, typename String = std::string_view>
		TOML_NODISCARD_CTOR
		explicit utf8_reader(U&& source, String&& source_path = {}) noexcept(
			std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
			: stream_{ static_cast<U&&>(source) }
		{
			currently_decoding_.count = {};

			codepoints_.current = {};
			codepoints_.count	= {};

			if (!source_path.empty())
				source_path_ = std::make_shared<const std::string>(static_cast<String&&>(source_path));
		}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept final
		{
			return source_path_;
		}

		TOML_NODISCARD
		const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS) final
		{
			utf8_reader_error_check({});

			if (codepoints_.current == codepoints_.count)
			{
				if TOML_UNLIKELY(!stream_ || !read_next_block())
					return nullptr;

				TOML_ASSERT_ASSUME(!codepoints_.current);
			}
			TOML_ASSERT_ASSUME(codepoints_.count);
			TOML_ASSERT_ASSUME(codepoints_.count <= block_capacity);
			TOML_ASSERT_ASSUME(codepoints_.current < codepoints_.count);

			return &codepoints_.buffer[codepoints_.current++];
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS) final
		{
			return stream_.peek_eof();
		}

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		optional<parse_error>&& error() noexcept final
		{
			return std::move(err_);
		}

#endif
	};

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string_view) -> utf8_reader<std::basic_string_view<Char>>;
	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string &&) -> utf8_reader<std::basic_string_view<Char>>;
	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string_view) -> utf8_reader<std::basic_istream<Char>>;
	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string &&) -> utf8_reader<std::basic_istream<Char>>;

#if TOML_EXCEPTIONS
#define utf8_buffered_reader_error_check(...) static_assert(true)
#else
#define utf8_buffered_reader_error_check(...)                                                                          \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(reader_.error())                                                                              \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#endif

	class TOML_EMPTY_BASES utf8_buffered_reader
	{
	  public:
		static constexpr size_t max_history_length = 128;

	  private:
		static constexpr size_t history_buffer_size = max_history_length - 1; //'head' is stored in the reader
		utf8_reader_interface& reader_;
		struct
		{
			utf8_codepoint buffer[history_buffer_size];
			size_t count, first;
		} history_					= {};
		const utf8_codepoint* head_ = {};
		size_t negative_offset_		= {};

	  public:
		TOML_NODISCARD_CTOR
		explicit utf8_buffered_reader(utf8_reader_interface& reader) noexcept //
			: reader_{ reader }
		{}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept
		{
			return reader_.source_path();
		}

		TOML_NODISCARD
		const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			utf8_buffered_reader_error_check({});

			if (negative_offset_)
			{
				negative_offset_--;

				// an entry negative offset of 1 just means "replay the current head"
				if (!negative_offset_)
					return head_;

				// otherwise step back into the history buffer
				else
					return history_.buffer
						 + ((history_.first + history_.count - negative_offset_) % history_buffer_size);
			}
			else
			{
				// first character read from stream
				if TOML_UNLIKELY(!history_.count && !head_)
					head_ = reader_.read_next();

				// subsequent characters and not eof
				else if (head_)
				{
					if TOML_UNLIKELY(history_.count < history_buffer_size)
						history_.buffer[history_.count++] = *head_;
					else
						history_.buffer[(history_.first++ + history_buffer_size) % history_buffer_size] = *head_;

					head_ = reader_.read_next();
				}

				return head_;
			}
		}

		TOML_NODISCARD
		const utf8_codepoint* step_back(size_t count) noexcept
		{
			utf8_buffered_reader_error_check({});

			TOML_ASSERT_ASSUME(history_.count);
			TOML_ASSERT_ASSUME(negative_offset_ + count <= history_.count);

			negative_offset_ += count;

			return negative_offset_
					 ? history_.buffer + ((history_.first + history_.count - negative_offset_) % history_buffer_size)
					 : head_;
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			return reader_.peek_eof();
		}

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		optional<parse_error>&& error() noexcept
		{
			return reader_.error();
		}

#endif
	};
}
TOML_ANON_NAMESPACE_END;

#if TOML_EXCEPTIONS
#define TOML_RETURNS_BY_THROWING [[noreturn]]
#else
#define TOML_RETURNS_BY_THROWING
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename... T>
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr bool is_match(char32_t codepoint, T... vals) noexcept
	{
		static_assert((std::is_same_v<char32_t, T> && ...));
		return ((codepoint == vals) || ...);
	}

	template <uint64_t>
	struct parse_integer_traits;
	template <>
	struct parse_integer_traits<2>
	{
		static constexpr auto scope_qualifier  = "binary integer"sv;
		static constexpr auto is_digit		   = impl::is_binary_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 63;
		static constexpr auto prefix_codepoint = U'b';
		static constexpr auto prefix		   = "b"sv;
		static constexpr auto full_prefix	   = "0b"sv;
	};
	template <>
	struct parse_integer_traits<8>
	{
		static constexpr auto scope_qualifier  = "octal integer"sv;
		static constexpr auto is_digit		   = impl::is_octal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 21; // strlen("777777777777777777777")
		static constexpr auto prefix_codepoint = U'o';
		static constexpr auto prefix		   = "o"sv;
		static constexpr auto full_prefix	   = "0o"sv;
	};
	template <>
	struct parse_integer_traits<10>
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto is_digit		  = impl::is_decimal_digit;
		static constexpr auto is_signed		  = true;
		static constexpr auto max_digits	  = 19; // strlen("9223372036854775807")
		static constexpr auto full_prefix	  = ""sv;
	};
	template <>
	struct parse_integer_traits<16>
	{
		static constexpr auto scope_qualifier  = "hexadecimal integer"sv;
		static constexpr auto is_digit		   = impl::is_hexadecimal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 16; // strlen("7FFFFFFFFFFFFFFF")
		static constexpr auto prefix_codepoint = U'x';
		static constexpr auto prefix		   = "x"sv;
		static constexpr auto full_prefix	   = "0x"sv;
	};

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(node_type val) noexcept
	{
		return impl::node_type_friendly_names[impl::unwrap_enum(val)];
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const std::string& str) noexcept
	{
		return std::string_view{ str };
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(bool val) noexcept
	{
		using namespace std::string_view_literals;

		return val ? "true"sv : "false"sv;
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint& cp) noexcept
	{
		if (cp.value <= U'\x1F')
			return impl::control_char_escapes[cp.value];
		else if (cp.value == U'\x7F')
			return "\\u007F"sv;
		else
			return std::string_view{ cp.bytes, cp.count };
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint* cp) noexcept
	{
		if (cp)
			return to_sv(*cp);
		return ""sv;
	}

	struct escaped_codepoint
	{
		const utf8_codepoint& cp;
	};

	template <typename T>
	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, char* const buf_end, const T& arg) noexcept
	{
		if TOML_UNLIKELY(write_pos >= buf_end)
			return;

		using arg_type = impl::remove_cvref<T>;

		// string views
		if constexpr (std::is_same_v<arg_type, std::string_view>)
		{
			const auto max_chars = static_cast<size_t>(buf_end - write_pos);
			const auto len		 = max_chars < arg.length() ? max_chars : arg.length();
			std::memcpy(write_pos, arg.data(), len);
			write_pos += len;
		}

		// doubles
		else if constexpr (std::is_same_v<arg_type, double>)
		{
#if TOML_FLOAT_CHARCONV
			const auto result = std::to_chars(write_pos, buf_end, arg);
			write_pos		  = result.ptr;
#else
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			ss.precision(std::numeric_limits<arg_type>::max_digits10);
			ss << arg;
			concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
#endif
		}

		// 64-bit integers
		else if constexpr (impl::is_one_of<arg_type, int64_t, uint64_t>)
		{
#if TOML_INT_CHARCONV
			const auto result = std::to_chars(write_pos, buf_end, arg);
			write_pos		  = result.ptr;
#else
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			using cast_type = std::conditional_t<std::is_signed_v<arg_type>, int64_t, uint64_t>;
			ss << static_cast<cast_type>(arg);
			concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
#endif
		}

		// escaped_codepoint
		else if constexpr (std::is_same_v<arg_type, escaped_codepoint>)
		{
			if (arg.cp.value <= U'\x7F')
				concatenate(write_pos, buf_end, to_sv(arg.cp));
			else
			{
				auto val			= static_cast<uint_least32_t>(arg.cp.value);
				const auto digits	= val > 0xFFFFu ? 8u : 4u;
				constexpr auto mask = uint_least32_t{ 0xFu };
				char buf[10]		= { '\\', digits > 4 ? 'U' : 'u' };
				for (auto i = 2u + digits; i-- > 2u;)
				{
					const auto hexdig = val & mask;
					buf[i]			  = static_cast<char>(hexdig >= 0xAu ? ('A' + (hexdig - 0xAu)) : ('0' + hexdig));
					val >>= 4;
				}
				concatenate(write_pos, buf_end, std::string_view{ buf, digits + 2u });
			}
		}

		// all other floats (fallback - coerce to double)
		else if constexpr (std::is_floating_point_v<arg_type>)
			concatenate(write_pos, buf_end, static_cast<double>(arg));

		// all other integers (fallback - coerce to (u)int64_t)
		else if constexpr (std::is_arithmetic_v<arg_type> && std::is_integral_v<arg_type>)
		{
			using cast_type = std::conditional_t<std::is_unsigned_v<arg_type>, uint64_t, int64_t>;
			concatenate(write_pos, buf_end, static_cast<cast_type>(arg));
		}

		else
		{
			static_assert(
				impl::dependent_false<T>,
				"concatenate() inputs are limited to std::string_views, integers, floats, and escaped_codepoint");
		}
	}

	struct error_builder
	{
		static constexpr std::size_t buf_size = 512;
		char buf[buf_size];
		char* write_pos			  = buf;
		char* const max_write_pos = buf + (buf_size - std::size_t{ 1 }); // allow for null terminator

		TOML_NODISCARD_CTOR
		error_builder(std::string_view scope) noexcept
		{
			concatenate(write_pos, max_write_pos, "Error while parsing "sv);
			concatenate(write_pos, max_write_pos, scope);
			concatenate(write_pos, max_write_pos, ": "sv);
		}

		template <typename T>
		void append(const T& arg) noexcept
		{
			concatenate(write_pos, max_write_pos, arg);
		}

		TOML_RETURNS_BY_THROWING
		auto finish(const source_position& pos, const source_path_ptr& source_path) const
		{
			*write_pos = '\0';

#if TOML_EXCEPTIONS
			throw parse_error{ buf, pos, source_path };
#else
			return parse_error{ std::string(buf, static_cast<size_t>(write_pos - buf)), pos, source_path };
#endif
		}

		TOML_DELETE_DEFAULTS(error_builder);
	};

	struct parse_scope
	{
		std::string_view& storage_;
		std::string_view parent_;

		TOML_NODISCARD_CTOR
		explicit parse_scope(std::string_view& current_scope, std::string_view new_scope) noexcept
			: storage_{ current_scope },
			  parent_{ current_scope }
		{
			storage_ = new_scope;
		}

		~parse_scope() noexcept
		{
			storage_ = parent_;
		}

		TOML_DELETE_DEFAULTS(parse_scope);
	};
#define push_parse_scope_2(scope, line) parse_scope ps_##line(current_scope, scope)
#define push_parse_scope_1(scope, line) push_parse_scope_2(scope, line)
#define push_parse_scope(scope)			push_parse_scope_1(scope, __LINE__)

	struct parse_key_buffer
	{
		std::string buffer;
		std::vector<std::pair<size_t, size_t>> segments;
		std::vector<source_position> starts;
		std::vector<source_position> ends;

		void clear() noexcept
		{
			buffer.clear();
			segments.clear();
			starts.clear();
			ends.clear();
		}

		void push_back(std::string_view segment, source_position b, source_position e)
		{
			segments.push_back({ buffer.length(), segment.length() });
			buffer.append(segment);
			starts.push_back(b);
			ends.push_back(e);
		}

		TOML_PURE_INLINE_GETTER
		std::string_view operator[](size_t i) const noexcept
		{
			return std::string_view{ buffer.c_str() + segments[i].first, segments[i].second };
		}

		TOML_PURE_INLINE_GETTER
		std::string_view back() const noexcept
		{
			return (*this)[segments.size() - 1u];
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return segments.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return segments.size();
		}
	};

	struct depth_counter_scope
	{
		size_t& depth_;

		TOML_NODISCARD_CTOR
		explicit depth_counter_scope(size_t& depth) noexcept //
			: depth_{ depth }
		{
			depth_++;
		}

		~depth_counter_scope() noexcept
		{
			depth_--;
		}

		TOML_DELETE_DEFAULTS(depth_counter_scope);
	};

	struct parsed_string
	{
		std::string_view value;
		bool was_multi_line;
	};

	struct table_vector_scope
	{
		std::vector<table*>& tables;

		TOML_NODISCARD_CTOR
		explicit table_vector_scope(std::vector<table*>& tables_, table& tbl) //
			: tables{ tables_ }
		{
			tables.push_back(&tbl);
		}

		~table_vector_scope() noexcept
		{
			tables.pop_back();
		}

		TOML_DELETE_DEFAULTS(table_vector_scope);
	};
}
TOML_ANON_NAMESPACE_END;

#if 1 // parser helper macros

// Q: "what the fuck is this? MACROS????"
// A: The parser needs to work in exceptionless mode (returning error objects directly)
//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
//    These macros encapsulate the differences between the two modes so I can write code code
//    as though I was only targeting one mode and not want yeet myself into the sun.
//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
//    of toml++.

#define is_eof()		 !cp
#define assert_not_eof() TOML_ASSERT_ASSUME(cp != nullptr)
#define return_if_eof(...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#if TOML_EXCEPTIONS
#define is_error()					false
#define return_after_error(...)		TOML_UNREACHABLE
#define assert_not_error()			static_assert(true)
#define return_if_error(...)		static_assert(true)
#define return_if_error_or_eof(...) return_if_eof(__VA_ARGS__)
#else
#define is_error()				!!err
#define return_after_error(...) return __VA_ARGS__
#define assert_not_error()		TOML_ASSERT(!is_error())
#define return_if_error(...)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_error())                                                                                   \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
#define return_if_error_or_eof(...)                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof() || is_error())                                                                       \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
#endif

#if defined(TOML_BREAK_AT_PARSE_ERRORS) && TOML_BREAK_AT_PARSE_ERRORS
#if defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
#define parse_error_break() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define parse_error_break() __debugbreak()
#endif
#endif
#ifndef parse_error_break
#if TOML_MSVC || TOML_ICC
#define parse_error_break() __debugbreak()
#else
#define parse_error_break() TOML_ASSERT(false)
#endif
#endif
#else
#define parse_error_break() static_assert(true)
#endif

#define set_error_and_return(ret, ...)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!is_error())                                                                                               \
			set_error(__VA_ARGS__);                                                                                    \
		return_after_error(ret);                                                                                       \
	}                                                                                                                  \
	while (false)

#define set_error_and_return_default(...) set_error_and_return({}, __VA_ARGS__)

#define set_error_and_return_if_eof(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
			set_error_and_return(__VA_ARGS__, "encountered end-of-file"sv);                                            \
	}                                                                                                                  \
	while (false)

#define advance_and_return_if_error(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
	}                                                                                                                  \
	while (false)

#define advance_and_return_if_error_or_eof(...)                                                                        \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
		set_error_and_return_if_eof(__VA_ARGS__);                                                                      \
	}                                                                                                                  \
	while (false)

#endif // parser helper macros

TOML_IMPL_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);

	class parser
	{
	  private:
		static constexpr size_t max_nested_values = TOML_MAX_NESTED_VALUES;

		utf8_buffered_reader reader;
		table root;
		source_position prev_pos = { 1, 1 };
		const utf8_codepoint* cp = {};
		std::vector<table*> implicit_tables;
		std::vector<table*> dotted_key_tables;
		std::vector<table*> open_inline_tables;
		std::vector<array*> table_arrays;
		parse_key_buffer key_buffer;
		std::string string_buffer;
		std::string recording_buffer; // for diagnostics
		bool recording = false, recording_whitespace = true;
		std::string_view current_scope;
		size_t nested_values = {};
#if !TOML_EXCEPTIONS
		mutable optional<parse_error> err;
#endif

		TOML_NODISCARD
		source_position current_position(source_index fallback_offset = 0) const noexcept
		{
			if (!is_eof())
				return cp->position;
			return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at(source_position pos, const T&... reason) const
		{
			static_assert(sizeof...(T) > 0);
			return_if_error();

			error_builder builder{ current_scope };
			(builder.append(reason), ...);

			parse_error_break();

#if TOML_EXCEPTIONS
			builder.finish(pos, reader.source_path());
#else
			err.emplace(builder.finish(pos, reader.source_path()));
#endif
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		void set_error(const T&... reason) const
		{
			set_error_at(current_position(1), reason...);
		}

		void go_back(size_t count = 1) noexcept
		{
			return_if_error();
			TOML_ASSERT_ASSUME(count);

			cp		 = reader.step_back(count);
			prev_pos = cp->position;
		}

		void advance()
		{
			return_if_error();
			assert_not_eof();

			prev_pos = cp->position;
			cp		 = reader.read_next();

#if !TOML_EXCEPTIONS
			if (reader.error())
			{
				err = std::move(reader.error());
				return;
			}
#endif

			if (recording && !is_eof())
			{
				if (recording_whitespace || !is_whitespace(*cp))
					recording_buffer.append(cp->bytes, cp->count);
			}
		}

		void start_recording(bool include_current = true) noexcept
		{
			return_if_error();

			recording			 = true;
			recording_whitespace = true;
			recording_buffer.clear();
			if (include_current && !is_eof())
				recording_buffer.append(cp->bytes, cp->count);
		}

		void stop_recording(size_t pop_bytes = 0) noexcept
		{
			return_if_error();

			recording = false;
			if (pop_bytes)
			{
				if (pop_bytes >= recording_buffer.length())
					recording_buffer.clear();
				else if (pop_bytes == 1u)
					recording_buffer.pop_back();
				else
					recording_buffer.erase(recording_buffer.begin()
											   + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
										   recording_buffer.end());
			}
		}

		bool consume_leading_whitespace()
		{
			return_if_error_or_eof({});

			bool consumed = false;
			while (!is_eof() && is_horizontal_whitespace(*cp))
			{
				if TOML_UNLIKELY(!is_ascii_horizontal_whitespace(*cp))
					set_error_and_return_default("expected space or tab, saw '"sv, escaped_codepoint{ *cp }, "'"sv);

				consumed = true;
				advance_and_return_if_error({});
			}
			return consumed;
		}

		bool consume_line_break()
		{
			return_if_error_or_eof({});

			if TOML_UNLIKELY(is_match(*cp, U'\v', U'\f'))
				set_error_and_return_default(
					R"(vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);

			if (*cp == U'\r')
			{
				advance_and_return_if_error({}); // skip \r

				if TOML_UNLIKELY(is_eof())
					set_error_and_return_default("expected '\\n' after '\\r', saw EOF"sv);

				if TOML_UNLIKELY(*cp != U'\n')
					set_error_and_return_default("expected '\\n' after '\\r', saw '"sv,
												 escaped_codepoint{ *cp },
												 "'"sv);
			}
			else if (*cp != U'\n')
				return false;

			advance_and_return_if_error({}); // skip \n
			return true;
		}

		bool consume_rest_of_line()
		{
			return_if_error_or_eof({});

			do
			{
				if (is_ascii_vertical_whitespace(*cp))
					return consume_line_break();
				else
					advance();
				return_if_error({});
			}
			while (!is_eof());

			return true;
		}

		bool consume_comment()
		{
			return_if_error_or_eof({});

			if (*cp != U'#')
				return false;

			push_parse_scope("comment"sv);

			advance_and_return_if_error({}); // skip the '#'

			while (!is_eof())
			{
				if (consume_line_break())
					return true;
				return_if_error({});

#if TOML_LANG_AT_LEAST(1, 0, 0)

				// toml/issues/567 (disallow non-TAB control characters in comments)
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);

				// toml/pull/720 (disallow surrogates in comments)
				else if TOML_UNLIKELY(is_unicode_surrogate(*cp))
					set_error_and_return_default(
						"unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited in comments"sv);
#endif

				advance_and_return_if_error({});
			}

			return true;
		}

		TOML_NODISCARD
		bool consume_expected_sequence(std::u32string_view seq)
		{
			return_if_error({});
			TOML_ASSERT(!seq.empty());

			for (auto c : seq)
			{
				set_error_and_return_if_eof({});
				if (*cp != c)
					return false;
				advance_and_return_if_error({});
			}
			return true;
		}

		template <typename T>
		TOML_NODISCARD
		bool consume_digit_sequence(T* digits, size_t len)
		{
			return_if_error({});
			TOML_ASSERT_ASSUME(digits);
			TOML_ASSERT_ASSUME(len);

			for (size_t i = 0; i < len; i++)
			{
				set_error_and_return_if_eof({});
				if (!is_decimal_digit(*cp))
					return false;

				digits[i] = static_cast<T>(*cp - U'0');
				advance_and_return_if_error({});
			}
			return true;
		}

		template <typename T>
		TOML_NODISCARD
		size_t consume_variable_length_digit_sequence(T* buffer, size_t max_len)
		{
			return_if_error({});
			TOML_ASSERT_ASSUME(buffer);
			TOML_ASSERT_ASSUME(max_len);

			size_t i = {};
			for (; i < max_len; i++)
			{
				if (is_eof() || !is_decimal_digit(*cp))
					break;

				buffer[i] = static_cast<T>(*cp - U'0');
				advance_and_return_if_error({});
			}
			return i;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_basic_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'"');
			push_parse_scope("string"sv);

			// skip the '"'
			advance_and_return_if_error_or_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			auto& str = string_buffer;
			str.clear();
			bool escaped			 = false;
			bool skipping_whitespace = false;
			do
			{
				if (escaped)
				{
					escaped = false;

					// handle 'line ending slashes' in multi-line mode
					if (multi_line && is_whitespace(*cp))
					{
						consume_leading_whitespace();

						if TOML_UNLIKELY(!consume_line_break())
							set_error_and_return_default(
								"line-ending backslashes must be the last non-whitespace character on the line"sv);

						skipping_whitespace = true;
						return_if_error({});
						continue;
					}

					bool skip_escaped_codepoint = true;
					assert_not_eof();
					switch (const auto escaped_codepoint = *cp)
					{
						// 'regular' escape codes
						case U'b': str += '\b'; break;
						case U'f': str += '\f'; break;
						case U'n': str += '\n'; break;
						case U'r': str += '\r'; break;
						case U't': str += '\t'; break;
						case U'"': str += '"'; break;
						case U'\\': str += '\\'; break;

#if TOML_LANG_UNRELEASED // toml/pull/790 (\e shorthand for \x1B)
						case U'e': str += '\x1B'; break;
#else
						case U'e':
							set_error_and_return_default(
								"escape sequence '\\e' is not supported in TOML 1.0.0 and earlier"sv);
#endif

#if TOML_LANG_UNRELEASED // toml/pull/796 (\xHH unicode scalar sequences)
						case U'x': [[fallthrough]];
#else
						case U'x':
							set_error_and_return_default(
								"escape sequence '\\x' is not supported in TOML 1.0.0 and earlier"sv);
#endif

						// unicode scalar sequences
						case U'u': [[fallthrough]];
						case U'U':
						{
							push_parse_scope("unicode scalar sequence"sv);
							advance_and_return_if_error_or_eof({});
							skip_escaped_codepoint = false;

							uint32_t place_value =
								escaped_codepoint == U'U' ? 0x10000000u : (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
							uint32_t sequence_value{};
							while (place_value)
							{
								set_error_and_return_if_eof({});

								if TOML_UNLIKELY(!is_hexadecimal_digit(*cp))
									set_error_and_return_default("expected hex digit, saw '"sv, to_sv(*cp), "'"sv);

								sequence_value += place_value * hex_to_dec(*cp);
								place_value /= 16u;
								advance_and_return_if_error({});
							}

							if TOML_UNLIKELY(is_unicode_surrogate(sequence_value))
								set_error_and_return_default(
									"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
							else if TOML_UNLIKELY(sequence_value > 0x10FFFFu)
								set_error_and_return_default("values greater than U+10FFFF are invalid"sv);

							if (sequence_value < 0x80)
							{
								str += static_cast<char>(sequence_value);
							}
							else if (sequence_value < 0x800u)
							{
								str += static_cast<char>((sequence_value >> 6) | 0xC0u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							else if (sequence_value < 0x10000u)
							{
								str += static_cast<char>((sequence_value >> 12) | 0xE0u);
								str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							else if (sequence_value < 0x110000u)
							{
								str += static_cast<char>((sequence_value >> 18) | 0xF0u);
								str += static_cast<char>(((sequence_value >> 12) & 0x3Fu) | 0x80u);
								str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							break;
						}

							// ???
							TOML_UNLIKELY_CASE
						default: set_error_and_return_default("unknown escape sequence '\\"sv, to_sv(*cp), "'"sv);
					}

					if (skip_escaped_codepoint)
						advance_and_return_if_error_or_eof({});
				}
				else
				{
					// handle closing delimiters
					if (*cp == U'"')
					{
						if (multi_line)
						{
							size_t lookaheads			  = {};
							size_t consecutive_delimiters = 1;
							do
							{
								advance_and_return_if_error({});
								lookaheads++;
								if (!is_eof() && *cp == U'"')
									consecutive_delimiters++;
								else
									break;
							}
							while (lookaheads < 4u);

							switch (consecutive_delimiters)
							{
								// """ " (one quote somewhere in a ML string)
								case 1:
									str += '"';
									skipping_whitespace = false;
									continue;

								// """ "" (two quotes somewhere in a ML string)
								case 2:
									str.append("\"\""sv);
									skipping_whitespace = false;
									continue;

								// """ """ (the end of the string)
								case 3: return str;

								// """ """" (one at the end of the string)
								case 4: str += '"'; return str;

								// """ """"" (two quotes at the end of the string)
								case 5:
									str.append("\"\""sv);
									advance_and_return_if_error({}); // skip the last '"'
									return str;

								default: TOML_UNREACHABLE;
							}
						}
						else
						{
							advance_and_return_if_error({}); // skip the closing delimiter
							return str;
						}
					}

					// handle escapes
					else if (*cp == U'\\')
					{
						advance_and_return_if_error_or_eof({}); // skip the '\'
						skipping_whitespace = false;
						escaped				= true;
						continue;
					}

					// handle line endings in multi-line mode
					if (multi_line && is_ascii_vertical_whitespace(*cp))
					{
						consume_line_break();
						return_if_error({});
						if (!skipping_whitespace)
							str += '\n';
						continue;
					}

					// handle control characters
					if TOML_UNLIKELY(is_nontab_control_character(*cp))
						set_error_and_return_default(
							"unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv);

#if TOML_LANG_AT_LEAST(1, 0, 0)

					// handle surrogates in strings
					if TOML_UNLIKELY(is_unicode_surrogate(*cp))
						set_error_and_return_default(
							"unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv);
#endif

					if (multi_line)
					{
						if (!skipping_whitespace || !is_horizontal_whitespace(*cp))
						{
							skipping_whitespace = false;
							str.append(cp->bytes, cp->count);
						}
					}
					else
						str.append(cp->bytes, cp->count);

					advance_and_return_if_error({});
				}
			}
			while (!is_eof());

			set_error_and_return_default("encountered end-of-file"sv);
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_literal_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'\'');
			push_parse_scope("literal string"sv);

			// skip the delimiter
			advance_and_return_if_error_or_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			auto& str = string_buffer;
			str.clear();
			do
			{
				return_if_error({});

				// handle closing delimiters
				if (*cp == U'\'')
				{
					if (multi_line)
					{
						size_t lookaheads			  = {};
						size_t consecutive_delimiters = 1;
						do
						{
							advance_and_return_if_error({});
							lookaheads++;
							if (!is_eof() && *cp == U'\'')
								consecutive_delimiters++;
							else
								break;
						}
						while (lookaheads < 4u);

						switch (consecutive_delimiters)
						{
							// ''' ' (one quote somewhere in a ML string)
							case 1: str += '\''; continue;

							// ''' '' (two quotes somewhere in a ML string)
							case 2: str.append("''"sv); continue;

							// ''' ''' (the end of the string)
							case 3: return str;

							// ''' '''' (one at the end of the string)
							case 4: str += '\''; return str;

							// ''' ''''' (two quotes at the end of the string)
							case 5:
								str.append("''"sv);
								advance_and_return_if_error({}); // skip the last '
								return str;

							default: TOML_UNREACHABLE;
						}
					}
					else
					{
						advance_and_return_if_error({}); // skip the closing delimiter
						return str;
					}
				}

				// handle line endings in multi-line mode
				if (multi_line && is_ascii_vertical_whitespace(*cp))
				{
					consume_line_break();
					return_if_error({});
					str += '\n';
					continue;
				}

				// handle control characters
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are explicitly prohibited"sv);

#if TOML_LANG_AT_LEAST(1, 0, 0)

				// handle surrogates in strings
				if TOML_UNLIKELY(is_unicode_surrogate(*cp))
					set_error_and_return_default("unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
#endif

				str.append(cp->bytes, cp->count);
				advance_and_return_if_error({});
			}
			while (!is_eof());

			set_error_and_return_default("encountered end-of-file"sv);
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		parsed_string parse_string()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp));
			push_parse_scope("string"sv);

			// get the first three characters to determine the string type
			const auto first = cp->value;
			advance_and_return_if_error_or_eof({});
			const auto second = cp->value;
			advance_and_return_if_error({});
			const auto third = cp ? cp->value : U'\0';

			// if we were eof at the third character then first and second need to be
			// the same string character (otherwise it's an unterminated string)
			if (is_eof())
			{
				if (second == first)
					return {};

				set_error_and_return_default("encountered end-of-file"sv);
			}

			// if the first three characters are all the same string delimiter then
			// it's a multi-line string.
			else if (first == second && first == third)
			{
				return { first == U'\'' ? parse_literal_string(true) : parse_basic_string(true), true };
			}

			// otherwise it's just a regular string.
			else
			{
				// step back two characters so that the current
				// character is the string delimiter
				go_back(2u);

				return { first == U'\'' ? parse_literal_string(false) : parse_basic_string(false), false };
			}
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_bare_key_segment()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp));

			string_buffer.clear();

			while (!is_eof())
			{
				if (!is_bare_key_character(*cp))
					break;

				string_buffer.append(cp->bytes, cp->count);
				advance_and_return_if_error({});
			}

			return string_buffer;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		bool parse_boolean()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U't', U'f', U'T', U'F'));
			push_parse_scope("boolean"sv);

			start_recording(true);
			auto result = is_match(*cp, U't', U'T');
			if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				set_error_and_return_default("expected '"sv,
											 to_sv(result),
											 "', saw '"sv,
											 to_sv(recording_buffer),
											 "'"sv);
			stop_recording();

			if (cp && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return result;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_inf_or_nan()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
			push_parse_scope("floating-point"sv);

			start_recording(true);
			const bool negative = *cp == U'-';
			if (negative || *cp == U'+')
				advance_and_return_if_error_or_eof({});

			const bool inf = is_match(*cp, U'i', U'I');
			if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				set_error_and_return_default("expected '"sv,
											 inf ? "inf"sv : "nan"sv,
											 "', saw '"sv,
											 to_sv(recording_buffer),
											 "'"sv);
			stop_recording();

			if (cp && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return inf ? (negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity())
					   : std::numeric_limits<double>::quiet_NaN();
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'+', U'-', U'.') || is_decimal_digit(*cp));
			push_parse_scope("floating-point"sv);

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_eof({});

			// consume value chars
			char chars[utf8_buffered_reader::max_history_length];
			size_t length			   = {};
			const utf8_codepoint* prev = {};
			bool seen_decimal = false, seen_exponent = false;
			char first_integer_part = '\0';
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_decimal_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if TOML_UNLIKELY(prev && *prev == U'_' && !is_decimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(length == sizeof(chars))
					set_error_and_return_default("exceeds length limit of "sv,
												 sizeof(chars),
												 " digits"sv,
												 (seen_exponent ? ""sv : " (consider using exponent notation)"sv));
				else if (*cp == U'.')
				{
					// .1
					// -.1
					// +.1 (no integer part)
					if (!first_integer_part)
						set_error_and_return_default("expected decimal digit, saw '.'"sv);

					// 1.0e+.10 (exponent cannot have '.')
					else if (seen_exponent)
						set_error_and_return_default("expected exponent decimal digit or sign, saw '.'"sv);

					// 1.0.e+.10
					// 1..0
					// (multiple '.')
					else if (seen_decimal)
						set_error_and_return_default("expected decimal digit or exponent, saw '.'"sv);

					seen_decimal = true;
				}
				else if (is_match(*cp, U'e', U'E'))
				{
					if (prev && !is_decimal_digit(*prev))
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

					// 1.0ee+10 (multiple 'e')
					else if (seen_exponent)
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

					seen_decimal  = true; // implied
					seen_exponent = true;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 1.-0 (sign in mantissa)
					if (!seen_exponent)
						set_error_and_return_default("expected decimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

					// 1.0e1-0 (misplaced exponent sign)
					else if (!is_match(*prev, U'e', U'E'))
						set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);
				}
				else if (is_decimal_digit(*cp))
				{
					if (!seen_decimal)
					{
						if (!first_integer_part)
							first_integer_part = static_cast<char>(cp->bytes[0]);
						else if (first_integer_part == '0')
							set_error_and_return_default("leading zeroes are prohibited"sv);
					}
				}
				else
					set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

				chars[length++] = static_cast<char>(cp->bytes[0]);
				prev			= cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (prev)
			{
				if (*prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits"sv);
				}
				else if (is_match(*prev, U'e', U'E', U'+', U'-', U'.'))
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);
				}
			}

			// convert to double
			double result;
#if TOML_FLOAT_CHARCONV
			{
				auto fc_result = std::from_chars(chars, chars + length, result);
				switch (fc_result.ec)
				{
					TOML_LIKELY_CASE
					case std::errc{}: // ok
						return result * sign;

					case std::errc::invalid_argument:
						set_error_and_return_default("'"sv,
													 std::string_view{ chars, length },
													 "' could not be interpreted as a value"sv);
						break;

					case std::errc::result_out_of_range:
						set_error_and_return_default("'"sv,
													 std::string_view{ chars, length },
													 "' is not representable in 64 bits"sv);
						break;

					default: //??
						set_error_and_return_default("an unspecified error occurred while trying to interpret '"sv,
													 std::string_view{ chars, length },
													 "' as a value"sv);
				}
			}
#else
			{
				std::stringstream ss;
				ss.imbue(std::locale::classic());
				ss.write(chars, static_cast<std::streamsize>(length));
				if ((ss >> result))
					return result * sign;
				else
					set_error_and_return_default("'"sv,
												 std::string_view{ chars, length },
												 "' could not be interpreted as a value"sv);
			}
#endif
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_hex_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'0', U'+', U'-'));
			push_parse_scope("hexadecimal floating-point"sv);

#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_eof({});

			// '0'
			if (*cp != U'0')
				set_error_and_return_default(" expected '0', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// 'x' or 'X'
			if (!is_match(*cp, U'x', U'X'))
				set_error_and_return_default("expected 'x' or 'X', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// <HEX DIGITS> ([.]<HEX DIGITS>)? [pP] [+-]? <DEC DIGITS>

			// consume value fragments
			struct fragment
			{
				char chars[24];
				size_t length;
				double value;
			};
			fragment fragments[] = {
				{}, // mantissa, whole part
				{}, // mantissa, fractional part
				{}	// exponent
			};
			fragment* current_fragment = fragments;
			const utf8_codepoint* prev = {};
			int exponent_sign		   = 1;
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_hexadecimal_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if (prev && *prev == U'_' && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (*cp == U'.')
				{
					// 0x10.0p-.0 (exponent cannot have '.')
					if (current_fragment == fragments + 2)
						set_error_and_return_default("expected exponent digit or sign, saw '.'"sv);

					// 0x10.0.p-0 (multiple '.')
					else if (current_fragment == fragments + 1)
						set_error_and_return_default("expected hexadecimal digit or exponent, saw '.'"sv);

					else
						current_fragment++;
				}
				else if (is_match(*cp, U'p', U'P'))
				{
					// 0x10.0pp-0 (multiple 'p')
					if (current_fragment == fragments + 2)
						set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);

					// 0x.p-0 (mantissa is just '.')
					else if (fragments[0].length == 0u && fragments[1].length == 0u)
						set_error_and_return_default("expected hexadecimal digit, saw '"sv, to_sv(*cp), "'"sv);

					else
						current_fragment = fragments + 2;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 0x-10.0p-0 (sign in mantissa)
					if (current_fragment != fragments + 2)
						set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

					// 0x10.0p0- (misplaced exponent sign)
					else if (!is_match(*prev, U'p', U'P'))
						set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);

					else
						exponent_sign = *cp == U'-' ? -1 : 1;
				}
				else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);
				else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
					set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);
				else if (current_fragment->length == sizeof(fragment::chars))
					set_error_and_return_default("fragment exceeeds maximum length of "sv,
												 sizeof(fragment::chars),
												 " characters"sv);
				else
					current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);

				prev = cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (current_fragment != fragments + 2 || current_fragment->length == 0u)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("missing exponent"sv);
			}
			else if (prev && *prev == U'_')
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("underscores must be followed by digits"sv);
			}

			// calculate values for the three fragments
			for (int fragment_idx = 0; fragment_idx < 3; fragment_idx++)
			{
				auto& f				= fragments[fragment_idx];
				const uint32_t base = fragment_idx == 2 ? 10u : 16u;

				// left-trim zeroes
				const char* c = f.chars;
				size_t sig	  = {};
				while (f.length && *c == '0')
				{
					f.length--;
					c++;
					sig++;
				}
				if (!f.length)
					continue;

				// calculate value
				auto place = 1u;
				for (size_t i = 0; i < f.length - 1u; i++)
					place *= base;
				uint32_t val{};
				while (place)
				{
					if (base == 16)
						val += place * hex_to_dec(*c);
					else
						val += place * static_cast<uint32_t>(*c - '0');
					if (fragment_idx == 1)
						sig++;
					c++;
					place /= base;
				}
				f.value = static_cast<double>(val);

				// shift the fractional part
				if (fragment_idx == 1)
				{
					while (sig--)
						f.value /= base;
				}
			}

			return (fragments[0].value + fragments[1].value) * pow(2.0, fragments[2].value * exponent_sign) * sign;

#else // !TOML_LANG_UNRELEASED

			set_error_and_return_default("hexadecimal floating-point values are not supported "
										 "in TOML 1.0.0 and earlier"sv);

#endif // !TOML_LANG_UNRELEASED
		}

		template <uint64_t base>
		TOML_NODISCARD
		TOML_NEVER_INLINE
		int64_t parse_integer()
		{
			return_if_error({});
			assert_not_eof();
			using traits = parse_integer_traits<base>;
			push_parse_scope(traits::scope_qualifier);

			[[maybe_unused]] int64_t sign = 1;
			if constexpr (traits::is_signed)
			{
				sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});
			}

			if constexpr (base == 10)
			{
				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected expected digit or sign, saw '"sv, to_sv(*cp), "'"sv);
			}
			else
			{
				// '0'
				if (*cp != U'0')
					set_error_and_return_default("expected '0', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// 'b', 'o', 'x'
				if (*cp != traits::prefix_codepoint)
					set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected digit, saw '"sv, to_sv(*cp), "'"sv);
			}

			// consume digits
			char digits[utf8_buffered_reader::max_history_length];
			size_t length			   = {};
			const utf8_codepoint* prev = {};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !traits::is_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if TOML_UNLIKELY(prev && *prev == U'_' && !traits::is_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(!traits::is_digit(*cp))
					set_error_and_return_default("expected digit, saw '"sv, to_sv(*cp), "'"sv);
				else if TOML_UNLIKELY(length == sizeof(digits))
					set_error_and_return_default("exceeds length limit of "sv, sizeof(digits), " digits"sv);
				else
					digits[length++] = static_cast<char>(cp->bytes[0]);

				prev = cp;
				advance_and_return_if_error({});
			}

			// sanity check ending state
			if (prev && *prev == U'_')
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("underscores must be followed by digits"sv);
			}

			// single digits can be converted trivially
			if (length == 1u)
			{
				int64_t result;

				if constexpr (base == 16)
					result = static_cast<int64_t>(hex_to_dec(digits[0]));
				else
					result = static_cast<int64_t>(digits[0] - '0');

				if constexpr (traits::is_signed)
					result *= sign;

				return result;
			}

			// bin, oct and hex allow leading zeroes so trim them first
			const char* end = digits + length;
			const char* msd = digits;
			if constexpr (base != 10)
			{
				while (msd < end && *msd == '0')
					msd++;
				if (msd == end)
					return 0ll;
			}

			// decimal integers do not allow leading zeroes
			else
			{
				if TOML_UNLIKELY(digits[0] == '0')
					set_error_and_return_default("leading zeroes are prohibited"sv);
			}

			// range check
			if TOML_UNLIKELY(static_cast<size_t>(end - msd) > traits::max_digits)
				set_error_and_return_default("'"sv,
											 traits::full_prefix,
											 std::string_view{ digits, length },
											 "' is not representable in 64 bits"sv);

			// do the thing
			{
				uint64_t result = {};
				{
					uint64_t power = 1;
					while (--end >= msd)
					{
						if constexpr (base == 16)
							result += power * hex_to_dec(*end);
						else
							result += power * static_cast<uint64_t>(*end - '0');

						power *= base;
					}
				}

				// range check
				if TOML_UNLIKELY(result > static_cast<uint64_t>((std::numeric_limits<int64_t>::max)()) + (sign < 0 ? 1ull : 0ull))
					set_error_and_return_default("'"sv,
												 traits::full_prefix,
												 std::string_view{ digits, length },
												 "' is not representable in 64 bits"sv);

				if constexpr (traits::is_signed)
					return static_cast<int64_t>(result) * sign;
				else
					return static_cast<int64_t>(result);
			}
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		date parse_date(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date"sv);

			// "YYYY"
			uint32_t digits[4];
			if (!consume_digit_sequence(digits, 4u))
				set_error_and_return_default("expected 4-digit year, saw '"sv, to_sv(cp), "'"sv);
			const auto year			= digits[3] + digits[2] * 10u + digits[1] * 100u + digits[0] * 1000u;
			const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit month, saw '"sv, to_sv(cp), "'"sv);
			const auto month = digits[1] + digits[0] * 10u;
			if (month == 0u || month > 12u)
				set_error_and_return_default("expected month between 1 and 12 (inclusive), saw "sv, month);
			const auto max_days_in_month = month == 2u
											 ? (is_leap_year ? 29u : 28u)
											 : (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u);
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "DD"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit day, saw '"sv, to_sv(cp), "'"sv);
			const auto day = digits[1] + digits[0] * 10u;
			if (day == 0u || day > max_days_in_month)
				set_error_and_return_default("expected day between 1 and "sv,
											 max_days_in_month,
											 " (inclusive), saw "sv,
											 day);

			if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return { year, month, day };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		time parse_time(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("time"sv);

			static constexpr size_t max_digits = 64; // far more than necessary but needed to allow fractional
													 // millisecond truncation per the spec
			uint32_t digits[max_digits];

			// "HH"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(cp), "'"sv);
			const auto hour = digits[1] + digits[0] * 10u;
			if (hour > 23u)
				set_error_and_return_default("expected hour between 0 to 59 (inclusive), saw "sv, hour);
			set_error_and_return_if_eof({});

			// ':'
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(cp), "'"sv);
			const auto minute = digits[1] + digits[0] * 10u;
			if (minute > 59u)
				set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv, minute);
			auto time = toml::time{ hour, minute };

			// ':'
			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/671 (allow omission of seconds)
			{
				if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
					return time;
			}
			else
				set_error_and_return_if_eof({});
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "SS"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit second, saw '"sv, to_sv(cp), "'"sv);
			const auto second = digits[1] + digits[0] * 10u;
			if (second > 59u)
				set_error_and_return_default("expected second between 0 and 59 (inclusive), saw "sv, second);
			time.second = static_cast<decltype(time.second)>(second);

			// '.' (early-exiting is allowed; fractional is optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return time;
			if (*cp != U'.')
				set_error_and_return_default("expected '.', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "FFFFFFFFF"
			size_t digit_count = consume_variable_length_digit_sequence(digits, max_digits);
			if (!digit_count)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("expected fractional digits, saw '"sv, to_sv(*cp), "'"sv);
			}
			else if (!is_eof())
			{
				if (digit_count == max_digits && is_decimal_digit(*cp))
					set_error_and_return_default("fractional component exceeds maximum precision of "sv, max_digits);
				else if (!part_of_datetime && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);
			}
			uint32_t value = 0u;
			uint32_t place = 1u;
			for (auto i = impl::min<size_t>(digit_count, 9u); i-- > 0u;)
			{
				value += digits[i] * place;
				place *= 10u;
			}
			for (auto i = digit_count; i < 9u; i++) // implicit zeros
				value *= 10u;
			time.nanosecond = value;
			return time;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		date_time parse_date_time()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date-time"sv);

			// "YYYY-MM-DD"
			auto date = parse_date(true);
			set_error_and_return_if_eof({});

			// ' ', 'T' or 't'
			if (!is_match(*cp, U' ', U'T', U't'))
				set_error_and_return_default("expected space, 'T' or 't', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "HH:MM:SS.FFFFFFFFF"
			auto time = parse_time(true);
			return_if_error({});

			// no offset
			if (is_eof() || is_value_terminator(*cp))
				return { date, time };

			// zero offset ('Z' or 'z')
			time_offset offset{};
			if (is_match(*cp, U'Z', U'z'))
				advance_and_return_if_error({});

			// explicit offset ("+/-HH:MM")
			else if (is_match(*cp, U'+', U'-'))
			{
				push_parse_scope("date-time offset"sv);

				// sign
				int sign = *cp == U'-' ? -1 : 1;
				advance_and_return_if_error_or_eof({});

				// "HH"
				int digits[2];
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(cp), "'"sv);
				const auto hour = digits[1] + digits[0] * 10;
				if (hour > 23)
					set_error_and_return_default("expected hour between 0 and 23 (inclusive), saw "sv, hour);
				set_error_and_return_if_eof({});

				// ':'
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(cp), "'"sv);
				const auto minute = digits[1] + digits[0] * 10;
				if (minute > 59)
					set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv, minute);
				offset.minutes = static_cast<decltype(offset.minutes)>((hour * 60 + minute) * sign);
			}

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return { date, time, offset };
		}

		TOML_NODISCARD
		node_ptr parse_array();

		TOML_NODISCARD
		node_ptr parse_inline_table();

		TOML_NODISCARD
		node_ptr parse_value_known_prefixes()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_control_character(*cp));
			TOML_ASSERT_ASSUME(*cp != U'_');

			switch (cp->value)
			{
				// arrays
				case U'[': return parse_array();

				// inline tables
				case U'{': return parse_inline_table();

				// floats beginning with '.'
				case U'.': return node_ptr{ new value{ parse_float() } };

				// strings
				case U'"': [[fallthrough]];
				case U'\'': return node_ptr{ new value{ parse_string().value } };

				default:
				{
					const auto cp_upper = static_cast<uint_least32_t>(cp->value) & ~0x20u;

					// bools
					if (cp_upper == 70u || cp_upper == 84u) // F or T
						return node_ptr{ new value{ parse_boolean() } };

					// inf/nan
					else if (cp_upper == 73u || cp_upper == 78u) // I or N
						return node_ptr{ new value{ parse_inf_or_nan() } };

					else
						return nullptr;
				}
			}
			TOML_UNREACHABLE;
		}

		TOML_NODISCARD
		node_ptr parse_value()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_value_terminator(*cp));
			push_parse_scope("value"sv);

			const depth_counter_scope depth_counter{ nested_values };
			if TOML_UNLIKELY(nested_values > max_nested_values)
				set_error_and_return_default("exceeded maximum nested value depth of "sv,
											 max_nested_values,
											 " (TOML_MAX_NESTED_VALUES)"sv);

			// check if it begins with some control character
			// (note that this will also fail for whitespace but we're assuming we've
			// called consume_leading_whitespace() before calling parse_value())
			if TOML_UNLIKELY(is_control_character(*cp))
				set_error_and_return_default("unexpected control character"sv);

			// underscores at the beginning
			else if (*cp == U'_')
				set_error_and_return_default("values may not begin with underscores"sv);

			const auto begin_pos = cp->position;
			node_ptr val;

			do
			{
				TOML_ASSERT_ASSUME(!is_control_character(*cp));
				TOML_ASSERT_ASSUME(*cp != U'_');

				// detect the value type and parse accordingly,
				// starting with value types that can be detected
				// unambiguously from just one character.

				val = parse_value_known_prefixes();
				return_if_error({});
				if (val)
					break;

				// value types from here down require more than one character to unambiguously identify
				// so scan ahead and collect a set of value 'traits'.
				enum TOML_CLOSED_FLAGS_ENUM value_traits : int
				{
					has_nothing	 = 0,
					has_digits	 = 1,
					has_b		 = 1 << 1, // as second char only (0b)
					has_e		 = 1 << 2, // only float exponents
					has_o		 = 1 << 3, // as second char only (0o)
					has_p		 = 1 << 4, // only hexfloat exponents
					has_t		 = 1 << 5,
					has_x		 = 1 << 6, // as second or third char only (0x, -0x, +0x)
					has_z		 = 1 << 7,
					has_colon	 = 1 << 8,
					has_plus	 = 1 << 9,
					has_minus	 = 1 << 10,
					has_dot		 = 1 << 11,
					begins_sign	 = 1 << 12,
					begins_digit = 1 << 13,
					begins_zero	 = 1 << 14,
					signs_msk  = has_plus | has_minus,
					bdigit_msk = has_digits | begins_digit,
					bzero_msk  = bdigit_msk | begins_zero,
				};
				value_traits traits	 = has_nothing;
				const auto has_any	 = [&](auto t) noexcept { return (traits & t) != has_nothing; };
				const auto has_none	 = [&](auto t) noexcept { return (traits & t) == has_nothing; };
				const auto add_trait = [&](auto t) noexcept { traits = static_cast<value_traits>(traits | t); };

				// examine the first character to get the 'begins with' traits
				// (good fail-fast opportunity; all the remaining types begin with numeric digits or signs)
				if (is_decimal_digit(*cp))
				{
					add_trait(begins_digit);
					if (*cp == U'0')
						add_trait(begins_zero);
				}
				else if (is_match(*cp, U'+', U'-'))
					add_trait(begins_sign);
				else
					break;

				// scan the rest of the value to determine the remaining traits
				char32_t chars[utf8_buffered_reader::max_history_length];
				size_t char_count = {}, advance_count = {};
				bool eof_while_scanning = false;
				const auto scan			= [&]() noexcept(!TOML_COMPILER_EXCEPTIONS)
				{
					if (is_eof())
						return;
					TOML_ASSERT_ASSUME(!is_value_terminator(*cp));

					do
					{
						if (const auto c = **cp; c != U'_')
						{
							chars[char_count++] = c;

							if (is_decimal_digit(c))
								add_trait(has_digits);
							else if (is_ascii_letter(c))
							{
								TOML_ASSERT_ASSUME((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'));
								switch (static_cast<char32_t>(c | 32u))
								{
									case U'b':
										if (char_count == 2u && has_any(begins_zero))
											add_trait(has_b);
										break;

									case U'e':
										if (char_count > 1u
											&& has_none(has_b | has_o | has_p | has_t | has_x | has_z | has_colon)
											&& (has_none(has_plus | has_minus) || has_any(begins_sign)))
											add_trait(has_e);
										break;

									case U'o':
										if (char_count == 2u && has_any(begins_zero))
											add_trait(has_o);
										break;

									case U'p':
										if (has_any(has_x))
											add_trait(has_p);
										break;

									case U'x':
										if ((char_count == 2u && has_any(begins_zero))
											|| (char_count == 3u && has_any(begins_sign) && chars[1] == U'0'))
											add_trait(has_x);
										break;

									case U't': add_trait(has_t); break;
									case U'z': add_trait(has_z); break;
								}
							}
							else if (c <= U':')
							{
								TOML_ASSERT_ASSUME(c < U'0' || c > U'9');
								switch (c)
								{
									case U'+': add_trait(has_plus); break;
									case U'-': add_trait(has_minus); break;
									case U'.': add_trait(has_dot); break;
									case U':': add_trait(has_colon); break;
								}
							}
						}

						advance_and_return_if_error();
						advance_count++;
						eof_while_scanning = is_eof();
					}
					while (advance_count < (utf8_buffered_reader::max_history_length - 1u) && !is_eof()
						   && !is_value_terminator(*cp));
				};
				scan();
				return_if_error({});

				// force further scanning if this could have been a date-time with a space instead of a T
				if (char_count == 10u									 //
					&& (traits | begins_zero) == (bzero_msk | has_minus) //
					&& chars[4] == U'-'									 //
					&& chars[7] == U'-'									 //
					&& !is_eof()										 //
					&& *cp == U' ')
				{
					const auto pre_advance_count = advance_count;
					const auto pre_scan_traits	 = traits;
					chars[char_count++]			 = *cp;
					add_trait(has_t);

					const auto backpedal = [&]() noexcept
					{
						go_back(advance_count - pre_advance_count);
						advance_count = pre_advance_count;
						traits		  = pre_scan_traits;
						char_count	  = 10u;
					};

					advance_and_return_if_error({});
					advance_count++;

					if (is_eof() || !is_decimal_digit(*cp))
						backpedal();
					else
					{
						chars[char_count++] = *cp;

						advance_and_return_if_error({});
						advance_count++;

						scan();
						return_if_error({});

						if (char_count == 12u)
							backpedal();
					}
				}

				// set the reader back to where we started
				go_back(advance_count);

				// if after scanning ahead we still only have one value character,
				// the only valid value type is an integer.
				if (char_count == 1u)
				{
					if (has_any(begins_digit))
					{
						val.reset(new value{ static_cast<int64_t>(chars[0] - U'0') });
						advance(); // skip the digit
						break;
					}

					// anything else would be ambiguous.
					else
						set_error_and_return_default(eof_while_scanning ? "encountered end-of-file"sv
																		: "could not determine value type"sv);
				}

				// now things that can be identified from two or more characters
				return_if_error({});
				TOML_ASSERT_ASSUME(char_count >= 2u);

				// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
				// typed parse functions to take over and show better diagnostics if there's an issue
				// (as opposed to the fallback "could not determine type" message)
				if (has_any(has_p))
					val.reset(new value{ parse_hex_float() });
				else if (has_any(has_x | has_o | has_b))
				{
					int64_t i;
					value_flags flags;
					if (has_any(has_x))
					{
						i	  = parse_integer<16>();
						flags = value_flags::format_as_hexadecimal;
					}
					else if (has_any(has_o))
					{
						i	  = parse_integer<8>();
						flags = value_flags::format_as_octal;
					}
					else // has_b
					{
						i	  = parse_integer<2>();
						flags = value_flags::format_as_binary;
					}
					return_if_error({});

					val.reset(new value{ i });
					val->ref_cast<int64_t>().flags(flags);
				}
				else if (has_any(has_e) || (has_any(begins_digit) && chars[1] == U'.'))
					val.reset(new value{ parse_float() });
				else if (has_any(begins_sign))
				{
					// single-digit signed integers
					if (char_count == 2u && has_any(has_digits))
					{
						val.reset(new value{ static_cast<int64_t>(chars[1] - U'0') * (chars[0] == U'-' ? -1LL : 1LL) });
						advance(); // skip the sign
						advance(); // skip the digit
						break;
					}

					// simple signed floats (e.g. +1.0)
					if (is_decimal_digit(chars[1]) && chars[2] == U'.')
						val.reset(new value{ parse_float() });

					// signed infinity or nan
					else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
						val.reset(new value{ parse_inf_or_nan() });
				}

				return_if_error({});
				if (val)
					break;

				// match trait masks against what they can match exclusively.
				// all correct value parses will come out of this list, so doing this as a switch is likely to
				// be a better friend to the optimizer on the success path (failure path can be slow but that
				// doesn't matter much).
				switch (unwrap_enum(traits))
				{
					// binary integers
					// 0b10
					case bzero_msk | has_b:
						val.reset(new value{ parse_integer<2>() });
						val->ref_cast<int64_t>().flags(value_flags::format_as_binary);
						break;

					// octal integers
					// 0o10
					case bzero_msk | has_o:
						val.reset(new value{ parse_integer<8>() });
						val->ref_cast<int64_t>().flags(value_flags::format_as_octal);
						break;

					// decimal integers
					// 00
					// 10
					// +10
					// -10
					case bzero_msk: [[fallthrough]];
					case bdigit_msk: [[fallthrough]];
					case begins_sign | has_digits | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_plus:
					{
						// if the value was so long we exhausted the history buffer it's reasonable to assume
						// there was more and the value's actual type is impossible to identify without making the
						// buffer bigger (since it could have actually been a float), so emit an error.
						//
						// (this will likely only come up during fuzzing and similar scenarios)
						static constexpr size_t max_numeric_value_length =
							utf8_buffered_reader::max_history_length - 2u;
						if TOML_UNLIKELY(!eof_while_scanning && advance_count > max_numeric_value_length)
							set_error_and_return_default("numeric value too long to identify type - cannot exceed "sv,
														 max_numeric_value_length,
														 " characters"sv);

						val.reset(new value{ parse_integer<10>() });
						break;
					}

					// hexadecimal integers
					// 0x10
					case bzero_msk | has_x:
						val.reset(new value{ parse_integer<16>() });
						val->ref_cast<int64_t>().flags(value_flags::format_as_hexadecimal);
						break;

					// decimal floats
					// 0e1
					// 0e-1
					// 0e+1
					// 0.0
					// 0.0e1
					// 0.0e-1
					// 0.0e+1
					case bzero_msk | has_e: [[fallthrough]];
					case bzero_msk | has_e | has_minus: [[fallthrough]];
					case bzero_msk | has_e | has_plus: [[fallthrough]];
					case bzero_msk | has_dot: [[fallthrough]];
					case bzero_msk | has_dot | has_e: [[fallthrough]];
					case bzero_msk | has_dot | has_e | has_minus: [[fallthrough]];
					case bzero_msk | has_dot | has_e | has_plus: [[fallthrough]];
					// 1e1
					// 1e-1
					// 1e+1
					// 1.0
					// 1.0e1
					// 1.0e-1
					// 1.0e+1
					case bdigit_msk | has_e: [[fallthrough]];
					case bdigit_msk | has_e | has_minus: [[fallthrough]];
					case bdigit_msk | has_e | has_plus: [[fallthrough]];
					case bdigit_msk | has_dot: [[fallthrough]];
					case bdigit_msk | has_dot | has_e: [[fallthrough]];
					case bdigit_msk | has_dot | has_e | has_minus: [[fallthrough]];
					case bdigit_msk | has_dot | has_e | has_plus: [[fallthrough]];
					// +1e1
					// +1.0
					// +1.0e1
					// +1.0e+1
					// +1.0e-1
					// -1.0e+1
					case begins_sign | has_digits | has_e | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | signs_msk: [[fallthrough]];
					// -1e1
					// -1e+1
					// +1e-1
					// -1.0
					// -1.0e1
					// -1.0e-1
					case begins_sign | has_digits | has_e | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_e | signs_msk: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | has_minus:
						val.reset(new value{ parse_float() });
						break;

					// hexadecimal floats
					// 0x10p0
					// 0x10p-0
					// 0x10p+0
					case bzero_msk | has_x | has_p: [[fallthrough]];
					case bzero_msk | has_x | has_p | has_minus: [[fallthrough]];
					case bzero_msk | has_x | has_p | has_plus: [[fallthrough]];
					// -0x10p0
					// -0x10p-0
					// +0x10p0
					// +0x10p+0
					// -0x10p+0
					// +0x10p-0
					case begins_sign | has_digits | has_x | has_p | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_p | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_p | signs_msk: [[fallthrough]];
					// 0x10.1p0
					// 0x10.1p-0
					// 0x10.1p+0
					case bzero_msk | has_x | has_dot | has_p: [[fallthrough]];
					case bzero_msk | has_x | has_dot | has_p | has_minus: [[fallthrough]];
					case bzero_msk | has_x | has_dot | has_p | has_plus: [[fallthrough]];
					// -0x10.1p0
					// -0x10.1p-0
					// +0x10.1p0
					// +0x10.1p+0
					// -0x10.1p+0
					// +0x10.1p-0
					case begins_sign | has_digits | has_x | has_dot | has_p | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_dot | has_p | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_dot | has_p | signs_msk:
						val.reset(new value{ parse_hex_float() });
						break;

					// times
					// HH:MM
					// HH:MM:SS
					// HH:MM:SS.FFFFFF
					case bzero_msk | has_colon: [[fallthrough]];
					case bzero_msk | has_colon | has_dot: [[fallthrough]];
					case bdigit_msk | has_colon: [[fallthrough]];
					case bdigit_msk | has_colon | has_dot: val.reset(new value{ parse_time() }); break;

					// local dates
					// YYYY-MM-DD
					case bzero_msk | has_minus: [[fallthrough]];
					case bdigit_msk | has_minus: val.reset(new value{ parse_date() }); break;

					// date-times
					// YYYY-MM-DDTHH:MM
					// YYYY-MM-DDTHH:MM-HH:MM
					// YYYY-MM-DDTHH:MM+HH:MM
					// YYYY-MM-DD HH:MM
					// YYYY-MM-DD HH:MM-HH:MM
					// YYYY-MM-DD HH:MM+HH:MM
					// YYYY-MM-DDTHH:MM:SS
					// YYYY-MM-DDTHH:MM:SS-HH:MM
					// YYYY-MM-DDTHH:MM:SS+HH:MM
					// YYYY-MM-DD HH:MM:SS
					// YYYY-MM-DD HH:MM:SS-HH:MM
					// YYYY-MM-DD HH:MM:SS+HH:MM
					case bzero_msk | has_minus | has_colon | has_t: [[fallthrough]];
					case bzero_msk | signs_msk | has_colon | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_t: [[fallthrough]];
					case bdigit_msk | signs_msk | has_colon | has_t: [[fallthrough]];
					// YYYY-MM-DDTHH:MM:SS.FFFFFF
					// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
					// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
					// YYYY-MM-DD HH:MM:SS.FFFFFF
					// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
					// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
					case bzero_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
					case bzero_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
					case bdigit_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
					// YYYY-MM-DDTHH:MMZ
					// YYYY-MM-DD HH:MMZ
					// YYYY-MM-DDTHH:MM:SSZ
					// YYYY-MM-DD HH:MM:SSZ
					// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
					// YYYY-MM-DD HH:MM:SS.FFFFFFZ
					case bzero_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
					case bzero_msk | has_minus | has_colon | has_dot | has_z | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
						val.reset(new value{ parse_date_time() });
						break;
				}
			}
			while (false);

			if (!val)
			{
				set_error_at(begin_pos, "could not determine value type"sv);
				return_after_error({});
			}

			val->source_ = { begin_pos, current_position(1), reader.source_path() };
			return val;
		}

		TOML_NEVER_INLINE
		bool parse_key()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp) || is_string_delimiter(*cp));
			push_parse_scope("key"sv);

			key_buffer.clear();
			recording_whitespace = false;

			while (!is_error())
			{
#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
				if (is_combining_mark(*cp))
					set_error_and_return_default("bare keys may not begin with unicode combining marks"sv);
#endif

				std::string_view key_segment;
				const auto key_begin = current_position();

				// bare_key_segment
				if (is_bare_key_character(*cp))
					key_segment = parse_bare_key_segment();

				// "quoted key segment"
				else if (is_string_delimiter(*cp))
				{
					const auto begin_pos = cp->position;

					recording_whitespace = true;
					parsed_string str	 = parse_string();
					recording_whitespace = false;
					return_if_error({});

					if (str.was_multi_line)
					{
						set_error_at(begin_pos,
									 "multi-line strings are prohibited in "sv,
									 key_buffer.empty() ? ""sv : "dotted "sv,
									 "keys"sv);
						return_after_error({});
					}
					else
						key_segment = str.value;
				}

				// ???
				else
					set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
												 to_sv(*cp),
												 "'"sv);

				const auto key_end = current_position();

				// whitespace following the key segment
				consume_leading_whitespace();

				// store segment
				key_buffer.push_back(key_segment, key_begin, key_end);

				// eof or no more key to come
				if (is_eof() || *cp != U'.')
					break;

				// was a dotted key - go around again
				advance_and_return_if_error_or_eof({});
				consume_leading_whitespace();
				set_error_and_return_if_eof({});
			}
			return_if_error({});

			return true;
		}

		TOML_NODISCARD
		key make_key(size_t segment_index) const
		{
			TOML_ASSERT(key_buffer.size() > segment_index);

			return key{
				key_buffer[segment_index],
				source_region{ key_buffer.starts[segment_index], key_buffer.ends[segment_index], root.source().path }
			};
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		table* parse_table_header()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'[');
			push_parse_scope("table header"sv);

			const source_position header_begin_pos = cp->position;
			source_position header_end_pos;
			bool is_arr = false;

			// parse header
			{
				// skip first '['
				advance_and_return_if_error_or_eof({});

				// skip past any whitespace that followed the '['
				const bool had_leading_whitespace = consume_leading_whitespace();
				set_error_and_return_if_eof({});

				// skip second '[' (if present)
				if (*cp == U'[')
				{
					if (had_leading_whitespace)
						set_error_and_return_default(
							"[[array-of-table]] brackets must be contiguous (i.e. [ [ this ] ] is prohibited)"sv);

					is_arr = true;
					advance_and_return_if_error_or_eof({});

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					set_error_and_return_if_eof({});
				}

				// check for a premature closing ']'
				if (*cp == U']')
					set_error_and_return_default("tables with blank bare keys are explicitly prohibited"sv);

				// get the actual key
				start_recording();
				parse_key();
				stop_recording(1u);
				return_if_error({});

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});

				// consume the closing ']'
				if (*cp != U']')
					set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
				if (is_arr)
				{
					advance_and_return_if_error_or_eof({});
					if (*cp != U']')
						set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
				}
				advance_and_return_if_error({});
				header_end_pos = current_position(1);

				// handle the rest of the line after the header
				consume_leading_whitespace();
				if (!is_eof() && !consume_comment() && !consume_line_break())
					set_error_and_return_default("expected a comment or whitespace, saw '"sv, to_sv(cp), "'"sv);
			}
			TOML_ASSERT(!key_buffer.empty());

			// check if each parent is a table/table array, or can be created implicitly as a table.
			table* parent = &root;
			for (size_t i = 0, e = key_buffer.size() - 1u; i < e; i++)
			{
				const std::string_view segment = key_buffer[i];
				auto pit					   = parent->lower_bound(segment);

				// parent already existed
				if (pit != parent->end() && pit->first == segment)
				{
					node& p = pit->second;

					if (auto tbl = p.as_table())
					{
						// adding to closed inline tables is illegal
						if (tbl->is_inline() && !impl::find(open_inline_tables.begin(), open_inline_tables.end(), tbl))
							set_error_and_return_default("cannot insert '"sv,
														 to_sv(recording_buffer),
														 "' into existing inline table"sv);

						parent = tbl;
					}
					else if (auto arr = p.as_array(); arr && impl::find(table_arrays.begin(), table_arrays.end(), arr))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!arr->empty());
						TOML_ASSERT(arr->back().is_table());
						parent = &arr->back().ref_cast<table>();
					}
					else
					{
						if (!is_arr && p.type() == node_type::table)
							set_error_and_return_default("cannot redefine existing table '"sv,
														 to_sv(recording_buffer),
														 "'"sv);
						else
							set_error_and_return_default("cannot redefine existing "sv,
														 to_sv(p.type()),
														 " '"sv,
														 to_sv(recording_buffer),
														 "' as "sv,
														 is_arr ? "array-of-tables"sv : "table"sv);
					}
				}

				// need to create a new implicit table
				else
				{
					pit		  = parent->emplace_hint<table>(pit, make_key(i));
					table& p  = pit->second.ref_cast<table>();
					p.source_ = pit->first.source();

					implicit_tables.push_back(&p);
					parent = &p;
				}
			}

			const auto last_segment = key_buffer.back();
			auto it					= parent->lower_bound(last_segment);

			// if there was already a matching node some sanity checking is necessary;
			// this is ok if we're making an array and the existing element is already an array (new element)
			// or if we're making a table and the existing element is an implicitly-created table (promote it),
			// otherwise this is a redefinition error.
			if (it != parent->end() && it->first == last_segment)
			{
				node& matching_node = it->second;
				if (auto arr = matching_node.as_array();
					is_arr && arr && impl::find(table_arrays.begin(), table_arrays.end(), arr))
				{
					table& tbl	= arr->emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				else if (auto tbl = matching_node.as_table(); !is_arr && tbl && !implicit_tables.empty())
				{
					if (auto found = impl::find(implicit_tables.begin(), implicit_tables.end(), tbl);
						found && (tbl->empty() || tbl->is_homogeneous<table>()))
					{
						implicit_tables.erase(implicit_tables.cbegin() + (found - implicit_tables.data()));
						tbl->source_.begin = header_begin_pos;
						tbl->source_.end   = header_end_pos;
						return tbl;
					}
				}

				// if we get here it's a redefinition error.
				if (!is_arr && matching_node.type() == node_type::table)
				{
					set_error_at(header_begin_pos,
								 "cannot redefine existing table '"sv,
								 to_sv(recording_buffer),
								 "'"sv);
					return_after_error({});
				}
				else
				{
					set_error_at(header_begin_pos,
								 "cannot redefine existing "sv,
								 to_sv(matching_node.type()),
								 " '"sv,
								 to_sv(recording_buffer),
								 "' as "sv,
								 is_arr ? "array-of-tables"sv : "table"sv);
					return_after_error({});
				}
			}

			// there was no matching node, sweet - we can freely instantiate a new table/table array.
			else
			{
				auto last_key = make_key(key_buffer.size() - 1u);

				// if it's an array we need to make the array and it's first table element,
				// set the starting regions, and return the table element
				if (is_arr)
				{
					it			   = parent->emplace_hint<array>(it, std::move(last_key));
					array& tbl_arr = it->second.ref_cast<array>();
					table_arrays.push_back(&tbl_arr);
					tbl_arr.source_ = { header_begin_pos, header_end_pos, reader.source_path() };

					table& tbl	= tbl_arr.emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				// otherwise we're just making a table
				else
				{
					it			= parent->emplace_hint<table>(it, std::move(last_key));
					table& tbl	= it->second.ref_cast<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}
			}
		}

		TOML_NEVER_INLINE
		bool parse_key_value_pair_and_insert(table* tbl)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp) || is_bare_key_character(*cp));
			push_parse_scope("key-value pair"sv);

			// read the key into the key buffer
			start_recording();
			parse_key();
			stop_recording(1u);
			return_if_error({});
			TOML_ASSERT(key_buffer.size() >= 1u);

			// skip past any whitespace that followed the key
			consume_leading_whitespace();
			set_error_and_return_if_eof({});

			// '='
			if (*cp != U'=')
				set_error_and_return_default("expected '=', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// skip past any whitespace that followed the '='
			consume_leading_whitespace();
			return_if_error({});
			set_error_and_return_if_eof({});

			// check that the next character could actually be a value
			if (is_value_terminator(*cp))
				set_error_and_return_default("expected value, saw '"sv, to_sv(*cp), "'"sv);

			// if it's a dotted kvp we need to spawn the parent sub-tables if necessary,
			// and set the target table to the second-to-last one in the chain
			if (key_buffer.size() > 1u)
			{
				for (size_t i = 0; i < key_buffer.size() - 1u; i++)
				{
					const std::string_view segment = key_buffer[i];
					auto pit					   = tbl->lower_bound(segment);

					// parent already existed
					if (pit != tbl->end() && pit->first == segment)
					{
						table* p = pit->second.as_table();

						// redefinition
						if TOML_UNLIKELY(!p
							|| !(impl::find(dotted_key_tables.begin(), dotted_key_tables.end(), p)
								 || impl::find(implicit_tables.begin(), implicit_tables.end(), p)))
						{
							set_error_at(key_buffer.starts[i],
										 "cannot redefine existing "sv,
										 to_sv(pit->second.type()),
										 " as dotted key-value pair"sv);
							return_after_error({});
						}

						tbl = p;
					}

					// need to create a new implicit table
					else
					{
						pit		  = tbl->emplace_hint<table>(pit, make_key(i));
						table& p  = pit->second.ref_cast<table>();
						p.source_ = pit->first.source();

						dotted_key_tables.push_back(&p);
						tbl = &p;
					}
				}
			}

			// ensure this isn't a redefinition
			const std::string_view last_segment = key_buffer.back();
			auto it								= tbl->lower_bound(last_segment);
			if (it != tbl->end() && it->first == last_segment)
			{
				set_error("cannot redefine existing "sv,
						  to_sv(it->second.type()),
						  " '"sv,
						  to_sv(recording_buffer),
						  "'"sv);
				return_after_error({});
			}

			// create the key first since the key buffer will likely get overwritten during value parsing (inline
			// tables)
			auto last_key = make_key(key_buffer.size() - 1u);

			// now we can actually parse the value
			node_ptr val = parse_value();
			return_if_error({});

			tbl->emplace_hint<node_ptr>(it, std::move(last_key), std::move(val));
			return true;
		}

		void parse_document()
		{
			assert_not_error();
			assert_not_eof();
			push_parse_scope("root table"sv);

			table* current_table = &root;

			do
			{
				return_if_error();

				// leading whitespace, line endings, comments
				if (consume_leading_whitespace() || consume_line_break() || consume_comment())
					continue;
				return_if_error();

				// [tables]
				// [[table array]]
				if (*cp == U'[')
					current_table = parse_table_header();

				// bare_keys
				// dotted.keys
				// "quoted keys"
				else if (is_bare_key_character(*cp) || is_string_delimiter(*cp))
				{
					push_parse_scope("key-value pair"sv);

					parse_key_value_pair_and_insert(current_table);

					// handle the rest of the line after the kvp
					// (this is not done in parse_key_value_pair() because that is also used for inline tables)
					consume_leading_whitespace();
					return_if_error();
					if (!is_eof() && !consume_comment() && !consume_line_break())
						set_error("expected a comment or whitespace, saw '"sv, to_sv(cp), "'"sv);
				}

				else // ??
					set_error("expected keys, tables, whitespace or comments, saw '"sv, to_sv(cp), "'"sv);
			}
			while (!is_eof());

			auto eof_pos	 = current_position(1);
			root.source_.end = eof_pos;
			if (current_table && current_table != &root && current_table->source_.end <= current_table->source_.begin)
				current_table->source_.end = eof_pos;
		}

		static void update_region_ends(node& nde) noexcept
		{
			const auto type = nde.type();
			if (type > node_type::array)
				return;

			if (type == node_type::table)
			{
				auto& tbl = nde.ref_cast<table>();
				if (tbl.is_inline()) // inline tables (and all their inline descendants) are already correctly
									 // terminated
					return;

				auto end = nde.source_.end;
				for (auto&& [k, v] : tbl)
				{
					TOML_UNUSED(k);
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
			}
			else // arrays
			{
				auto& arr = nde.ref_cast<array>();
				auto end  = nde.source_.end;
				for (auto&& v : arr)
				{
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
				nde.source_.end = end;
			}
		}

	  public:
		parser(utf8_reader_interface&& reader_) //
			: reader{ reader_ }
		{
			root.source_ = { prev_pos, prev_pos, reader.source_path() };

			if (!reader.peek_eof())
			{
				cp = reader.read_next();

#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
#endif

				if (cp)
					parse_document();
			}

			update_region_ends(root);
		}

		TOML_NODISCARD
		operator parse_result() && noexcept
		{
#if TOML_EXCEPTIONS

			return { std::move(root) };

#else

			if (err)
				return parse_result{ *std::move(err) };
			else
				return parse_result{ std::move(root) };

#endif
		}
	};

	TOML_EXTERNAL_LINKAGE
	node_ptr parser::parse_array()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_eof({});

		node_ptr arr_ptr{ new array{} };
		array& arr = arr_ptr->ref_cast<array>();
		enum TOML_CLOSED_ENUM parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (!is_error())
		{
			while (consume_leading_whitespace() || consume_line_break() || consume_comment())
				continue;
			set_error_and_return_if_eof({});

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == val)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				set_error_and_return_default("expected value or closing ']', saw comma"sv);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance_and_return_if_error({});
				break;
			}

			// must be a value
			else
			{
				if (prev == val)
				{
					set_error_and_return_default("expected comma or closing ']', saw '"sv, to_sv(*cp), "'"sv);
					continue;
				}
				prev = val;
				if (!arr.capacity())
					arr.reserve(4u);
				arr.emplace_back<node_ptr>(parse_value());
			}
		}

		return_if_error({});
		return arr_ptr;
	}

	TOML_EXTERNAL_LINKAGE
	node_ptr parser::parse_inline_table()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_eof({});

		node_ptr tbl_ptr{ new table{} };
		table& tbl = tbl_ptr->ref_cast<table>();
		tbl.is_inline(true);
		table_vector_scope table_scope{ open_inline_tables, tbl };

		enum TOML_CLOSED_ENUM parse_elem : int
		{
			none,
			comma,
			kvp
		};
		parse_elem prev = none;
		while (!is_error())
		{
			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
			{
				while (consume_leading_whitespace() || consume_line_break() || consume_comment())
					continue;
			}
			else
			{
				while (consume_leading_whitespace())
					continue;
			}
			return_if_error({});
			set_error_and_return_if_eof({});

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == kvp)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
				}
				else
					set_error_and_return_default("expected key-value pair or closing '}', saw comma"sv);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				if constexpr (!TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
				{
					if (prev == comma)
					{
						set_error_and_return_default("expected key-value pair, saw closing '}' (dangling comma)"sv);
						continue;
					}
				}
				advance_and_return_if_error({});
				break;
			}

			// key-value pair
			else if (is_string_delimiter(*cp) || is_bare_key_character(*cp))
			{
				if (prev == kvp)
					set_error_and_return_default("expected comma or closing '}', saw '"sv, to_sv(*cp), "'"sv);
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(&tbl);
				}
			}

			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, to_sv(*cp), "'"sv);
		}

		return_if_error({});
		return tbl_ptr;
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_IMPL_NAMESPACE_END;

#undef TOML_RETURNS_BY_THROWING
#undef advance_and_return_if_error
#undef advance_and_return_if_error_or_eof
#undef assert_not_eof
#undef assert_not_error
#undef is_eof
#undef is_error
#undef parse_error_break
#undef push_parse_scope
#undef push_parse_scope_1
#undef push_parse_scope_2
#undef return_after_error
#undef return_if_eof
#undef return_if_error
#undef return_if_error_or_eof
#undef set_error_and_return
#undef set_error_and_return_default
#undef set_error_and_return_if_eof
#undef utf8_buffered_reader_error_check
#undef utf8_reader_error
#undef utf8_reader_error_check
#undef utf8_reader_return_after_error

TOML_ANON_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse(utf8_reader_interface && reader)
	{
		return impl::parser{ std::move(reader) };
	}

	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse_file(std::string_view file_path)
	{
#if TOML_EXCEPTIONS
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	throw parse_error                                                                                                  \
	{                                                                                                                  \
		msg, source_position{}, std::make_shared<const std::string>(std::move(path))                                   \
	}
#else
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	return parse_result                                                                                                \
	{                                                                                                                  \
		parse_error                                                                                                    \
		{                                                                                                              \
			msg, source_position{}, std::make_shared<const std::string>(std::move(path))                               \
		}                                                                                                              \
	}
#endif

		std::string file_path_str(file_path);

		// open file with a custom-sized stack buffer
		std::ifstream file;
		TOML_OVERALIGNED char file_buffer[sizeof(void*) * 1024u];
		file.rdbuf()->pubsetbuf(file_buffer, sizeof(file_buffer));
		file.open(file_path_str, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
		if (!file.is_open())
			TOML_PARSE_FILE_ERROR("File could not be opened for reading", file_path_str);

		// get size
		const auto file_size = file.tellg();
		if (file_size == -1)
			TOML_PARSE_FILE_ERROR("Could not determine file size", file_path_str);
		file.seekg(0, std::ifstream::beg);

		// read the whole file into memory first if the file isn't too large
		constexpr auto large_file_threshold = 1024 * 1024 * 2; // 2 MB
		if (file_size <= large_file_threshold)
		{
			std::vector<char> file_data;
			file_data.resize(static_cast<size_t>(file_size));
			file.read(file_data.data(), static_cast<std::streamsize>(file_size));
			return parse(std::string_view{ file_data.data(), file_data.size() }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		else
			return parse(file, std::move(file_path_str));

#undef TOML_PARSE_FILE_ERROR
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::string_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(file_path);
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::u8string_view file_path)
	{
		std::string file_path_str;
		file_path_str.resize(file_path.length());
		memcpy(file_path_str.data(), file_path.data(), file_path.length());
		return TOML_ANON_NAMESPACE::do_parse_file(file_path_str);
	}

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::wstring_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(impl::narrow(file_path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#undef TOML_OVERALIGNED

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/formatter.inl  ****************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	enum class TOML_CLOSED_FLAGS_ENUM formatted_string_traits : unsigned
	{
		none,
		line_breaks	  = 1u << 0, // \n
		tabs		  = 1u << 1, // \t
		control_chars = 1u << 2, // also includes non-ascii vertical whitespace
		single_quotes = 1u << 3,
		non_bare	  = 1u << 4, // anything not satisfying "is bare key character"
		non_ascii	  = 1u << 5, // any codepoint >= 128

		all = (non_ascii << 1u) - 1u
	};
	TOML_MAKE_FLAGS(formatted_string_traits);

	TOML_EXTERNAL_LINKAGE
	formatter::formatter(const node* source_node,
						 const parse_result* source_pr,
						 const formatter_constants& constants,
						 const formatter_config& config) noexcept //
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		: source_{ source_pr && *source_pr ? &source_pr->table() : source_node },
		  result_{ source_pr },
#else
		: source_{ source_pr ? source_pr : source_node },
#endif
		  constants_{ &constants },
		  config_{ config }
	{
		TOML_ASSERT_ASSUME(source_);

		config_.flags = (config_.flags | constants_->mandatory_flags) & ~constants_->ignored_flags;

		indent_columns_ = {};
		for (auto c : config_.indent)
			indent_columns_ += c == '\t' ? 4u : 1u;

		int_format_mask_ = config_.flags
						 & (format_flags::allow_binary_integers | format_flags::allow_octal_integers
							| format_flags::allow_hexadecimal_integers);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::attach(std::ostream & stream) noexcept
	{
		indent_		   = {};
		naked_newline_ = true;
		stream_		   = &stream;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::detach() noexcept
	{
		stream_ = nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_newline(bool force)
	{
		if (!naked_newline_ || force)
		{
			print_to_stream(*stream_, '\n');
			naked_newline_ = true;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_indent()
	{
		for (int i = 0; i < indent_; i++)
		{
			print_to_stream(*stream_, config_.indent);
			naked_newline_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(char c)
	{
		print_to_stream(*stream_, c);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(std::string_view str)
	{
		print_to_stream(*stream_, str);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str, bool allow_multi_line, bool allow_bare)
	{
		if (str.empty())
		{
			print_unformatted(literal_strings_allowed() ? "''"sv : "\"\""sv);
			return;
		}

		// pre-scan the string to determine how we should output it
		formatted_string_traits traits = {};

		if (!allow_bare)
			traits |= formatted_string_traits::non_bare;
		bool unicode_allowed = unicode_strings_allowed();

		// ascii fast path
		if (is_ascii(str.data(), str.length()))
		{
			for (auto c : str)
			{
				switch (c)
				{
					case '\n': traits |= formatted_string_traits::line_breaks; break;
					case '\t': traits |= formatted_string_traits::tabs; break;
					case '\'': traits |= formatted_string_traits::single_quotes; break;
					default:
					{
						if TOML_UNLIKELY(is_control_character(c))
							traits |= formatted_string_traits::control_chars;

						if (!is_ascii_bare_key_character(static_cast<char32_t>(c)))
							traits |= formatted_string_traits::non_bare;
						break;
					}
				}

				static constexpr auto all_ascii_traits =
					formatted_string_traits::all & ~formatted_string_traits::non_ascii;
				if (traits == all_ascii_traits)
					break;
			}
		}

		// unicode slow path
		else
		{
			traits |= formatted_string_traits::non_ascii;
			utf8_decoder decoder;

			// if the unicode is malformed just treat the string as a single-line non-literal and
			// escape all non-ascii characters (to ensure round-tripping and help with diagnostics)
			const auto bad_unicode = [&]() noexcept
			{
				traits &= ~formatted_string_traits::line_breaks;
				traits |= formatted_string_traits::control_chars | formatted_string_traits::non_bare;
				unicode_allowed = false;
			};

			for (auto c : str)
			{
				decoder(c);

				if TOML_UNLIKELY(decoder.error())
				{
					bad_unicode();
					break;
				}

				if (!decoder.has_code_point())
					continue;

				switch (decoder.codepoint)
				{
					case U'\n': traits |= formatted_string_traits::line_breaks; break;
					case U'\t': traits |= formatted_string_traits::tabs; break;
					case U'\'': traits |= formatted_string_traits::single_quotes; break;
					default:
					{
						if TOML_UNLIKELY(is_control_character(decoder.codepoint)
							|| is_non_ascii_vertical_whitespace(decoder.codepoint))
							traits |= formatted_string_traits::control_chars;

						if (!is_bare_key_character(decoder.codepoint))
							traits |= formatted_string_traits::non_bare;
						break;
					}
				}
			}

			if (decoder.needs_more_input())
				bad_unicode();
		}

		// if the string meets the requirements of being 'bare' we can emit a bare string
		// (bare strings are composed of letters and numbers; no whitespace, control chars, quotes, etc)
		if (!(traits & formatted_string_traits::non_bare)
			&& (!(traits & formatted_string_traits::non_ascii) || unicode_allowed))
		{
			print_unformatted(str);
			return;
		}

		// determine if this should be a multi-line string (triple-quotes)
		const auto multi_line = allow_multi_line			 //
							 && multi_line_strings_allowed() //
							 && !!(traits & formatted_string_traits::line_breaks);

		// determine if this should be a literal string (single-quotes with no escaping)
		const auto literal = literal_strings_allowed()													   //
						  && !(traits & formatted_string_traits::control_chars)							   //
						  && (!(traits & formatted_string_traits::single_quotes) || multi_line)			   //
						  && (!(traits & formatted_string_traits::tabs) || real_tabs_in_strings_allowed()) //
						  && (!(traits & formatted_string_traits::non_ascii) || unicode_allowed);

		// literal strings (single quotes, no escape codes)
		if (literal)
		{
			const auto quot = multi_line ? R"(''')"sv : R"(')"sv;
			print_unformatted(quot);
			print_unformatted(str);
			print_unformatted(quot);
			return;
		}

		// anything from here down is a non-literal string, so requires iteration and escaping.
		print_unformatted(multi_line ? R"(""")"sv : R"(")"sv);

		const auto real_tabs_allowed = real_tabs_in_strings_allowed();

		// ascii fast path
		if (!(traits & formatted_string_traits::non_ascii))
		{
			for (auto c : str)
			{
				switch (c)
				{
					case '"': print_to_stream(*stream_, R"(\")"sv); break;
					case '\\': print_to_stream(*stream_, R"(\\)"sv); break;
					case '\x7F': print_to_stream(*stream_, R"(\u007F)"sv); break;
					case '\t': print_to_stream(*stream_, real_tabs_allowed ? "\t"sv : R"(\t)"sv); break;
					case '\n': print_to_stream(*stream_, multi_line ? "\n"sv : R"(\n)"sv); break;
					default:
					{
						// control characters from lookup table
						if TOML_UNLIKELY(c >= '\x00' && c <= '\x1F')
							print_to_stream(*stream_, control_char_escapes[c]);

						// regular characters
						else
							print_to_stream(*stream_, c);
					}
				}
			}
		}

		// unicode slow path
		else
		{
			utf8_decoder decoder;
			const char* cp_start = str.data();
			const char* cp_end	 = cp_start;
			for (auto c : str)
			{
				decoder(c);
				cp_end++;

				// if the decoder encounters malformed unicode just emit raw bytes and
				if (decoder.error())
				{
					while (cp_start != cp_end)
					{
						print_to_stream(*stream_, R"(\u00)"sv);
						print_to_stream(*stream_,
										static_cast<uint8_t>(*cp_start),
										value_flags::format_as_hexadecimal,
										2);
						cp_start++;
					}
					decoder.reset();
					continue;
				}

				if (!decoder.has_code_point())
					continue;

				switch (decoder.codepoint)
				{
					case U'"': print_to_stream(*stream_, R"(\")"sv); break;
					case U'\\': print_to_stream(*stream_, R"(\\)"sv); break;
					case U'\x7F': print_to_stream(*stream_, R"(\u007F)"sv); break;
					case U'\t': print_to_stream(*stream_, real_tabs_allowed ? "\t"sv : R"(\t)"sv); break;
					case U'\n': print_to_stream(*stream_, multi_line ? "\n"sv : R"(\n)"sv); break;
					default:
					{
						// control characters from lookup table
						if TOML_UNLIKELY(decoder.codepoint <= U'\x1F')
							print_to_stream(*stream_,
											control_char_escapes[static_cast<uint_least32_t>(decoder.codepoint)]);

						// escaped unicode characters
						else if (decoder.codepoint > U'\x7F'
								 && (!unicode_allowed || is_non_ascii_vertical_whitespace(decoder.codepoint)))
						{
							if (static_cast<uint_least32_t>(decoder.codepoint) > 0xFFFFu)
							{
								print_to_stream(*stream_, R"(\U)"sv);
								print_to_stream(*stream_,
												static_cast<uint_least32_t>(decoder.codepoint),
												value_flags::format_as_hexadecimal,
												8);
							}
							else
							{
								print_to_stream(*stream_, R"(\u)"sv);
								print_to_stream(*stream_,
												static_cast<uint_least32_t>(decoder.codepoint),
												value_flags::format_as_hexadecimal,
												4);
							}
						}

						// regular characters
						else
							print_to_stream(*stream_, cp_start, static_cast<size_t>(cp_end - cp_start));
					}
				}

				cp_start = cp_end;
			}
		}

		print_unformatted(multi_line ? R"(""")"sv : R"(")"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<std::string>& val)
	{
		print_string(val.get());
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<int64_t>& val)
	{
		naked_newline_ = false;

		if (*val >= 0 && !!int_format_mask_)
		{
			static constexpr auto value_flags_mask =
				value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;

			const auto fmt = val.flags() & value_flags_mask;
			switch (fmt)
			{
				case value_flags::format_as_binary:
					if (!!(int_format_mask_ & format_flags::allow_binary_integers))
					{
						print_to_stream(*stream_, "0b"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_octal:
					if (!!(int_format_mask_ & format_flags::allow_octal_integers))
					{
						print_to_stream(*stream_, "0o"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_hexadecimal:
					if (!!(int_format_mask_ & format_flags::allow_hexadecimal_integers))
					{
						print_to_stream(*stream_, "0x"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				default: break;
			}
		}

		// fallback to decimal
		print_to_stream(*stream_, *val);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<double>& val)
	{
		const std::string_view* inf_nan = nullptr;
		switch (fpclassify(*val))
		{
			case fp_class::neg_inf: inf_nan = &constants_->float_neg_inf; break;
			case fp_class::pos_inf: inf_nan = &constants_->float_pos_inf; break;
			case fp_class::nan: inf_nan = &constants_->float_nan; break;
			case fp_class::ok:
				print_to_stream(*stream_,
								*val,
								value_flags::none,
								!!(config_.flags & format_flags::relaxed_float_precision));
				break;
			default: TOML_UNREACHABLE;
		}

		if (inf_nan)
		{
			if (!!(config_.flags & format_flags::quote_infinities_and_nans))
				print_to_stream_bookended(*stream_, *inf_nan, '"');
			else
				print_to_stream(*stream_, *inf_nan);
		}

		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<bool>& val)
	{
		print_unformatted(*val ? constants_->bool_true : constants_->bool_false);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date_time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_value(const node& val_node, node_type type)
	{
		TOML_ASSUME(type > node_type::array);
		switch (type)
		{
			case node_type::string: print(*reinterpret_cast<const value<std::string>*>(&val_node)); break;
			case node_type::integer: print(*reinterpret_cast<const value<int64_t>*>(&val_node)); break;
			case node_type::floating_point: print(*reinterpret_cast<const value<double>*>(&val_node)); break;
			case node_type::boolean: print(*reinterpret_cast<const value<bool>*>(&val_node)); break;
			case node_type::date: print(*reinterpret_cast<const value<date>*>(&val_node)); break;
			case node_type::time: print(*reinterpret_cast<const value<time>*>(&val_node)); break;
			case node_type::date_time: print(*reinterpret_cast<const value<date_time>*>(&val_node)); break;
			default: TOML_UNREACHABLE;
		}
	}

#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

	TOML_EXTERNAL_LINKAGE
	bool formatter::dump_failed_parse_result()
	{
		if (result_ && !(*result_))
		{
			stream() << result_->error();
			return true;
		}
		return false;
	}

#else

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(const)
	bool formatter::dump_failed_parse_result()
	{
		return false;
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/toml_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_DISABLE_ARITHMETIC_WARNINGS;

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	size_t toml_formatter_count_inline_columns(const node& node, size_t line_wrap_cols) noexcept
	{
		switch (node.type())
		{
			case node_type::table:
			{
				auto& tbl = *reinterpret_cast<const table*>(&node);
				if (tbl.empty())
					return 2u;		// "{}"
				size_t weight = 3u; // "{ }"
				for (auto&& [k, v] : tbl)
				{
					weight += k.length() + toml_formatter_count_inline_columns(v, line_wrap_cols) + 2u; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::array:
			{
				auto& arr = *reinterpret_cast<const array*>(&node);
				if (arr.empty())
					return 2u;		// "[]"
				size_t weight = 3u; // "[ ]"
				for (auto& elem : arr)
				{
					weight += toml_formatter_count_inline_columns(elem, line_wrap_cols) + 2u; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::string:
			{
				// todo: proper utf8 decoding?
				// todo: tab awareness?
				auto& str = (*reinterpret_cast<const value<std::string>*>(&node)).get();
				return str.length() + 2u; // + ""
			}

			case node_type::integer:
			{
				auto val = (*reinterpret_cast<const value<int64_t>*>(&node)).get();
				if (!val)
					return 1u;
				size_t weight = {};
				if (val < 0)
				{
					weight += 1u;
					val *= -1;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(val))) + 1u;
			}

			case node_type::floating_point:
			{
				auto val = (*reinterpret_cast<const value<double>*>(&node)).get();
				if (val == 0.0)
					return 3u;		// "0.0"
				size_t weight = 2u; // ".0"
				if (val < 0.0)
				{
					weight += 1u;
					val *= -1.0;
				}
				return weight + static_cast<size_t>(log10(val)) + 1u;
				break;
			}

			case node_type::boolean: return 5u;
			case node_type::date: [[fallthrough]];
			case node_type::time: return 10u;
			case node_type::date_time: return 30u;
			case node_type::none: TOML_UNREACHABLE;
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_forces_multiline(const node& node, size_t line_wrap_cols, size_t starting_column_bias) noexcept
	{
		return (toml_formatter_count_inline_columns(node, line_wrap_cols) + starting_column_bias) >= line_wrap_cols;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_pending_table_separator()
	{
		if (pending_table_separator_)
		{
			print_newline(true);
			print_newline(true);
			pending_table_separator_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const key& k)
	{
		print_string(k.str(), false, true);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_inline(const table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted("{ "sv);

		bool first = false;
		for (auto&& [k, v] : tbl)
		{
			if (first)
				print_unformatted(", "sv);
			first = true;

			print(k);
			print_unformatted(" = "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: print_value(v, type);
			}
		}

		print_unformatted(" }"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		const auto original_indent = indent();
		const auto multiline	   = TOML_ANON_NAMESPACE::toml_formatter_forces_multiline(
			  arr,
			  120u,
			  indent_columns() * static_cast<size_t>(original_indent < 0 ? 0 : original_indent));

		print_unformatted("["sv);

		if (multiline)
		{
			if (original_indent < 0)
				indent(0);
			if (indent_array_elements())
				increase_indent();
		}
		else
			print_unformatted(' ');

		for (size_t i = 0; i < arr.size(); i++)
		{
			if (i > 0u)
			{
				print_unformatted(',');
				if (!multiline)
					print_unformatted(' ');
			}

			if (multiline)
			{
				print_newline(true);
				print_indent();
			}

			auto& v			= arr[i];
			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: print_value(v, type);
			}
		}
		if (multiline)
		{
			indent(original_indent);
			print_newline(true);
			print_indent();
		}
		else
			print_unformatted(' ');

		print_unformatted("]"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const table& tbl)
	{
		static constexpr auto is_non_inline_array_of_tables = [](auto&& nde) noexcept
		{
			auto arr = nde.as_array();
			return arr && arr->is_array_of_tables() && !arr->template get_as<table>(0u)->is_inline();
		};

		// values, arrays, and inline tables/table arrays
		for (auto&& [k, v] : tbl)
		{
			const auto type = v.type();
			if ((type == node_type::table && !reinterpret_cast<const table*>(&v)->is_inline())
				|| (type == node_type::array && is_non_inline_array_of_tables(v)))
				continue;

			pending_table_separator_ = true;
			print_newline();
			print_indent();
			print(k);
			print_unformatted(" = "sv);
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: print_value(v, type);
			}
		}

		const auto print_key_path = [&]()
		{
			size_t i{};
			for (const auto k : key_path_)
			{
				if (i++)
					print_unformatted('.');
				print(*k);
			}
		};

		// non-inline tables
		for (auto&& [k, v] : tbl)
		{
			const auto type = v.type();
			if (type != node_type::table || reinterpret_cast<const table*>(&v)->is_inline())
				continue;
			auto& child_tbl = *reinterpret_cast<const table*>(&v);

			// we can skip indenting and emitting the headers for tables that only contain other tables
			// (so we don't over-nest)
			size_t child_value_count{}; // includes inline tables and non-table arrays
			size_t child_table_count{};
			size_t child_table_array_count{};
			for (auto&& [child_k, child_v] : child_tbl)
			{
				TOML_UNUSED(child_k);
				const auto child_type = child_v.type();
				TOML_ASSUME(child_type != node_type::none);
				switch (child_type)
				{
					case node_type::table:
						if (reinterpret_cast<const table*>(&child_v)->is_inline())
							child_value_count++;
						else
							child_table_count++;
						break;

					case node_type::array:
						if (is_non_inline_array_of_tables(child_v))
							child_table_array_count++;
						else
							child_value_count++;
						break;

					default: child_value_count++;
				}
			}
			bool skip_self = false;
			if (child_value_count == 0u && (child_table_count > 0u || child_table_array_count > 0u))
				skip_self = true;

			key_path_.push_back(&k);

			if (!skip_self)
			{
				print_pending_table_separator();
				if (indent_sub_tables())
					increase_indent();
				print_indent();
				print_unformatted("["sv);
				print_key_path();
				print_unformatted("]"sv);
				pending_table_separator_ = true;
			}

			print(child_tbl);

			key_path_.pop_back();
			if (!skip_self && indent_sub_tables())
				decrease_indent();
		}

		// table arrays
		for (auto&& [k, v] : tbl)
		{
			if (!is_non_inline_array_of_tables(v))
				continue;
			auto& arr = *reinterpret_cast<const array*>(&v);

			if (indent_sub_tables())
				increase_indent();
			key_path_.push_back(&k);

			for (size_t i = 0; i < arr.size(); i++)
			{
				print_pending_table_separator();
				print_indent();
				print_unformatted("[["sv);
				print_key_path();
				print_unformatted("]]"sv);
				pending_table_separator_ = true;
				print(*reinterpret_cast<const table*>(&arr[i]));
			}

			key_path_.pop_back();
			if (indent_sub_tables())
				decrease_indent();
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		switch (auto source_type = source().type())
		{
			case node_type::table:
			{
				auto& tbl = *reinterpret_cast<const table*>(&source());
				if (tbl.is_inline())
					print_inline(tbl);
				else
				{
					decrease_indent(); // so root kvps and tables have the same indent
					print(tbl);
				}
				break;
			}

			case node_type::array: print(*reinterpret_cast<const array*>(&source())); break;

			default: print_value(source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/json_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted('{');

		if (indent_sub_tables())
			increase_indent();
		bool first = false;
		for (auto&& [k, v] : tbl)
		{
			if (first)
				print_unformatted(',');
			first = true;
			print_newline(true);
			print_indent();

			print_string(k.str(), false);
			print_unformatted(" : "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: print_value(v, type);
			}
		}
		if (indent_sub_tables())
			decrease_indent();
		print_newline(true);
		print_indent();

		print_unformatted('}');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		print_unformatted('[');
		if (indent_array_elements())
			increase_indent();
		for (size_t i = 0; i < arr.size(); i++)
		{
			if (i > 0u)
				print_unformatted(',');
			print_newline(true);
			print_indent();

			auto& v			= arr[i];
			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: print_value(v, type);
			}
		}
		if (indent_array_elements())
			decrease_indent();
		print_newline(true);
		print_indent();
		print_unformatted(']');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		switch (auto source_type = source().type())
		{
			case node_type::table: print(*reinterpret_cast<const table*>(&source())); break;
			case node_type::array: print(*reinterpret_cast<const array*>(&source())); break;
			default: print_value(source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/yaml_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_string(const value<std::string>& str)
	{
		if (str->empty())
		{
			base::print(str);
			return;
		}

		bool contains_newline = false;
		for (auto c = str->c_str(), e = str->c_str() + str->length(); c < e && !contains_newline; c++)
			contains_newline = *c == '\n';

		if (contains_newline)
		{
			print_unformatted("|-"sv);

			increase_indent();

			auto line_end  = str->c_str() - 1u;
			const auto end = str->c_str() + str->length();
			while (line_end != end)
			{
				auto line_start = line_end + 1u;
				line_end		= line_start;
				for (; line_end != end && *line_end != '\n'; line_end++)
					;

				if TOML_LIKELY(line_start != line_end || line_end != end)
				{
					print_newline();
					print_indent();
					print_unformatted(std::string_view{ line_start, static_cast<size_t>(line_end - line_start) });
				}
			}

			decrease_indent();
		}
		else
			print_string(*str, false, true);
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::table& tbl, bool parent_is_array)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		increase_indent();

		for (auto&& [k, v] : tbl)
		{
			if (!parent_is_array)
			{
				print_newline();
				print_indent();
			}
			parent_is_array = false;

			print_string(k.str(), false, true);
			print_unformatted(": "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				case node_type::string: print_yaml_string(*reinterpret_cast<const value<std::string>*>(&v)); break;
				default: print_value(v, type);
			}
		}

		decrease_indent();
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::array& arr, bool parent_is_array)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		increase_indent();

		for (auto&& v : arr)
		{
			if (!parent_is_array)
			{
				print_newline();
				print_indent();
			}
			parent_is_array = false;

			print_unformatted("- "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*reinterpret_cast<const table*>(&v), true); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v), true); break;
				case node_type::string: print_yaml_string(*reinterpret_cast<const value<std::string>*>(&v)); break;
				default: print_value(v, type);
			}
		}

		decrease_indent();
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		switch (auto source_type = source().type())
		{
			case node_type::table:
				decrease_indent(); // so root kvps and tables have the same indent
				print(*reinterpret_cast<const table*>(&source()));
				break;

			case node_type::array: print(*reinterpret_cast<const array*>(&source())); break;

			case node_type::string: print_yaml_string(*reinterpret_cast<const value<std::string>*>(&source())); break;

			default: print_value(source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

#endif // TOML_IMPLEMENTATION

TOML_POP_WARNINGS;

// macro hygiene
#if TOML_UNDEF_MACROS
#undef TOML_ABI_NAMESPACE_BOOL
#undef TOML_ABI_NAMESPACE_END
#undef TOML_ABI_NAMESPACE_START
#undef TOML_ABI_NAMESPACES
#undef TOML_ABSTRACT_BASE
#undef TOML_ALWAYS_INLINE
#undef TOML_ANON_NAMESPACE
#undef TOML_ANON_NAMESPACE_END
#undef TOML_ANON_NAMESPACE_START
#undef TOML_ARM
#undef TOML_ASSERT
#undef TOML_ASSERT_ASSUME
#undef TOML_ASSUME
#undef TOML_ASYMMETRICAL_EQUALITY_OPS
#undef TOML_ATTR
#undef TOML_CLANG
#undef TOML_CLOSED_ENUM
#undef TOML_CLOSED_FLAGS_ENUM
#undef TOML_COMPILER_EXCEPTIONS
#undef TOML_CONST_GETTER
#undef TOML_CONST_INLINE_GETTER
#undef TOML_CONSTRAINED_TEMPLATE
#undef TOML_CPP_VERSION
#undef TOML_DELETE_DEFAULTS
#undef TOML_DISABLE_ARITHMETIC_WARNINGS
#undef TOML_DISABLE_CODE_ANALYSIS_WARNINGS
#undef TOML_DISABLE_SPAM_WARNINGS
#undef TOML_DISABLE_SPAM_WARNINGS_CLANG_10
#undef TOML_DISABLE_SPAM_WARNINGS_CLANG_11
#undef TOML_DISABLE_SUGGEST_ATTR_WARNINGS
#undef TOML_DISABLE_SWITCH_WARNINGS
#undef TOML_DISABLE_WARNINGS
#undef TOML_EMPTY_BASES
#undef TOML_ENABLE_IF
#undef TOML_ENABLE_WARNINGS
#undef TOML_EVAL_BOOL_0
#undef TOML_EVAL_BOOL_1
#undef TOML_EXTERNAL_LINKAGE
#undef TOML_FLAGS_ENUM
#undef TOML_FLOAT_CHARCONV
#undef TOML_FLOAT128
#undef TOML_FLOAT16
#undef TOML_FP16
#undef TOML_GCC
#undef TOML_HAS_ATTR
#undef TOML_HAS_CHAR8
#undef TOML_HAS_CUSTOM_OPTIONAL_TYPE
#undef TOML_HAS_INCLUDE
#undef TOML_HAS_SSE2
#undef TOML_HAS_SSE4_1
#undef TOML_HIDDEN_CONSTRAINT
#undef TOML_ICC
#undef TOML_ICC_CL
#undef TOML_IMPL_NAMESPACE_END
#undef TOML_IMPL_NAMESPACE_START
#undef TOML_IMPLEMENTATION
#undef TOML_INCLUDE_WINDOWS_H
#undef TOML_INT_CHARCONV
#undef TOML_INT128
#undef TOML_INTELLISENSE
#undef TOML_INTERNAL_LINKAGE
#undef TOML_LANG_AT_LEAST
#undef TOML_LANG_EFFECTIVE_VERSION
#undef TOML_LANG_HIGHER_THAN
#undef TOML_LANG_UNRELEASED
#undef TOML_LAUNDER
#undef TOML_LIFETIME_HOOKS
#undef TOML_LIKELY
#undef TOML_LIKELY_CASE
#undef TOML_MAKE_FLAGS
#undef TOML_MAKE_FLAGS_
#undef TOML_MAKE_VERSION
#undef TOML_MSVC
#undef TOML_NAMESPACE
#undef TOML_NEVER_INLINE
#undef TOML_NODISCARD
#undef TOML_NODISCARD_CTOR
#undef TOML_OPEN_ENUM
#undef TOML_OPEN_FLAGS_ENUM
#undef TOML_PARSER_TYPENAME
#undef TOML_POP_WARNINGS
#undef TOML_PURE_GETTER
#undef TOML_PURE_INLINE_GETTER
#undef TOML_PUSH_WARNINGS
#undef TOML_REQUIRES
#undef TOML_SA_LIST_BEG
#undef TOML_SA_LIST_END
#undef TOML_SA_LIST_NEW
#undef TOML_SA_LIST_NXT
#undef TOML_SA_LIST_SEP
#undef TOML_SA_NATIVE_VALUE_TYPE_LIST
#undef TOML_SA_NEWLINE
#undef TOML_SA_NODE_TYPE_LIST
#undef TOML_SA_UNWRAPPED_NODE_TYPE_LIST
#undef TOML_SA_VALUE_EXACT_FUNC_MESSAGE
#undef TOML_SA_VALUE_FUNC_MESSAGE
#undef TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#undef TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
#undef TOML_SA_VALUE_MESSAGE_WSTRING
#undef TOML_SIMPLE_STATIC_ASSERT_MESSAGES
#undef TOML_TRIVIAL_ABI
#undef TOML_UINT128
#undef TOML_UNLIKELY
#undef TOML_UNLIKELY_CASE
#undef TOML_UNREACHABLE
#undef TOML_UNUSED
#endif

#endif // TOMLPLUSPLUS_H
