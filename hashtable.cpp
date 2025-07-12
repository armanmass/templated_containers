import std;
using namespace std;

template<int SIZE = 10, typename KeyType, typename ValueType>
class HashTable {
    public:
        void insert(const KeyType& _key, const& ValueType& _val){
            int group = hashFunction(_key);
            bool collision = false;

            for(auto& kv_pair : hashTable[group]){
                if(_key == kv_pair.first){
                    kv_pair.second = _val;
                    cout << "[ERR] Entry with key " << _key << " overwritten." << endl;
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

            for(auto& kv_pair : hashTable[group]){
                if(_key == kv_pair.first){
                    hashTable[group].erase(kv_pair);
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
        const int hashGroups = SIZE;
        list<pair<KeyType,ValueType>> hashTable[hashGroups];
        int _size = 0;

        int hashFunction(const KeyType& _key){
            return hash{}(_key) % hashGroups;
        }
}

int main(){
    HashTable<101,int,string> h;
    if(h.empty()){
        cout << "HashTable constructed and empty." << endl;
    }else{
        cout << "Incorrect implementation for HT or empty() member." << endl;
    }
    h.insert(1,"test");

    return 0;
}