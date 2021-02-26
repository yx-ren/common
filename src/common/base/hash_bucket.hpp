#ifndef __HASHBUCKET_HPP__
#define __HASHBUCKET_HPP__

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

// -------------------- //

// hash functor
template<typename T>
class HashFunctor
{
public:
    HashFunctor() {}

    size_t operator()(const T& key)
    {
        return key;
    }
};

template<>
class HashFunctor<std::string>
{
public:
    HashFunctor() {}

    size_t operator()(const std::string& key)
    {
        return bkdr(key);
    }

    size_t operator()(const char* k)
    {
        std::string key(k);
        return bkdr(key);
    }

private:
    size_t bkdr(const std::string& key)
    {
        int seed    = 131;
        size_t hash = 0;
        for (std::string::const_iterator it = key.begin();
            it != key.end(); it++)
        {
            hash = hash * seed + *it;
        }

        return hash & 0x7fffffff;
    }
};

// -------------------- //

// node
template<typename K, typename V>
struct hash_node
{
    hash_node()
        : next(NULL), hash(0)
    {}

    hash_node(const K& _key, const V& _value)
        : m_val(std::make_pair(_key, _value))
        , next(NULL)
        , hash(0)
    {}

    std::pair<K, V> m_val;
    hash_node<K, V>* next;
    size_t hash;
};

// -------------------- //

// hash bucket
template<typename K, typename V, typename HashFun = HashFunctor<K> >
class hash_bucket
{
public:
    // forward declaration
    class hash_bucket_it;
    typedef hash_bucket_it iterator;

public:
    hash_bucket();
    ~hash_bucket();

    std::pair<iterator, bool> insert(const std::pair<K, V>& value);

    iterator find(const K& key);
    iterator begin(void);
    iterator end(void);

    void erase(const K& key);

    size_t size(void);

    bool empty(void);

    void dump(std::ostream& os = std::cout);

    V& operator[](const K& key);


    // -------------------- //

public:
    // iterator
    class hash_bucket_it : public std::iterator<std::input_iterator_tag, std::pair<K, V>* >
    {
    public:
        hash_bucket_it(void) : m_pnode(NULL), m_pvecTable(NULL) {}

        hash_bucket_it(hash_node<K, V>* pnode, std::vector<hash_node<K, V>*>* pvecTable) :
            m_pnode(pnode), m_pvecTable(pvecTable) {}

        hash_bucket_it(const hash_bucket_it& rhs) :
            m_pnode(rhs.m_pnode), m_pvecTable(rhs.m_pvecTable) {}

        hash_bucket_it& operator=(const hash_bucket_it& rhs)
        {
            m_pnode     = rhs.m_pnode;
            m_pvecTable = rhs.m_pvecTable;
            return *this;
        }

        // prefix increment
        hash_bucket_it& operator++(void)
        {
            increment();
            return *this;
        }

        // postfix increment
        const hash_bucket_it operator++(int)
        {
            hash_bucket_it tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const hash_bucket_it& rhs) const{return m_pnode == rhs.m_pnode && m_pvecTable == rhs.m_pvecTable;}
        bool operator!=(const hash_bucket_it& rhs) const{return !(m_pnode == rhs.m_pnode);}

        std::pair<K, V>* operator->(void) const{return &(m_pnode->m_val);}
        std::pair<K, V>& operator*(void) const{return m_pnode->m_val;}

    private:
        void increment(void)
        {
            // check input arguments
            if (m_pvecTable == NULL)
            {
                m_pnode = NULL;
                return;
            }

            if (m_pnode->hash >= m_pvecTable->size())
            {
                m_pnode = NULL;
                return;
            }

            // -------------------- //

            // on same hash index
            if (m_pnode->next != NULL)
            {
                m_pnode = m_pnode->next;
                return;
            }

            // on diff hash index
            for (typename std::vector<hash_node<K, V>*>::const_iterator it = m_pvecTable->begin() + m_pnode->hash + 1;
                it != m_pvecTable->end(); ++it)
            {
                hash_node<K, V>* pos = *it;
                while (pos != NULL)
                {
                    pos = pos->next;
                    if (pos != NULL)
                    {
                        m_pnode = pos;
                        return;
                    }
                }
            }

            // -------------------- //

            m_pnode = NULL;
            return;
        }

    private:
        hash_node<K, V>* m_pnode;
        std::vector<hash_node<K, V>*>* m_pvecTable;
    };

    // -------------------- //

