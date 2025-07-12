import std;
using namespace std;

template<int SIZE = 10, typename KeyType, typename ValueType>
class HashTable {
    public:
        void insert(const KeyType& key, const& ValueType& val){
            int idx = hashFunction(key);
            
        }
        // insert
        // remove




    private:
        const int hashGroups = SIZE;
        list<pair<KeyType,ValueType>> hashTable[hashGroups];

        int hashFunction(const KeyType& key){
            return hash{}(key) % hashGroups;
        }
}

int main(){
    return 0;
}