.PHONY: all build test test-short test-verbose test-race test-cover clean run help coverage-report \
        install-deps show-coverage bench test-bench test-bench-prof profile-cpu profile-mem fmt \
        lint vet mod-tidy mod-verify

# Имя проекта
PROJECT_NAME := datastructures
GO_PACKAGES := ./array ./binarytree ./dlinkedlist ./hashtable ./linkedlist ./queue ./stack ./serialization

# Определение ОС
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

# Отключаем цвета на Windows (или используем echo -e для Git Bash)
ifeq ($(UNAME_S),Windows)
    # Для Git Bash/MINGW64 используем ANSI-цвета с правильным экранированием
    GREEN := \033[0;32m
    YELLOW := \033[1;33m
    RED := \033[0;31m
    NC := \033[0m
    # Используем bash для команд
    SHELL := bash
else
    GREEN := \033[0;32m
    YELLOW := \033[1;33m
    RED := \033[0;31m
    NC := \033[0m
endif

all: test build

build:
	@printf "$(GREEN)Сборка проекта...$(NC)\n"
	@mkdir -p bin 2>/dev/null || true
	go build -o bin/$(PROJECT_NAME).exe main.go
	@printf "$(GREEN)Сборка завершена: bin/$(PROJECT_NAME).exe$(NC)\n"

run:
	@printf "$(GREEN)Запуск программы...$(NC)\n"
	go run main.go

test:
	@printf "$(GREEN)Запуск тестов...$(NC)\n"
	go test $(GO_PACKAGES) -v -count=1

test-short:
	@printf "$(GREEN)Запуск быстрых тестов...$(NC)\n"
	go test $(GO_PACKAGES) -short -count=1

test-verbose:
	@printf "$(GREEN)Запуск тестов с детальным выводом...$(NC)\n"
	go test $(GO_PACKAGES) -v -count=1

test-race:
	@printf "$(GREEN)Запуск тестов с детектором гонок...$(NC)\n"
	go test $(GO_PACKAGES) -race -count=1

test-cover:
	@printf "$(GREEN)Запуск тестов с покрытием...$(NC)\n"
	mkdir -p coverage
	go test $(GO_PACKAGES) -coverprofile=coverage/coverage.out -covermode=atomic
	@printf "$(GREEN)Статистика покрытия:$(NC)\n"
	go tool cover -func=coverage/coverage.out
	@printf "$(GREEN)Создание HTML отчета...$(NC)\n"
	go tool cover -html=coverage/coverage.out -o coverage/coverage.html
	@printf "$(GREEN)Отчет о покрытии сохранен в coverage/coverage.html$(NC)\n"

coverage-report: test-cover
	@printf "$(GREEN)Открытие отчета о покрытии...$(NC)\n"
	start coverage/coverage.html

test-bench:
	@printf "$(GREEN)Запуск бенчмарков...$(NC)\n"
	go test $(GO_PACKAGES) -bench=. -benchmem

test-bench-prof:
	@printf "$(GREEN)Запуск бенчмарков с профилированием...$(NC)\n"
	mkdir -p profile
	go test $(GO_PACKAGES) -bench=. -benchmem -cpuprofile=profile/cpu.out -memprofile=profile/mem.out
	@printf "$(GREEN)Профили сохранены в profile/$(NC)\n"

fmt:
	@printf "$(GREEN)Форматирование кода...$(NC)\n"
	go fmt $(GO_PACKAGES)

lint:
	@printf "$(GREEN)Запуск линтера...$(NC)\n"
	golangci-lint run ./...

vet:
	@printf "$(GREEN)Запуск go vet...$(NC)\n"
	go vet $(GO_PACKAGES)

mod-tidy:
	@printf "$(GREEN)Обновление зависимостей...$(NC)\n"
	go mod tidy

mod-verify:
	@printf "$(GREEN)Проверка зависимостей...$(NC)\n"
	go mod verify

clean:
	@printf "$(RED)Очистка...$(NC)\n"
	rm -rf bin coverage profile *.txt *.bin 2>/dev/null || true
	@printf "$(GREEN)Очистка завершена$(NC)\n"

help:
	@echo "Доступные команды:"
	@echo "  make build            - сборка проекта"
	@echo "  make run              - запуск программы"
	@echo "  make test             - запуск всех тестов"
	@echo "  make test-short       - запуск быстрых тестов"
	@echo "  make test-verbose     - запуск тестов с детальным выводом"
	@echo "  make test-race        - запуск тестов с детектором гонок"
	@echo "  make test-cover       - запуск тестов с покрытием"
	@echo "  make coverage-report  - открыть отчет о покрытии"
	@echo "  make test-bench       - запуск бенчмарков"
	@echo "  make fmt              - форматирование кода"
	@echo "  make lint             - запуск линтера"
	@echo "  make vet              - запуск go vet"
	@echo "  make mod-tidy         - обновление зависимостей"
	@echo "  make clean            - очистка временных файлов"

install-deps:
	@printf "$(GREEN)Установка зависимостей...$(NC)\n"
	go get -u github.com/stretchr/testify
	go mod tidy

show-coverage:
	@printf "$(GREEN)Текущее покрытие:$(NC)\n"
	go tool cover -func=coverage/coverage.out 2>/dev/null || echo "Сначала запустите make test-cover"

bench:
	@printf "$(GREEN)Запуск бенчмарков...$(NC)\n"
	go test $(GO_PACKAGES) -bench=. -benchmem -count=5