    // forbiden copy-construct and assin
private:
    hash_bucket(const hash_bucket&);
    hash_bucket& operator=(const hash_bucket&);

private:
    bool expand_bucket();

    static size_t hash(const K& key, HashFun hf, size_t table_len);

    static size_t get_next_prime(size_t size);

private:
    std::vector<hash_node<K, V>*> m_vecTable;
    size_t m_nElementCount;
};

// -------------------- //

template<typename K, typename V, typename HashFun>
hash_bucket<K, V, HashFun>::hash_bucket()
    : m_nElementCount(0)
{
    expand_bucket();
}

template<typename K, typename V, typename HashFun>
hash_bucket<K, V, HashFun>::~hash_bucket()
{
    for (typename std::vector< hash_node<K, V>* >::iterator iter = m_vecTable.begin();
    iter != m_vecTable.end(); iter++)
    {
        hash_node<K, V>** iter_node = &(*iter);
        while (*iter_node != NULL)
        {
            hash_node<K, V>** del_node = iter_node;
            *iter_node = (*iter_node)->next;

            delete *del_node;
            *del_node = NULL;
        }
    }
}

template<typename K, typename V, typename HashFun>
std::pair<typename hash_bucket<K, V, HashFun>::iterator, bool> hash_bucket<K, V, HashFun>::insert(const std::pair<K, V>& value)
{
    iterator ret;
    if ((ret = find(value.first)) != end())
        return std::make_pair(ret, false);

    // -------------------- //

    // check expand
    if (m_nElementCount >= m_vecTable.size())
        expand_bucket();

    // none hash conflict
    size_t index = hash(value.first, HashFun(), m_vecTable.size());
    if (m_vecTable[index] == NULL)
    {
        hash_node<K, V>* head_node = new hash_node<K, V>;
        head_node->next = NULL;
        m_vecTable[index] = head_node;
    }

    // -------------------- //

    // hash conflict
    hash_node<K, V>* insert_node = new hash_node<K, V>;
    insert_node->m_val = std::make_pair(value.first, value.second);
    insert_node->hash  = index;

    insert_node->next = m_vecTable[index]->next;
    m_vecTable[index]->next = insert_node;
    m_nElementCount++;

    return std::make_pair(iterator(insert_node, &m_vecTable), true);
}

template<typename K, typename V, typename HashFun>
typename hash_bucket<K, V, HashFun>::iterator hash_bucket<K, V, HashFun>::find(const K& key)
{
    size_t index = hash(key, HashFun(), m_vecTable.size());
    hash_node<K, V>* iter = m_vecTable[index];

    if (iter != NULL && iter->next != NULL)
    {
        iter = iter->next; // jump the head node
        while (iter != NULL)
        {
            if (iter->m_val.first == key)
            {
                return iterator(iter, &m_vecTable);
            }

            iter = iter->next;
        }
    }

    return this->end();
}

template<typename K, typename V, typename HashFun>
typename hash_bucket<K, V, HashFun>::iterator hash_bucket<K, V, HashFun>::begin(void)
{
    for (typename std::vector<hash_node<K, V>*>::const_iterator it = m_vecTable.begin();
        it != m_vecTable.end(); ++it)
    {
        const hash_node<K, V>* pnode = *it;
        if (pnode != NULL && pnode->next != NULL)
        {
            return iterator(pnode->next, &m_vecTable);
        }
    }

    return this->end();
}

template<typename K, typename V, typename HashFun>
typename hash_bucket<K, V, HashFun>::iterator hash_bucket<K, V, HashFun>::end(void)
{
    return iterator();
}

template<typename K, typename V, typename HashFun>
void hash_bucket<K, V, HashFun>::erase(const K& key)
{
    size_t index = hash(key, HashFun(), m_vecTable.size());
    if (m_vecTable[index] == NULL)
        return;

    hash_node<K, V>* iter    = m_vecTable[index];
    hash_node<K, V>** head   = &(m_vecTable[index]);

    while (iter->next != NULL)
    {
        if (iter->next->m_val.first == key)
        {
            hash_node<K, V>** del_node = &(iter->next);
            iter->next = (*del_node)->next;

            delete *del_node;
            *del_node = NULL;
            m_nElementCount--;

            // last one, erase the head node
            if ((*head)->next == NULL)
            {
                delete *head;
                *head = NULL;
            }

            return;
        }

        iter = iter->next;
    }

    return;
}

