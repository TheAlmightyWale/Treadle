#pragma once
#include <string>
#include <iostream>

inline auto ASSERT(bool condition, std::string const& msg) noexcept  {
		[[unlikely]]
		if (!condition) {
			std::cerr << msg << std::endl;
			exit(EXIT_FAILURE);
		}
}

inline auto FATAL(std::string const& msg) noexcept {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}