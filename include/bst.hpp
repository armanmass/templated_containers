#include <iostream>
#include <utility>
#include <functional>
#include <concepts>

template<typename T>
struct Node 
{
    T value;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(const T& val) 
        : value(val) 
    { }
};

template<typename T>
class BST 
{
    public:
        void add(const T& val)
        {
            if(root == nullptr)
            {
                root = Node(val);
                return;
            }
            Node<T>* dummy = root;

            while(true)
            {
                if(val > dummy->value)
                {
                    if(dummy->right == nullptr)
                    {
                        dummy->right = Node(val);
                        return;
                    }
                    dummy = dummy->right;
                }
                else if(val < dummy->value)
                {
                    if(dummy->left == nullptr)
                    {
                        dummy->left = Node(val);
                        return;
                    }
                    dummy = dummy->left;
                }
                else
                {
                    return;
                }
            }
            return;
        }

        bool contains(const T& val){
            if(root == nullptr) return false;
            Node<T>* dummy = root;

            while(true)
            {
                if(val > dummy->value)
                {
                    if(dummy->right == nullptr) break;
                    dummy = dummy->right;
                }
                else if(val < dummy->value)
                {
                    if(dummy->left == nullptr) break;
                    dummy = dummy->left;
                }
                else
                {
                    return true;
                }
            }

            return false;
        }

        int size() { return size_; }
        bool empty() { return size_ == 0; }

    private:
        int size_ = 0;
        Node<T>* root = nullptr;
};

int main()
{
    return 0;
}