template<typename K, typename V, typename HashFun>
size_t hash_bucket<K, V, HashFun>::size(void)
{
    return m_nElementCount;
}

template<typename K, typename V, typename HashFun>
bool hash_bucket<K, V, HashFun>::empty(void)
{
    return m_nElementCount == 0 ? true : false;
}

template<typename K, typename V, typename HashFun>
void hash_bucket<K, V, HashFun>::dump(std::ostream& os)
{
    if (!os)
        return;

    os << "\n// ---------- start dump hash_bucket info ---------- //\n";

    int free_count = 0;
    int used_count = 0;
    int index = 0;
    for (typename std::vector< hash_node<K, V>* >::const_iterator iter = m_vecTable.begin();
        iter != m_vecTable.end(); iter++, index++)
    {
        if (*iter == NULL || (*iter)->next == NULL)
        {
            free_count++;
            continue;
        }

        os << "index:[" << index << "]" << std::endl;
        const hash_node<K, V>* iter_node = m_vecTable[index];
        int same_key_count = 0;
        while (iter_node->next != NULL)
        {
            iter_node = iter_node->next;
            os << "    "
                << "key:[" << iter_node->m_val.first << "], "
                << "val:[" << iter_node->m_val.second << "], "
                << "hash:[" << iter_node->hash << "]" << std::endl;
            same_key_count++;
            used_count++;
        }
        os << "element_count:[" << same_key_count << "]" << std::endl << std::endl;
    }

    os << "element:[" << used_count << "], "
        << "free base:[" << free_count << "], "
        << "total base:[" << m_vecTable.size() << "]" << std::endl;

    os << "// ---------- end dump hash_bucket info ---------- //\n";

    return;
}

template<typename K, typename V, typename HashFun>
V& hash_bucket<K, V, HashFun>::operator[](const K& key)
{
    return (insert(std::make_pair(key, V()))).first->second;
}

template<typename K, typename V, typename HashFun>
bool hash_bucket<K, V, HashFun>::expand_bucket()
{
    if (m_nElementCount < m_vecTable.size())
        return true;

    int expand_size = get_next_prime(m_vecTable.size());
    std::vector< hash_node<K, V>* > vec_expand;
    vec_expand.resize(expand_size);
    for (typename std::vector< hash_node<K, V>* >::iterator iter = m_vecTable.begin();
    iter != m_vecTable.end(); iter++)
    {
        if (*iter == NULL)
            continue;

        // -------------------- //

        // traversal the link-list
        hash_node<K, V>* iter_node = (*iter)->next;
        while (iter_node != NULL)
        {
            // get Vid head_node
            size_t index = hash(iter_node->m_val.first, HashFun(), vec_expand.size());
            hash_node<K, V>* head_node = vec_expand[index];
            if (head_node == NULL)
            {
                head_node = new hash_node<K, V>;
                head_node->next = NULL;
                vec_expand[index] = head_node;
            }

            // set the node from old_table to new_table
            hash_node<K, V>* insert_node = iter_node;
            insert_node->hash = index;
            iter_node = iter_node->next;

            insert_node->next = head_node->next;
            head_node->next = insert_node;
        }

        (*iter)->next = NULL;
    }

    swap(m_vecTable, vec_expand);

    // release
    for (typename std::vector< hash_node<K, V>* >::iterator iter = vec_expand.begin();
        iter != vec_expand.end(); iter++)
    {
        hash_node<K, V>* iter_node = *iter;
        while (iter_node != NULL)
        {
            hash_node<K, V>* del_node = iter_node;
            iter_node = iter_node->next;

            delete del_node;
            del_node = NULL;
        }
    }

    return true;
}

template<typename K, typename V, typename HashFun>
size_t hash_bucket<K, V, HashFun>::hash(const K& key, HashFun hf, size_t table_len)
{
    return hf(key) % table_len;
}

template<typename K, typename V, typename HashFun>
size_t hash_bucket<K, V, HashFun>::get_next_prime(size_t size)
{
    static const unsigned long prime_table[] =
    {
        53ul, 97ul, 193ul, 389ul, 769ul,
        1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
        49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
        1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
        50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
        1610612741ul, 3221225473ul, 4294967291ul
    };

    int table_size = sizeof(prime_table) / sizeof(prime_table[0]);
    for (int i = 0; i < table_size; ++i)
    {
        if (prime_table[i] > size)
            return prime_table[i];
    }

    return prime_table[table_size - 1];
}
#endif
