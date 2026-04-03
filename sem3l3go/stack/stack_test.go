package stack

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type StackTestSuite struct {
	suite.Suite
	stack *Stack
}

func (s *StackTestSuite) SetupTest() {
	s.stack = NewStack()
}

func (s *StackTestSuite) TearDownTest() {
	s.stack.Destroy()
}

func (s *StackTestSuite) TestPush() {
	s.stack.Push("first")
	assert.Equal(s.T(), "first", s.stack.Top.Data)

	s.stack.Push("second")
	assert.Equal(s.T(), "second", s.stack.Top.Data)
	assert.Equal(s.T(), "first", s.stack.Top.Next.Data)
}

func (s *StackTestSuite) TestPop() {
	// Поп из пустого стека
	ok := s.stack.Pop()
	assert.False(s.T(), ok)

	s.stack.Push("a")
	s.stack.Push("b")
	s.stack.Push("c")

	ok = s.stack.Pop()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.stack.Top.Data)

	ok = s.stack.Pop()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "a", s.stack.Top.Data)

	ok = s.stack.Pop()
	assert.True(s.T(), ok)
	assert.Nil(s.T(), s.stack.Top)

	ok = s.stack.Pop()
	assert.False(s.T(), ok)
}

func (s *StackTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Пустой стек
	s.stack.Print()

	// Стек с элементами
	s.stack.Push("a")
	s.stack.Push("b")
	s.stack.Push("c")
	s.stack.Print()

	w.Close()
	os.Stdout = old
	// Читаем вывод чтобы "использовать" r
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *StackTestSuite) TestDestroy() {
	s.stack.Push("a")
	s.stack.Push("b")
	s.stack.Destroy()
	assert.Nil(s.T(), s.stack.Top)
}

func (s *StackTestSuite) TestFileIO() {
	filename := "test_stack.txt"
	defer os.Remove(filename)

	s.stack.Push("first")
	s.stack.Push("second")
	s.stack.Push("third")
	err := s.stack.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newStack := NewStack()
	err = newStack.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	// Проверяем порядок (LIFO)
	assert.Equal(s.T(), "third", newStack.Top.Data)
	assert.Equal(s.T(), "second", newStack.Top.Next.Data)
	assert.Equal(s.T(), "first", newStack.Top.Next.Next.Data)
}

func (s *StackTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.stack.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.stack.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *StackTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.stack.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

func (s *StackTestSuite) TestFileIOError() {
	// Попытка сохранения в недоступное место
	err := s.stack.SaveToFile("/invalid/path/file.txt")
	assert.Error(s.T(), err)
}

func (s *StackTestSuite) TestFileIOEmptyFile() {
	// Создаем пустой файл
	filename := "empty.txt"
	defer os.Remove(filename)

	file, _ := os.Create(filename)
	file.Close()

	newStack := NewStack()
	err := newStack.LoadFromFile(filename)
	assert.NoError(s.T(), err)
	assert.Nil(s.T(), newStack.Top)
}

func (s *StackTestSuite) TestRunStack() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем SPUSH
	RunStack([]string{"prog", "--file", filename, "--query", "SPUSH first"})
	RunStack([]string{"prog", "--file", filename, "--query", "SPUSH second"})
	RunStack([]string{"prog", "--file", filename, "--query", "SPUSH third"})

	// Тестируем SPRINT
	RunStack([]string{"prog", "--file", filename, "--query", "SPRINT"})

	// Тестируем SPOP
	RunStack([]string{"prog", "--file", filename, "--query", "SPOP"})
	RunStack([]string{"prog", "--file", filename, "--query", "SPOP"})

	// Проверяем после удалений
	RunStack([]string{"prog", "--file", filename, "--query", "SPRINT"})

	// Удаляем последний элемент
	RunStack([]string{"prog", "--file", filename, "--query", "SPOP"})

	// Попытка удалить из пустого стека
	RunStack([]string{"prog", "--file", filename, "--query", "SPOP"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *StackTestSuite) TestRunStackNoFile() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunStack([]string{"prog", "--query", "SPUSH first"})
	RunStack([]string{"prog", "--query", "SPUSH second"})
	RunStack([]string{"prog", "--query", "SPRINT"})
	RunStack([]string{"prog", "--query", "SPOP"})
	RunStack([]string{"prog", "--query", "SPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *StackTestSuite) TestRunStackInvalidCommand() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunStack([]string{"prog", "--query", "INVALID"})

	// Команда без аргумента
	RunStack([]string{"prog", "--query", "SPUSH"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *StackTestSuite) TestRunStackNoArgs() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunStack([]string{})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func TestStackSuite(t *testing.T) {
	suite.Run(t, new(StackTestSuite))
}

// Бенчмарки
func BenchmarkStackPush(b *testing.B) {
	stack := NewStack()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		stack.Push("test")
	}
	stack.Destroy()
}

func BenchmarkStackPop(b *testing.B) {
	stack := NewStack()
	for i := 0; i < b.N; i++ {
		stack.Push("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		stack.Pop()
	}
	stack.Destroy()
}

func BenchmarkStackPrint(b *testing.B) {
	stack := NewStack()
	for i := 0; i < 100; i++ {
		stack.Push("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		stack.Print()
	}
	stack.Destroy()
}

func BenchmarkStackFileIO(b *testing.B) {
	filename := "bench_stack.txt"
	defer os.Remove(filename)

	stack := NewStack()
	for i := 0; i < 100; i++ {
		stack.Push("test")
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		stack.SaveToFile(filename)
		stack.LoadFromFile(filename)
	}
	stack.Destroy()
}
