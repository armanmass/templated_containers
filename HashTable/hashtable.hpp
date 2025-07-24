#include <iostream>
#include <list>
#include <utility>
#include <functional>
#include <concepts>

template<typename T>
concept Hashable = requires(T x) 
{
    { std::hash<T>{}(x) } -> std::convertible_to<size_t>;
};

template<typename T>
concept Valuable = std::movable<T>;


template<Hashable KeyType, Valuable ValueType, int SIZE = 10>
class HashTable 
{
    public:
        void insert(const KeyType& _key, const ValueType& _val)
        {
            int group = hashFunction(_key);
            bool collision = false;

            for(auto& kv_pair : hashTable[group])
            {
                if(_key == kv_pair.first)
                {
                    kv_pair.second = _val;
                    // std::cout << "ERR: Entry with key " << _key << " overwritten." << std::endl;
                    collision = true;
                    break;
                }
            }
            
            if(!collision)
            {
                hashTable[group].push_back({_key,_val});
                _size++;
            }

            return;
        }

        void remove(const KeyType& _key)
        {
            int group = hashFunction(_key);
            bool removed = false;
            auto it = hashTable[group].begin();

            for(; it != hashTable[group].end(); it++)
            {
                if(_key == it->first)
                {
                    hashTable[group].erase(it);
                    removed = true;
                    --_size;
                    break;
                }
            }

            if(!removed)
                throw std::out_of_range("Key not found in HashTable.");
        }

        ValueType get(const KeyType& _key)
        {
            int group = hashFunction(_key);
            auto it = hashTable[group].begin();

            for(; it != hashTable[group].end(); it++)
            {
                if(_key == it->first)
                    return it->second;
            }

            throw std::out_of_range("Key not found in HashTable.");
        }

        bool contains(const KeyType& _key)
        {
            int group = hashFunction(_key);
            auto it = hashTable[group].begin();

            for(; it != hashTable[group].end(); it++)
            {
                if(_key == it->first)
                    return true;
            }

            return false;
        }
        

        void printTable()
        {
            if(empty())
            {
                std::cout << "Table is empty." << std::endl;
                return;
            }

            std::cout << "Printing table: " << std::endl;
            for(auto& lst : hashTable)
            {
                for(auto& kv : lst)
                {
                    std::cout << "{ " << kv.first << " : " << kv.second << " }" << std::endl;
                }
            }
        }


        bool empty(){ return _size == 0; }

        int size(){ return _size; }

    private:
        static const int hashGroups = SIZE;
        std::list<std::pair<KeyType,ValueType>> hashTable[hashGroups];
        int _size = 0;

        int hashFunction(const KeyType& _key)
        {
            return std::hash<KeyType>{}(_key) % hashGroups;
        }
};