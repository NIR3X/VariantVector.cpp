# Variant Vector Serialization/Deserialization

This C++ code provides a mechanism for serializing and deserializing a variant vector (`CVariantVector::Type`). The variant vector can hold elements of different types, including `uint64_t`, `std::string`, and `std::vector<uint8_t>`.

## Serialization (Packing)

### `varsizedIntPack` Function
Encodes a `uint64_t` value using a variable-sized integer encoding and appends the encoded bytes to a vector.

### `CVariantVector::Pack` Function
Packs a `CVariantVector::Type` into a vector of bytes.
- The size of the variant vector is packed first using `varsizedIntPack`.
- For each variant in the vector:
  - The variant index is packed.
  - Depending on the variant index, the corresponding value is packed (either `uint64_t`, `std::string`, or `std::vector<uint8_t>`).

## Deserialization (Unpacking)

### `varsizedIntUnpack` Function
Decodes a variable-sized integer from a vector of bytes and updates the offset.

### `CVariantVector::Unpack` Function
Unpacks a vector of bytes into a `CVariantVector::Type`.
- Reads the size of the variant vector using `varsizedIntUnpack`.
- Iterates over the elements in the vector:
  - Reads the variant index using `varsizedIntUnpack`.
  - Based on the variant index, reads and adds the corresponding value to the variant vector.

## Usage

```cpp
#include "VariantVector.h"
#include <iostream>

int main() {
	// Creating a variant vector
	CVariantVector::Type v = {
		1ULL,
		"Hello",
		std::vector<uint8_t>{0x01, 0x02, 0x03},
	};

	// Packing the variant vector
	std::vector<uint8_t> packed = CVariantVector::Pack(v);

	// Displaying the packed bytes
	std::cout << "Packed Bytes: ";
	for (uint8_t byte : packed) {
		std::cout << std::hex << (int)byte << " ";
	}
	std::cout << std::endl;

	// Unpacking the bytes
	std::optional<CVariantVector::Type> unpacked = CVariantVector::Unpack(packed);

	// Displaying the unpacked values
	if (unpacked.has_value()) {
		std::cout << "Unpacked Values:" << std::endl;
		for (const auto& variant : *unpacked) {
			switch (variant.index()) {
			case CVariantVector::GetTypeIndex<uint64_t>():
				std::cout << "uint64_t: " << std::get<uint64_t>(variant) << std::endl;
				break;
			case CVariantVector::GetTypeIndex<std::string>():
				std::cout << "std::string: " << std::get<std::string>(variant) << std::endl;
				break;
			case CVariantVector::GetTypeIndex<std::vector<uint8_t>>():
				std::cout << "std::vector<uint8_t>: ";
				for (const auto& byte : std::get<std::vector<uint8_t>>(variant)) {
					std::cout << std::hex << (int)byte << " ";
				}
				std::cout << std::endl;
				break;
			}
		}
	} else {
		std::cerr << "Error unpacking the data." << std::endl;
	}
}
```

For additional information and usage examples, refer to `VariantVectorTest.cpp`.

## Notes

* The `GetTypeIndex` function is used to get a unique identifier for each type (`uint64_t`, `std::string`, `std::vector<uint8_t>`).
* `std::optional` is used to handle potential errors during deserialization.
* Ensure that dependencies are correctly implemented for the code to work as intended.

## License
[![GNU AGPLv3 Image](https://www.gnu.org/graphics/agplv3-155x51.png)](https://www.gnu.org/licenses/agpl-3.0.html)  

This program is Free Software: You can use, study share and improve it at your
will. Specifically you can redistribute and/or modify it under the terms of the
[GNU Affero General Public License](https://www.gnu.org/licenses/agpl-3.0.html) as
published by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
