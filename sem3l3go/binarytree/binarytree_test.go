package binarytree

import (
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
)

type BinaryTreeTestSuite struct {
	suite.Suite
	tree *BinaryTree
}

func (s *BinaryTreeTestSuite) SetupTest() {
	s.tree = NewBinaryTree()
}

func (s *BinaryTreeTestSuite) TearDownTest() {
	s.tree = nil
}

func (s *BinaryTreeTestSuite) TestInsertAndSearch() {
	s.tree.Insert("mango")
	s.tree.Insert("apple")
	s.tree.Insert("zebra")

	assert.True(s.T(), s.tree.Search("mango"))
	assert.True(s.T(), s.tree.Search("apple"))
	assert.True(s.T(), s.tree.Search("zebra"))
	assert.False(s.T(), s.tree.Search("none"))
}

func (s *BinaryTreeTestSuite) TestIsFull() {
	// Пустое дерево считается полным
	assert.True(s.T(), s.tree.IsFull(), "Empty tree should be full")

	// Только корень - полное дерево
	s.tree.Insert("root")
	assert.True(s.T(), s.tree.IsFull(), "Tree with only root should be full")

	// Добавляем левого потомка
	s.tree.Insert("apple") // apple < root, становится левым потомком
	// Теперь у root только левый потомок - дерево НЕ полное
	assert.False(s.T(), s.tree.IsFull(), "Tree with only left child should not be full")

	// Добавляем правого потомка
	s.tree.Insert("zebra") // zebra > root, становится правым потомком
	// Теперь у root оба потомка - проверяем дальше рекурсивно
	// У apple и zebra нет потомков, они листья - дерево полное
	assert.True(s.T(), s.tree.IsFull(), "Tree with two children at root and leaves should be full")

	// Очищаем дерево
	s.tree = NewBinaryTree()

	// Создаем дерево, где у узла только один потомок
	s.tree.Insert("root")
	s.tree.Insert("left")      // left < root
	s.tree.Insert("left.left") // left.left < left
	// root имеет только левого потомка, у которого есть потомок
	// Это дерево НЕ полное
	assert.False(s.T(), s.tree.IsFull(), "Tree with a node having only one child should not be full")
}

func (s *BinaryTreeTestSuite) TestFullTree() {
	// Создаем полное дерево
	//        50
	//       /  \
	//      30   70
	//     / \   / \
	//    20 40 60 80
	s.tree.Insert("50")
	s.tree.Insert("30")
	s.tree.Insert("70")
	s.tree.Insert("20")
	s.tree.Insert("40")
	s.tree.Insert("60")
	s.tree.Insert("80")

	assert.True(s.T(), s.tree.IsFull(), "Complete binary tree should be full")

	// Добавляем элемент, нарушающий полноту
	s.tree.Insert("55") // должен стать левым или правым потомком
	// Теперь у какого-то узла будет только один потомок
	assert.False(s.T(), s.tree.IsFull(), "Tree with a node having only one child should not be full")
}

func (s *BinaryTreeTestSuite) TestSkewedTree() {
	// Левостороннее дерево
	s.tree.Insert("d")
	s.tree.Insert("c")
	s.tree.Insert("b")
	s.tree.Insert("a")

	assert.False(s.T(), s.tree.IsFull(), "Left-skewed tree should not be full")

	// Правостороннее дерево
	tree2 := NewBinaryTree()
	tree2.Insert("a")
	tree2.Insert("b")
	tree2.Insert("c")
	tree2.Insert("d")

	assert.False(s.T(), tree2.IsFull(), "Right-skewed tree should not be full")
}

func (s *BinaryTreeTestSuite) TestTraversals() {
	// Строим дерево для тестирования обходов
	//        d
	//      /   \
	//     b     f
	//    / \   / \
	//   a   c e   g
	s.tree.Insert("d")
	s.tree.Insert("b")
	s.tree.Insert("f")
	s.tree.Insert("a")
	s.tree.Insert("c")
	s.tree.Insert("e")
	s.tree.Insert("g")

	// Проверяем наличие всех элементов
	assert.True(s.T(), s.tree.Search("a"))
	assert.True(s.T(), s.tree.Search("b"))
	assert.True(s.T(), s.tree.Search("c"))
	assert.True(s.T(), s.tree.Search("d"))
	assert.True(s.T(), s.tree.Search("e"))
	assert.True(s.T(), s.tree.Search("f"))
	assert.True(s.T(), s.tree.Search("g"))

	// Захватываем вывод для проверки методов обхода
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем методы обхода
	s.tree.PrintInorder()
	s.tree.PrintPreorder()
	s.tree.PrintPostorder()
	s.tree.PrintBFS()

	w.Close()
	os.Stdout = old
}

