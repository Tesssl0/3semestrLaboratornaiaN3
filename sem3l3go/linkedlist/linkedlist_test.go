package linkedlist

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type LinkedListTestSuite struct {
	suite.Suite
	list *LinkedList
}

func (s *LinkedListTestSuite) SetupTest() {
	s.list = NewLinkedList()
}

func (s *LinkedListTestSuite) TearDownTest() {
	s.list.Destroy()
}

func (s *LinkedListTestSuite) TestAddToHead() {
	s.list.AddToHead("first")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)

	s.list.AddToHead("second")
	assert.Equal(s.T(), "second", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)
	assert.Equal(s.T(), "first", s.list.Head.Next.Data)
}

func (s *LinkedListTestSuite) TestAddToTail() {
	s.list.AddToTail("first")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)

	s.list.AddToTail("second")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "second", s.list.Tail.Data)
	assert.Equal(s.T(), "second", s.list.Head.Next.Data)
}

func (s *LinkedListTestSuite) TestRemoveFromHead() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveFromHead()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Data)

	s.list.RemoveFromHead()
	s.list.RemoveFromHead()
	ok = s.list.RemoveFromHead()
	assert.False(s.T(), ok)
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func (s *LinkedListTestSuite) TestRemoveFromTail() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveFromTail()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Tail.Data)

	s.list.RemoveFromTail()
	s.list.RemoveFromTail()
	ok = s.list.RemoveFromTail()
	assert.False(s.T(), ok)
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func (s *LinkedListTestSuite) TestRemoveByValue() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	// Удаление из середины
	ok := s.list.RemoveByValue("b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "a", s.list.Head.Data)
	assert.Equal(s.T(), "c", s.list.Head.Next.Data)
	assert.Equal(s.T(), "c", s.list.Tail.Data)

	// Удаление из головы
	ok = s.list.RemoveByValue("a")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "c", s.list.Head.Data)

	// Удаление из хвоста
	s.list.AddToTail("d")
	ok = s.list.RemoveByValue("d")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "c", s.list.Tail.Data)

	// Удаление несуществующего
	ok = s.list.RemoveByValue("x")
	assert.False(s.T(), ok)

	// Удаление из пустого списка
	emptyList := NewLinkedList()
	ok = emptyList.RemoveByValue("x")
	assert.False(s.T(), ok)
}

func (s *LinkedListTestSuite) TestSearch() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")

	assert.True(s.T(), s.list.Search("a"))
	assert.True(s.T(), s.list.Search("b"))
	assert.False(s.T(), s.list.Search("c"))
}

func (s *LinkedListTestSuite) TestAddBefore() {
	// Добавление перед головой
	s.list.AddToTail("b")
	ok := s.list.AddBefore("b", "a")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "a", s.list.Head.Data)
	assert.Equal(s.T(), "b", s.list.Head.Next.Data)

	// Добавление перед средним элементом
	s.list.AddToTail("d")
	ok = s.list.AddBefore("d", "c")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "c", s.list.Head.Next.Next.Data)

	// Добавление перед несуществующим
	ok = s.list.AddBefore("x", "y")
	assert.False(s.T(), ok)

	// Добавление в пустой список
	emptyList := NewLinkedList()
	ok = emptyList.AddBefore("x", "y")
	assert.False(s.T(), ok)
}

func (s *LinkedListTestSuite) TestAddAfter() {
	// Добавление после головы
	s.list.AddToTail("a")
	ok := s.list.AddAfter("a", "b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Next.Data)
	assert.Equal(s.T(), "b", s.list.Tail.Data)

	// Добавление после хвоста
	ok = s.list.AddAfter("b", "c")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "c", s.list.Tail.Data)

	// Добавление после среднего элемента
	ok = s.list.AddAfter("a", "x")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "x", s.list.Head.Next.Data)

	// Добавление после несуществующего
	ok = s.list.AddAfter("y", "z")
	assert.False(s.T(), ok)
}

func (s *LinkedListTestSuite) TestRemoveBefore() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")
	s.list.AddToTail("d")

	// Удаление перед вторым элементом
	ok := s.list.RemoveBefore("b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Data)

	// Удаление перед последним элементом
	ok = s.list.RemoveBefore("d")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "d", s.list.Tail.Data)

	// Удаление перед несуществующим
	ok = s.list.RemoveBefore("x")
	assert.False(s.T(), ok)

	// Удаление перед первым элементом
	ok = s.list.RemoveBefore("b")
	assert.False(s.T(), ok)

	// Удаление из пустого списка
	emptyList := NewLinkedList()
	ok = emptyList.RemoveBefore("x")
	assert.False(s.T(), ok)
}

