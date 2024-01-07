class Number:
    def __init__(self, n):
        self._n = n
    
    def __add__(self, other):
        return Number(self._n + other.getN())
    
    def getN(self):
        return self._n
    
n1 = Number(3)
n2 = Number(5)
n3 = Number(6)
n4 = Number(9)

n5 = n1.__add__(n2).__add__(n3).__add__(n4)

print(n5.getN())

#########################################################

class TreeNode:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

class BinaryTree:
    def __init__(self):
        self.root = None

    def insert(self, value):
        if self.root == None:
            self.root = TreeNode(value)
        else:
            self._insert_recursive(self.root, value)

    def _insert_recursive(self, current_node, value):
        if value < current_node.value:
            if current_node.left == None:
                current_node.left = TreeNode(value)
            else:
                self._insert_recursive(current_node.left, value)
        elif value > current_node.value:
            if current_node.right == None:
                current_node.right = TreeNode(value)
            else:
                self._insert_recursive(current_node.right, value)

    def inorder_traversal(self, node):
        if node == None:
            node = self.root
        
        if node.left:
            self.inorder_traversal(node.left)
        
        print(node.value)
        
        if node.right:
            self.inorder_traversal(node.right)

def main():
    tree = BinaryTree()
    
    # Insert values into the binary tree
    tree.insert(5)
    tree.insert(3)
    tree.insert(8)
    tree.insert(2)
    tree.insert(4)
    tree.insert(7)
    tree.insert(9)

    # Perform inorder traversal by printing values
    print("Inorder traversal:")
    tree.inorder_traversal(None)  # Prints values in ascending order

main()
