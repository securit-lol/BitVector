# BitVector

## English

Implementation of a dynamic array of boolean values where each element occupies exactly 1 bit.
The class implements container-like behavior and provides Random Access Iterators.

The project consists of a single header file: [BitVector/bitvector.h](BitVector/bitvector.h).
Tests are included in the repository and are built using CMake.

Supported operations
`push_back(val), pop_back(), operator[](pos), at(pos), size(), capacity(), empty(), reserve(n), shrink_to_fit(), resize(n, val=false), clear(), front(), back(), begin(), end(), cbegin(), cend()`

Short example

```cpp
#include "bitvector.h"
#include <iostream>

int main() {
	BoolVector<> bv; // empty vector
	bv.push_back(true);
	bv.push_back(false);
	bv.push_back(true);

	std::cout << "size: " << bv.size() << '\n';
	for (size_t i = 0; i < bv.size(); ++i) {
		std::cout << i << ": " << (bv[i] ? "1" : "0") << '\n';
	}

	// Iterators are supported
	size_t idx = 0;
	for (auto val : bv) {
		std::cout << "it[" << idx++ << "]=" << (val ? '1' : '0') << '\n';
	}

	return 0;
}
```

Tests do not cover every feature of the container. If you find a bug in the project, please open a PR.

---

## Русский

Реализация динамического массива булевых значений, где каждый элемент занимает ровно 1 бит.
Класс реализует поведение контейнера и предоставляет итераторы уровня Random Access Iterator.

Проект состоит из одного заголовочного файла: [BitVector/bitvector.h](BitVector/bitvector.h).
Также в репозитории присутствуют тесты, собираемые через CMake.

Поддерживаемые команды
`push_back(val), pop_back(), operator[](pos), at(pos), size(), capacity(), empty(), reserve(n), shrink_to_fit(), resize(n, val=false), clear(), front(), back(), begin(), end(), cbegin(), cend()`

Краткий пример использования

```cpp
#include "bitvector.h"
#include <iostream>

int main() {
	BoolVector<> bv; // пустой вектор
	bv.push_back(true);
	bv.push_back(false);
	bv.push_back(true);

	std::cout << "size: " << bv.size() << '\n';
	for (size_t i = 0; i < bv.size(); ++i) {
		std::cout << i << ": " << (bv[i] ? "1" : "0") << '\n';
	}

	// Можно использовать итераторы
	size_t idx = 0;
	for (auto val : bv) {
		std::cout << "it[" << idx++ << "]=" << (val ? '1' : '0') << '\n';
	}

	return 0;
}
```

Тесты не покрывают все возможности контейнера; если вы нашли ошибку в проекте - просьба открыть PR.

