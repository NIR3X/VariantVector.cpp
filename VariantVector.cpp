#include "VariantVector.h"
#include "VarsizedInt.cpp/VarsizedInt.h"

static inline void varsizedIntPack(std::vector<uint8_t>& packed, uint64_t x) {
	uint8_t bytes[CVarsizedInt::MaxSize] = {};
	int bytesSize = CVarsizedInt::Encode(bytes, x);
	packed.insert(packed.end(), bytes, bytes + bytesSize);
}

std::vector<uint8_t> CVariantVector::Pack(const CVariantVector::Type& variantVec) {
	std::vector<uint8_t> packed = {};
	varsizedIntPack(packed, variantVec.size());

	for (const auto& variant : variantVec) {
		uint64_t variantIndex = variant.index();
		varsizedIntPack(packed, variantIndex);

		switch (variantIndex) {
		case GetTypeIndex<uint64_t>():
			varsizedIntPack(packed, std::get<uint64_t>(variant));
			break;
		case GetTypeIndex<std::string>(): {
				const std::string& value = std::get<std::string>(variant);
				varsizedIntPack(packed, value.size());
				packed.insert(packed.end(), value.begin(), value.end());
			}
			break;
		case GetTypeIndex<std::vector<uint8_t>>(): {
				const std::vector<uint8_t>& value = std::get<std::vector<uint8_t>>(variant);
				varsizedIntPack(packed, value.size());
				packed.insert(packed.end(), value.begin(), value.end());
			}
			break;
		}
	}

	return packed;
}

static inline std::optional<uint64_t> varsizedIntUnpack(const std::vector<uint8_t>& data, uint64_t& offset) {
	if (data.size() < offset + 1) {
		return std::nullopt;
	}
	uint64_t bytesSize = CVarsizedInt::ParseSize((uint8_t*)&data[offset]);
	if (bytesSize < 0 || data.size() < offset + bytesSize) {
		return std::nullopt;
	}
	uint64_t x = CVarsizedInt::Decode((uint8_t*)&data[offset]);
	offset += bytesSize;
	return x;
}

std::optional<CVariantVector::Type> CVariantVector::Unpack(const std::vector<uint8_t>& data) {
	uint64_t offset = 0;
	std::optional<uint64_t> variantVecSize = varsizedIntUnpack(data, offset);
	if (!variantVecSize.has_value()) {
		return std::nullopt;
	}
	uint64_t variantVecSizeValue = *variantVecSize;

	CVariantVector::Type variantVec = {};
	for (uint64_t i = 0; i < variantVecSizeValue; ++i) {
		std::optional<uint64_t> variantIndex = varsizedIntUnpack(data, offset);
		if (!variantIndex.has_value()) {
			return std::nullopt;
		}

		switch (*variantIndex) {
		case GetTypeIndex<uint64_t>(): {
				std::optional<uint64_t> value = varsizedIntUnpack(data, offset);
				if (!value.has_value()) {
					return std::nullopt;
				}
				variantVec.push_back(*value);
			}
			break;
		case GetTypeIndex<std::string>(): {
				std::optional<uint64_t> valueSize = varsizedIntUnpack(data, offset);
				if (!valueSize.has_value()) {
					return std::nullopt;
				}
				uint64_t valueSizeValue = *valueSize;
				if (data.size() < offset + valueSizeValue) {
					return std::nullopt;
				}
				std::string value = std::string((char*)&data[offset], valueSizeValue);
				offset += valueSizeValue;
				variantVec.push_back(value);
			}
			break;
		case GetTypeIndex<std::vector<uint8_t>>(): {
				std::optional<uint64_t> valueSize = varsizedIntUnpack(data, offset);
				if (!valueSize.has_value()) {
					return std::nullopt;
				}
				uint64_t valueSizeValue = *valueSize;
				if (data.size() < offset + valueSizeValue) {
					return std::nullopt;
				}
				std::vector<uint8_t> value = std::vector<uint8_t>(&data[offset], &data[offset] + valueSizeValue);
				offset += valueSizeValue;
				variantVec.push_back(value);
			}
			break;
		}
	}

	return variantVec;
}