/*
	func (s *LinkedListTestSuite) TestRemoveAfter() {
		s.list.AddToTail("a")
		s.list.AddToTail("b")
		s.list.AddToTail("c")
		s.list.AddToTail("d")

		// Удаление после первого элемента
		ok := s.list.RemoveAfter("a")
		assert.True(s.T(), ok)
		assert.Equal(s.T(), "c", s.list.Head.Next.Data)

		// Удаление после среднего элемента
		ok = s.list.RemoveAfter("c")
		assert.True(s.T(), ok)
		assert.Equal(s.T(), "d", s.list.Tail.Data)

		// Удаление после последнего элемента
		ok = s.list.RemoveAfter("d")
		assert.False(s.T(), ok)

		// Удаление после несуществующего
		ok = s.list.RemoveAfter("x")
		assert.False(s.T(), ok)

		// Удаление из пустого списка
		emptyList := NewLinkedList()
		ok = emptyList.RemoveAfter("x")
		assert.False(s.T(), ok)
	}
*/
func (s *LinkedListTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Пустой список
	s.list.Print()

	// Список с элементами
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")
	s.list.Print()

	w.Close()
	os.Stdout = old
	// Читаем вывод чтобы "использовать" r
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *LinkedListTestSuite) TestDestroy() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.Destroy()
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func (s *LinkedListTestSuite) TestFileIO() {
	filename := "test_list.txt"
	defer os.Remove(filename)

	s.list.AddToTail("hello")
	s.list.AddToTail("world")
	err := s.list.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newList := NewLinkedList()
	err = newList.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	assert.True(s.T(), newList.Search("hello"))
	assert.True(s.T(), newList.Search("world"))
}

func (s *LinkedListTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.list.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.list.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *LinkedListTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.list.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

func (s *LinkedListTestSuite) TestFileIOError() {
	// Попытка сохранения в недоступное место
	err := s.list.SaveToFile("/invalid/path/file.txt")
	assert.Error(s.T(), err)
}

func (s *LinkedListTestSuite) TestRunLinkedList() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем LPUSH
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LPUSH first"})
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LPUSH second"})

	// Тестируем LAPPEND
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LAPPEND third"})

	// Тестируем LSEARCH
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LSEARCH first"})
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LSEARCH nonexistent"})

	// Тестируем LADDTO (AddBefore)
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LADDTO third before"})

	// Тестируем LADDAFTER
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LADDAFTER second after"})

	// Тестируем LREMOVETO (RemoveBefore)
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LREMOVETO after"})

	// Тестируем LREMOVEAFTER
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LREMOVEAFTER second"})

	// Тестируем LREMOVE
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LREMOVE first"})

	// Тестируем LREMOVEHEAD
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LREMOVEHEAD"})

	// Тестируем LREMOVETAIL
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LREMOVETAIL"})

	// Тестируем LPRINT
	RunLinkedList([]string{"prog", "--file", filename, "--query", "LPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *LinkedListTestSuite) TestRunLinkedListNoFile() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunLinkedList([]string{"prog", "--query", "LPUSH first"})
	RunLinkedList([]string{"prog", "--query", "LAPPEND second"})
	RunLinkedList([]string{"prog", "--query", "LSEARCH first"})
	RunLinkedList([]string{"prog", "--query", "LADDTO second before"})
	RunLinkedList([]string{"prog", "--query", "LADDAFTER first after"})
	RunLinkedList([]string{"prog", "--query", "LREMOVETO after"})
	RunLinkedList([]string{"prog", "--query", "LREMOVEAFTER first"})
	RunLinkedList([]string{"prog", "--query", "LREMOVE first"})
	RunLinkedList([]string{"prog", "--query", "LREMOVEHEAD"})
	RunLinkedList([]string{"prog", "--query", "LREMOVETAIL"})
	RunLinkedList([]string{"prog", "--query", "LPRINT"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *LinkedListTestSuite) TestRunLinkedListInvalidCommand() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunLinkedList([]string{"prog", "--query", "INVALID"})

	// Команды без аргументов
	RunLinkedList([]string{"prog", "--query", "LPUSH"})
	RunLinkedList([]string{"prog", "--query", "LAPPEND"})
	RunLinkedList([]string{"prog", "--query", "LSEARCH"})
	RunLinkedList([]string{"prog", "--query", "LADDTO"})
	RunLinkedList([]string{"prog", "--query", "LADDAFTER"})
	RunLinkedList([]string{"prog", "--query", "LREMOVETO"})
	RunLinkedList([]string{"prog", "--query", "LREMOVEAFTER"})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func (s *LinkedListTestSuite) TestRunLinkedListNoArgs() {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunLinkedList([]string{})

	w.Close()
	os.Stdout = old
	// Используем r для подавления предупреждения
	go func() {
		buf := make([]byte, 1024)
		r.Read(buf)
	}()
}

func TestLinkedListSuite(t *testing.T) {
	suite.Run(t, new(LinkedListTestSuite))
}

// Бенчмарки
func BenchmarkLinkedListAddToTail(b *testing.B) {
	list := NewLinkedList()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.AddToTail("test")
	}
	list.Destroy()
}

func BenchmarkLinkedListAddToHead(b *testing.B) {
	list := NewLinkedList()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.AddToHead("test")
	}
	list.Destroy()
}

func BenchmarkLinkedListSearch(b *testing.B) {
	list := NewLinkedList()
	for i := 0; i < 1000; i++ {
		list.AddToTail("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.Search("test")
	}
	list.Destroy()
}

func BenchmarkLinkedListRemoveByValue(b *testing.B) {
	list := NewLinkedList()
	for i := 0; i < 1000; i++ {
		list.AddToTail("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.RemoveByValue("test")
	}
	list.Destroy()
}

func BenchmarkLinkedListAddBefore(b *testing.B) {
	list := NewLinkedList()
	list.AddToTail("target")
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.AddBefore("target", "value")
	}
	list.Destroy()
}
