package dlinkedlist

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type DLinkedListTestSuite struct {
	suite.Suite
	list *DlinkedList
}

func (s *DLinkedListTestSuite) SetupTest() {
	s.list = NewDlinkedList()
}

func (s *DLinkedListTestSuite) TearDownTest() {
	s.list.Destroy()
}

func (s *DLinkedListTestSuite) TestAddToHead() {
	s.list.AddToHead("first")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)

	s.list.AddToHead("second")
	assert.Equal(s.T(), "second", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)
	assert.Equal(s.T(), "first", s.list.Head.Next.Data)
	assert.Equal(s.T(), "second", s.list.Head.Next.Prev.Data)
}

func (s *DLinkedListTestSuite) TestAddToTail() {
	s.list.AddToTail("first")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Data)

	s.list.AddToTail("second")
	assert.Equal(s.T(), "first", s.list.Head.Data)
	assert.Equal(s.T(), "second", s.list.Tail.Data)
	assert.Equal(s.T(), "second", s.list.Head.Next.Data)
	assert.Equal(s.T(), "first", s.list.Tail.Prev.Data)
}

func (s *DLinkedListTestSuite) TestRemoveFromHead() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveFromHead()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Data)
	assert.Nil(s.T(), s.list.Head.Prev)

	s.list.RemoveFromHead()
	s.list.RemoveFromHead()
	ok = s.list.RemoveFromHead()
	assert.False(s.T(), ok)
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func (s *DLinkedListTestSuite) TestRemoveFromTail() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveFromTail()
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Tail.Data)
	assert.Nil(s.T(), s.list.Tail.Next)

	s.list.RemoveFromTail()
	s.list.RemoveFromTail()
	ok = s.list.RemoveFromTail()
	assert.False(s.T(), ok)
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func (s *DLinkedListTestSuite) TestRemoveByValue() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveByValue("b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "a", s.list.Head.Data)
	assert.Equal(s.T(), "c", s.list.Head.Next.Data)
	assert.Equal(s.T(), "c", s.list.Tail.Data)
	assert.Equal(s.T(), "a", s.list.Tail.Prev.Data)

	ok = s.list.RemoveByValue("x")
	assert.False(s.T(), ok)
}

func (s *DLinkedListTestSuite) TestSearch() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")

	assert.True(s.T(), s.list.Search("a"))
	assert.True(s.T(), s.list.Search("b"))
	assert.False(s.T(), s.list.Search("c"))
}

func (s *DLinkedListTestSuite) TestAddBefore() {
	s.list.AddToTail("b")

	ok := s.list.AddBefore("b", "a")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "a", s.list.Head.Data)
	assert.Equal(s.T(), "b", s.list.Head.Next.Data)
	assert.Equal(s.T(), "a", s.list.Tail.Prev.Data)

	ok = s.list.AddBefore("x", "y")
	assert.False(s.T(), ok)
}

func (s *DLinkedListTestSuite) TestAddAfter() {
	s.list.AddToTail("a")

	ok := s.list.AddAfter("a", "b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Next.Data)
	assert.Equal(s.T(), "b", s.list.Tail.Data)
	assert.Equal(s.T(), "a", s.list.Tail.Prev.Data)

	ok = s.list.AddAfter("x", "y")
	assert.False(s.T(), ok)
}

func (s *DLinkedListTestSuite) TestRemoveBefore() {
	// Тест 1: удаление перед средним элементом
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveBefore("b")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "b", s.list.Head.Data)
	assert.Equal(s.T(), "c", s.list.Head.Next.Data)
	assert.Nil(s.T(), s.list.Head.Prev)

	// Очищаем список
	s.list.Destroy()
	s.list = NewDlinkedList()

	// Тест 2: удаление перед последним элементом
	s.list.AddToTail("x")
	s.list.AddToTail("y")
	s.list.AddToTail("z")

	ok = s.list.RemoveBefore("z")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "x", s.list.Head.Data)
	assert.Equal(s.T(), "z", s.list.Head.Next.Data)
	assert.Equal(s.T(), "z", s.list.Tail.Data)

	// Тест 3: попытка удалить перед несуществующим элементом
	ok = s.list.RemoveBefore("nonexistent")
	assert.False(s.T(), ok)

	// Тест 4: попытка удалить перед первым элементом
	ok = s.list.RemoveBefore("x")
	assert.False(s.T(), ok)
}

