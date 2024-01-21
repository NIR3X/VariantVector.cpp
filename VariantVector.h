#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

class CVariantVector {
public:
	using Type = std::vector<std::variant<uint64_t, std::string, std::vector<uint8_t>>>;

	template<typename T>
	static consteval size_t GetTypeIndex() {
		return Type::value_type {std::in_place_type<T>}.index();
	}

	static std::vector<uint8_t> Pack(const CVariantVector::Type& variantVec);
	static std::optional<CVariantVector::Type> Unpack(const std::vector<uint8_t>& data);
};
