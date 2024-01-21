#include "VariantVector.h"
#include <cassert>
#include <iostream>

int main() {
	CVariantVector::Type v = {
		1ULL,
		"Hello",
		std::vector<uint8_t> {0x01, 0x02, 0x03},
	};

	std::vector<uint8_t> packed = CVariantVector::Pack(v);
	for (uint8_t byte : packed) {
		std::cout << std::hex << (int)byte << " ";
	}
	std::cout << std::endl;

	std::optional<CVariantVector::Type> unpacked = CVariantVector::Unpack(packed);
	assert(unpacked.has_value());

	CVariantVector::Type& unpackedValue = unpacked.value();
	assert(unpackedValue.size() == v.size());

	assert(std::holds_alternative<uint64_t>(unpackedValue[0]));
	assert(std::get<uint64_t>(unpackedValue[0]) == std::get<uint64_t>(v[0]));

	assert(std::holds_alternative<std::string>(unpackedValue[1]));
	assert(std::get<std::string>(unpackedValue[1]) == std::get<std::string>(v[1]));

	assert(std::holds_alternative<std::vector<uint8_t>>(unpackedValue[2]));
	assert(std::get<std::vector<uint8_t>>(unpackedValue[2]) == std::get<std::vector<uint8_t>>(v[2]));

	assert(unpackedValue == v);

	std::cout << "PASS" << std::endl;
}
