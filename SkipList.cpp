#include<random>
#include<string.h>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
template<typename K, typename V>
class Node {
public:
    Node() {}
    Node(const K& k, const V& v, int level, Node* next = nullptr);
    Node(int level);
    ~Node();
    K GetKey()const;
    V GetValue()const;
    void SetValue(const V& v);
    int level_;
    Node<K, V>** forward;
private:
    K k;
    V v;
};

template<typename K, typename V>
Node<K, V>::Node(const K& k, const V& v, int level, Node* next):k(k), v(v), level_(level) {
    this->forward = new Node<K, V>* [level_ + 1];
    for (int i = 0; i <= level_; ++i)this->forward[i] = next;
}
template<typename K, typename V>
Node<K, V>::Node(int level) : level_(level) {
    this->forward = new Node<K, V>* [level_ + 1];
    for (int i = 0; i <= level_; ++i)this->forward[i] = nullptr;
}
template<typename K, typename V>
Node<K, V>::~Node() {
    delete[] forward;
}
template<typename K, typename V>
K Node<K, V>::GetKey()const {
    return k;
}
template<typename K, typename V>
V Node<K, V>::GetValue()const {
    return v;
}
template<typename K, typename V>
void Node<K, V>::SetValue(const V& v) {
    this->v = v;
}

template<typename T>
struct Less {
    bool operator()(const T& a, const T& b) {
        return a < b;
    }
};
template<typename K, typename V, typename Comp = Less<K>>
class SkipList {
public:
    SkipList();
    SkipList(Comp less);
    ~SkipList();
    void Insert(const K& k, const V& v);
    void Delete(const K& k);
    Node<K, V>* Find(const K& k);
    void ShowList();
    size_t GetLength();
    void DumpFile(const char* dest);
    void LoadFile(const char* src);
    void Clear();
private:
    int GetRandomLevel();
    Node<K, V>* CreateNode(K k, V v, int level, Node<K, V>* next = nullptr);
    Node<K, V>* CreateNode(int level);
private:
    const int MAX_LEVEL = 30;
    int level_;
    Node<K, V>* head_;
    Comp less_;
    int length_;
};

