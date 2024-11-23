#pragma once
#include <string_view>

constexpr std::string_view cpp_code_v = R"(
#include <print> // C++23

// main
auto main() -> int {
	/* hello world */
	std::println("hello world");
}

)";

constexpr std::string_view cmake_code_v = R"(
cmake_minimum_required(VERSION 3.23)

# project
project(foo)

add_executable(foo main.cpp)
)";
