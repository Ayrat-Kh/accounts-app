cmake_minimum_required(VERSION VERSION 3.5 FATAL_ERROR)

# boost
set(BOOST_INCLUDE_LIBRARIES beast asio json uuid callable_traits process filesystem random tti coroutine)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
  Boost_content
  GIT_REPOSITORY https://github.com/boostorg/boost
  GIT_TAG boost-1.84.0
  GIT_SUBMODULES "
  libs/coroutine
  libs/date_time
  libs/endian
  libs/atomic
  libs/dynamic_bitset
  libs/exception
  libs/integer
  libs/logic
  libs/pool
  libs/tti
  libs/smart_ptr
  libs/optional
  libs/type_traits
  libs/algorithm
  libs/tuple
  libs/numeric/conversion
  libs/function
  libs/functional
  libs/typeof
  libs/random
  libs/unordered
  libs/mpl
  libs/callable_traits
  libs/tokenizer
  libs/function_types
  libs/fusion
  libs/regex
  libs/array
  libs/static_string
  libs/utility
  libs/io
  libs/detail
  libs/concept_check
  libs/preprocessor
  libs/lexical_cast
  libs/iterator
  libs/range
  libs/conversion
  libs/context
  libs/bind
  libs/core
  libs/container
  libs/align
  libs/assert
  libs/config
  libs/mp11
  libs/json
  libs/describe
  libs/asio
  libs/beast
  libs/system
  libs/uuid
  libs/process
  libs/filesystem
  libs/intrusive
  libs/throw_exception
  libs/move
  libs/container_hash
  libs/static_assert
  libs/variant2
  libs/winapi
  libs/predef
  tools/cmake"
)
FetchContent_MakeAvailable(Boost_content)