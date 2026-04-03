# ================================
#   Makefile для Laba3sem2v2
# ================================

CXX       := g++
CXXFLAGS  := -std=c++17 -g -O0 --coverage -I. -I"D:/boosttesp/boost_1_90_0" -mconsole -Wall -Wextra -Wpedantic
LDFLAGS   := --coverage -mconsole -L"D:/boosttesp/boost_1_90_0/stage/lib"

# Библиотеки Boost - выбираем правильную версию
# Для 64-битной релизной версии:
BOOST_LIBS := -lboost_unit_test_framework-vc143-mt-x64-1_90
# Для 32-битной релизной версии (раскомментируйте если нужно):
# BOOST_LIBS := -lboost_unit_test_framework-vc143-mt-x32-1_90
# Для отладочной версии (раскомментируйте если нужно):
# BOOST_LIBS := -lboost_unit_test_framework-vc143-mt-gd-x64-1_90

BUILD_DIR  := build
COV_DIR    := coverage_report
LINT_DIR   := lint_reports

SRCS := \
    array.cpp \
    binaryTree.cpp \
    dlinkedList.cpp \
    hashTable.cpp \
    linkedList.cpp \
    queue.cpp \
    stack.cpp \
    serialization.cpp \
    test_main.cpp

# Все исходники для линтера
ALL_SRCS := $(SRCS) $(wildcard *.h)

OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

TARGET := Laba3sem2v2.exe

# -------------------------------
#   Основные цели
# -------------------------------

all: $(TARGET)
	@echo "Сборка завершена!"

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(BOOST_LIBS)
	@echo "Исполняемый файл создан: $@"

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# -------------------------------
#   Линтер cppcheck
# -------------------------------

.PHONY: lint
lint: lint-cppcheck
	@echo "Линтер выполнен!"

.PHONY: lint-cppcheck
lint-cppcheck:
	@echo "Запуск cppcheck..."
	mkdir -p $(LINT_DIR)
	"/c/Program Files/Cppcheck/cppcheck.exe" --enable=all \
		--suppress=missingIncludeSystem \
		--suppress=unmatchedSuppression \
		-I. -I"D:/boosttesp/boost_1_90_0" \
		--std=c++17 \
		--output-file=$(LINT_DIR)/cppcheck.log \
		--verbose \
		$(ALL_SRCS) 2>&1
	@echo "Отчет cppcheck сохранен в $(LINT_DIR)/cppcheck.log"

# -------------------------------
#   Тесты
# -------------------------------

test: $(TARGET)
	./$(TARGET)

# -------------------------------
#   Покрытие
# -------------------------------

coverage: clean_coverage $(TARGET)
	./$(TARGET)
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info "*boost*" "*test_main.cpp" -o coverage.info
	genhtml coverage.info --output-directory $(COV_DIR)
	@echo "========================================"
	@echo "  Отчёт покрытия создан: $(COV_DIR)/index.html"
	@echo "========================================"
# -------------------------------
#   Очистка
# -------------------------------

clean:
	rm -rf $(BUILD_DIR) *.exe *.gcda *.gcno *.gcov coverage.info $(LINT_DIR)
	@echo "Очистка завершена!"

clean_coverage:
	rm -rf $(COV_DIR) coverage.info *.gcda *.gcno *.gcov

clean_lint:
	rm -rf $(LINT_DIR)

.PHONY: all test coverage clean clean_coverage lint lint-cppcheck clean_lint