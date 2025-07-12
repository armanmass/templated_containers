#include <iostream>
#include <list>
#include <utility>
#include <functional>

using namespace std;

template<typename KeyType, typename ValueType,int SIZE = 10>
class HashTable {
    public:
        void insert(const KeyType& _key, const ValueType& _val){
            int group = hashFunction(_key);
            bool collision = false;

            for(auto& kv_pair : hashTable[group]){
                if(_key == kv_pair.first){
                    kv_pair.second = _val;
                    cout << "ERR: Entry with key " << _key << " overwritten." << endl;
                    collision = true;
                    break;
                }
            }
            
            if(!collision){
                hashTable[group].push_back({_key,_val});
                _size++;
            }

            return;
        }

        void remove(const KeyType& _key){
            int group = hashFunction(_key);
            bool removed = true;
            auto it = hashTable[group].begin();

            for(; it != hashTable[group].end(); it++){
                if(_key == it->first){
                    hashTable[group].erase(it);
                    removed = true;
                    _size--;
                    break;
                }
            }
            
            if(!removed){
                cout << "ERR: Key [" << _key <<  "] not found in table. Nothing removed." << endl;
            }
                

            return;           
        }

        void printTable(){
            if(empty()){
                cout << "Table is empty." << endl;
                return;
            }

            cout << "Printing table: " << endl;
            for(auto& lst : hashTable){
                for(auto& kv : lst){
                    cout << "{ " << kv.first << " : " << kv.second << " }" << endl;
                }
            }

            return;
        }


        bool empty(){ return _size == 0; }

        int size(){ return _size; }

    private:
        static const int hashGroups = SIZE;
        list<pair<KeyType,ValueType>> hashTable[hashGroups];
        int _size = 0;

        int hashFunction(const KeyType& _key){
            return hash<KeyType>{}(_key) % hashGroups;
        }
};

int main(){
    HashTable<int,string> h;
    if(h.empty()){
        cout << "HashTable constructed and empty." << endl;
    }else{
        cout << "Incorrect implementation for HT or empty() member." << endl;
    }
    h.insert(13242,"test");
    h.remove(69);
    h.insert(231,"test2");
    h.insert(15234,"test3");
    h.insert(1234,"test4");
    h.insert(112367,"test5");
    h.insert(1,"1");
    h.insert(1,"2");
    h.remove(1234);
    h.remove(1234);
    h.remove(1234);

    cout << h.size() << " elements in table." << endl;

    h.printTable();

    return 0;
}