template<typename K, typename V, typename Comp>
SkipList<K, V, Comp>::SkipList():less_{ Comp() } {
    srand(time(0));
    head_ = CreateNode(MAX_LEVEL);
    level_ = length_ = 0;
}
template<typename K, typename V, typename Comp>
SkipList<K, V, Comp>::SkipList(Comp less):less_{ less } {
    srand(time(0));
    head_ = CreateNode(MAX_LEVEL);
    level_ = length_ = 0;
}
template<typename K, typename V, typename Comp>
SkipList<K, V, Comp>::~SkipList() {
    delete head_;
}
template<typename K, typename V, typename Comp>
int SkipList<K, V, Comp>::GetRandomLevel() {
    int lv = 1;
    while (rand() % 2)lv++;
    if (lv > MAX_LEVEL)lv = MAX_LEVEL;
    if (lv > level_)level_ = lv;
    return lv;
}
template<typename K, typename V, typename Comp>
Node<K, V>* SkipList<K, V, Comp>::CreateNode(K k, V v, int level, Node<K, V>* next) {
    Node<K, V>* node = new Node<K, V>(k, v, level, next);
    return node;
}
template<typename K, typename V, typename Comp>
Node<K, V>* SkipList<K, V, Comp>::CreateNode(int level) {
    Node<K, V>* node = new Node<K, V>(level);
    return node;
}
template < typename K, typename V, typename Comp >
Node<K, V>* SkipList<K, V, Comp>::Find(const K& k) {
    auto current = head_;
    for (int i = level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && less_(current->forward[i]->GetKey(), k)) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current != nullptr && current->GetKey() == k) {
        return current;
    }
    else return nullptr;
}
template < typename K, typename V, typename Comp >
void SkipList<K, V, Comp>::Insert(const K& k, const V& v) {
    int lv = GetRandomLevel();
    Node<K, V>* inserted_node = CreateNode(k, v, lv);
    Node<K, V>** update = new Node<K, V>* [level_ + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (level_ + 1));
    auto current = head_;
    for (int i = level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && less_(current->forward[i]->GetKey(), k)) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current != nullptr && current->GetKey() == k) {
        std::cout << "Key :" << current->GetKey() << " exists\n";
        return;
    }
    for (int i = 0; i <= lv; ++i) {
        inserted_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = inserted_node;
    }
    std::cout << "Insert Key :" << inserted_node->GetKey() << " Value :" << inserted_node->GetValue() << '\n';
    length_++;
}
template < typename K, typename V, typename Comp >
void SkipList<K, V, Comp>::ShowList() {
    auto p = head_->forward[0];
    while (p != nullptr) {
        std::cout << p->GetKey() << ':' << p->GetValue() << '\n';
        p = p->forward[0];
    }
}
template < typename K, typename V, typename Comp >
size_t SkipList<K, V, Comp>::GetLength() {
    return length_;
}
template < typename K, typename V, typename Comp >
void SkipList<K, V, Comp>::Delete(const K& k) {
    Node<K, V>** update = new Node<K, V>* [level_ + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (level_ + 1));
    auto current = head_;
    for (int i = level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && less_(current->forward[i]->GetKey(), k)) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current == nullptr || current->GetKey() != k) {
        std::cout << "Key :" << k << " is not exist\n";
        return;
    }
    int lv = current->level_;
    for (int i = 0; i <= lv; ++i) {
        update[i]->forward[i] = update[i]->forward[i]->forward[i];
    }
    delete current;
    std::cout << "Key :" << k << " is deleted\n";
    length_--;
}
template<typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::DumpFile(const char* dest) {
    std::ofstream file_writer(dest);
    std::cout << "dump_file-----------------\n";
    Node<K, V>* current = this->head_->forward[0];
    while (current != nullptr) {
        file_writer << current->GetKey() << ":" << current->GetValue() << '\n';
        std::cout << current->GetKey() << ":" << current->GetValue() << '\n';
        current = current->forward[0];
    }
    file_writer.flush();
    file_writer.close();
    std::cout << "dump_file_end-----------------\n";
}
template<typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::LoadFile(const char* src) {
    std::ifstream file_reader(src);
    std::cout << "load_file-----------------\n";
    std::string line;
    while (std::getline(file_reader, line)) {
        int pos = line.find(':');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        std::stringstream ss;
        K k;
        V v;
        ss << key; ss >> k;
        ss.clear();
        ss << value; ss >> v;
        Insert(k, v);
    }
    file_reader.close();
    std::cout << "load_file_end-----------------\n";
}
template<typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::Clear() {
    auto p = head_;
    while (p != nullptr) {
        auto q = p->forward[0];
        delete p;
        p = q;
    }
    head_ = CreateNode(MAX_LEVEL);
    level_ = length_ = 0;
}
int main(int argc, char* argv[]) {
    // SkipList<int, int> sklist;
    // sklist.Insert(1, 2);
    // sklist.Insert(2, 3);
    // sklist.Delete(2);
    // sklist.Insert(3, 1);
    // sklist.Delete(3);
    // //sklist.Delete(3);
    // sklist.Insert(2, 100);
    // //sklist.Delete(1);
    // std::cout << sklist.Find(2)->GetValue() << '\n';
    // sklist.DumpFile("1.txt");
    // sklist.ShowList();
    // //sklist.Delete(4);
    // sklist.Clear();
    // sklist.LoadFile("1.txt");
    // sklist.ShowList();
    auto func = [](const std::string& a, const std::string& b) {
        return a.size() < b.size();
    };
    SkipList < std::string, std::string, decltype(func) > sklist(func);
    sklist.Insert(std::string("79000"), std::string("123"));
    sklist.Insert(std::string("1123"), std::string("123"));
    sklist.ShowList();
    sklist.DumpFile("2.txt");
    sklist.Clear();
    sklist.LoadFile("2.txt");
    sklist.ShowList();

    // SkipList<const char*, const char*>sklist;
    // sklist.Insert("123", "456");
    // sklist.Insert("123", "678");
    // sklist.ShowList();
}