func (s *DLinkedListTestSuite) TestRemoveAfter() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.AddToTail("c")

	ok := s.list.RemoveAfter("a")
	assert.True(s.T(), ok)
	assert.Equal(s.T(), "c", s.list.Head.Next.Data)

	ok = s.list.RemoveAfter("c")
	assert.False(s.T(), ok)

	// Тест с пустым списком
	emptyList := NewDlinkedList()
	ok = emptyList.RemoveAfter("x")
	assert.False(s.T(), ok)
}

func (s *DLinkedListTestSuite) TestPrint() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Пустой список
	s.list.Print()

	// Список с элементами
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.Print()

	w.Close()
	os.Stdout = old
}

func (s *DLinkedListTestSuite) TestFileIO() {
	filename := "test_dlist.txt"
	defer os.Remove(filename)

	s.list.AddToTail("hello")
	s.list.AddToTail("world")
	err := s.list.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newList := NewDlinkedList()
	err = newList.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	assert.True(s.T(), newList.Search("hello"))
	assert.True(s.T(), newList.Search("world"))
}

func (s *DLinkedListTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем
	err := s.list.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем
	err = s.list.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *DLinkedListTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.list.LoadFromFile("nonexistent.txt")
	assert.Error(s.T(), err)
}

func (s *DLinkedListTestSuite) TestRunDLinkedList() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.txt"
	defer os.Remove(filename)

	// Тестируем DPUSH
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DPUSH first"})
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DPUSH second"})

	// Тестируем DAPPEND
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DAPPEND third"})

	// Тестируем DSEARCH
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DSEARCH first"})
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DSEARCH nonexistent"})

	// Тестируем DADDTO (AddBefore)
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DADDTO third before"})

	// Тестируем DADDAFTER
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DADDAFTER second after"})

	// Тестируем DREMOVETO (RemoveBefore)
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DREMOVETO after"})

	// Тестируем DREMOVEAFTER
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DREMOVEAFTER second"})

	// Тестируем DREMOVE
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DREMOVE first"})

	// Тестируем DREMOVEHEAD
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DREMOVEHEAD"})

	// Тестируем DREMOVETAIL
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DREMOVETAIL"})

	// Тестируем DPRINT
	RunDLinkedList([]string{"prog", "--file", filename, "--query", "DPRINT"})

	w.Close()
	os.Stdout = old
}

func (s *DLinkedListTestSuite) TestRunDLinkedListNoFile() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunDLinkedList([]string{"prog", "--query", "DPUSH first"})
	RunDLinkedList([]string{"prog", "--query", "DAPPEND second"})
	RunDLinkedList([]string{"prog", "--query", "DSEARCH first"})
	RunDLinkedList([]string{"prog", "--query", "DPRINT"})
	RunDLinkedList([]string{"prog", "--query", "DREMOVEHEAD"})
	RunDLinkedList([]string{"prog", "--query", "DREMOVETAIL"})

	w.Close()
	os.Stdout = old
}

func (s *DLinkedListTestSuite) TestRunDLinkedListInvalidCommand() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Неверная команда
	RunDLinkedList([]string{"prog", "--query", "INVALID"})

	w.Close()
	os.Stdout = old
}

func (s *DLinkedListTestSuite) TestRunDLinkedListNoArgs() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Без аргументов
	RunDLinkedList([]string{})

	w.Close()
	os.Stdout = old
}

func (s *DLinkedListTestSuite) TestDestroy() {
	s.list.AddToTail("a")
	s.list.AddToTail("b")
	s.list.Destroy()
	assert.Nil(s.T(), s.list.Head)
	assert.Nil(s.T(), s.list.Tail)
}

func TestDLinkedListSuite(t *testing.T) {
	suite.Run(t, new(DLinkedListTestSuite))
}

// Бенчмарки
func BenchmarkDLinkedListAddToTail(b *testing.B) {
	list := NewDlinkedList()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.AddToTail("test")
	}
	list.Destroy()
}

func BenchmarkDLinkedListAddToHead(b *testing.B) {
	list := NewDlinkedList()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.AddToHead("test")
	}
	list.Destroy()
}

func BenchmarkDLinkedListSearch(b *testing.B) {
	list := NewDlinkedList()
	for i := 0; i < 1000; i++ {
		list.AddToTail("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.Search("test")
	}
	list.Destroy()
}

func BenchmarkDLinkedListRemoveByValue(b *testing.B) {
	list := NewDlinkedList()
	for i := 0; i < 1000; i++ {
		list.AddToTail("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		list.RemoveByValue("test")
	}
	list.Destroy()
}