func (s *BinaryTreeTestSuite) TestEmptyTreeTraversals() {
	// Захватываем вывод для проверки методов обхода на пустом дереве
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем методы обхода на пустом дереве
	s.tree.PrintInorder()
	s.tree.PrintPreorder()
	s.tree.PrintPostorder()
	s.tree.PrintBFS()

	w.Close()
	os.Stdout = old
}

func (s *BinaryTreeTestSuite) TestFileIO() {
	filename := "test_tree.bin"
	defer os.Remove(filename)

	s.tree.Insert("banana")
	s.tree.Insert("apple")
	s.tree.Insert("cherry")
	err := s.tree.SaveToFile(filename)
	assert.NoError(s.T(), err)

	newTree := NewBinaryTree()
	err = newTree.LoadFromFile(filename)
	assert.NoError(s.T(), err)

	assert.True(s.T(), newTree.Search("banana"))
	assert.True(s.T(), newTree.Search("apple"))
	assert.True(s.T(), newTree.Search("cherry"))
}

func (s *BinaryTreeTestSuite) TestFileIOEmptyFilename() {
	// Сохранение с пустым именем файла
	err := s.tree.SaveToFile("")
	assert.NoError(s.T(), err)

	// Загрузка с пустым именем файла
	err = s.tree.LoadFromFile("")
	assert.NoError(s.T(), err)
}

func (s *BinaryTreeTestSuite) TestFileIONotFound() {
	// Загрузка несуществующего файла
	err := s.tree.LoadFromFile("nonexistent.bin")
	assert.Error(s.T(), err)
}

func (s *BinaryTreeTestSuite) TestRunBinaryTree() {
	// Сохраняем оригинальный stdout
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Создаем временный файл
	filename := "test_run.bin"
	defer os.Remove(filename)

	// Тестируем TINSERT
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TINSERT apple"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TINSERT banana"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TINSERT cherry"})

	// Тестируем TSEARCH
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TSEARCH apple"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TSEARCH orange"})

	// Тестируем TGET
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TGET apple"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TGET orange"})

	// Тестируем TFULL
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TFULL"})

	// Тестируем обходы
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TINORDER"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TPREORDER"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TPOSTORDER"})
	RunBinaryTree([]string{"prog", "--file", filename, "--query", "TBFS"})

	w.Close()
	os.Stdout = old
}

func (s *BinaryTreeTestSuite) TestRunBinaryTreeNoFile() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без файла
	RunBinaryTree([]string{"prog", "--query", "TINSERT apple"})
	RunBinaryTree([]string{"prog", "--query", "TSEARCH apple"})
	RunBinaryTree([]string{"prog", "--query", "TGET apple"})
	RunBinaryTree([]string{"prog", "--query", "TFULL"})
	RunBinaryTree([]string{"prog", "--query", "TINORDER"})

	w.Close()
	os.Stdout = old
}

func (s *BinaryTreeTestSuite) TestRunBinaryTreeInvalidCommand() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем неверную команду
	RunBinaryTree([]string{"prog", "--query", "INVALID"})

	w.Close()
	os.Stdout = old
}

func (s *BinaryTreeTestSuite) TestRunBinaryTreeNoArgs() {
	old := os.Stdout
	_, w, _ := os.Pipe()
	os.Stdout = w

	// Тестируем без аргументов
	RunBinaryTree([]string{})

	w.Close()
	os.Stdout = old
}

func TestBinaryTreeSuite(t *testing.T) {
	suite.Run(t, new(BinaryTreeTestSuite))
}

// Бенчмарки
func BenchmarkBinaryTreeInsert(b *testing.B) {
	tree := NewBinaryTree()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		tree.Insert("test")
	}
}

func BenchmarkBinaryTreeSearch(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 1000; i++ {
		tree.Insert("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		tree.Search("test")
	}
}

func BenchmarkBinaryTreeIsFull(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 100; i++ {
		tree.Insert("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		tree.IsFull()
	}
}

func BenchmarkBinaryTreeTraversal(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 100; i++ {
		tree.Insert("test")
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		tree.PrintInorder()
	}
}
