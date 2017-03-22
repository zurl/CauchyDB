#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;
const int BLOCK_SIZE = 8 + 16 * 5;
//const int NODE_SIZE = 5;
//const int NODE_SIZE_HALF = (NODE_SIZE + 1) / 2; // 2
// 4 bytes (int) 4 bytes(float) 4 bytes char???fucking fuck
// NODE_SIZE_EQU ::=
// NODE_SIZE = (BLOCK_SIZE - 8 ) / ( key_type_size );
// 2 bytes size + 2 bytes isLeaf
// 8 bytes int, 8 bytes long, 16 bytes char, 64 bytes, 256 bytes
#include <sstream>
const char * itos(int i){
    char * buffer = new char[36];
    ostringstream os;
    os << i;
    strcpy(buffer, os.str().c_str());
    return buffer;
}

template<typename Type>
class NodeSize{
public:
    static const int NODE_SIZE;
};

template<typename Type>
const int NodeSize<Type>::NODE_SIZE = (BLOCK_SIZE - 8 ) / sizeof(Type);


template<typename Type>
struct Node{
    short isLeaf; short size;
    Type v[NodeSize<Type>::NODE_SIZE];
    Node *s[NodeSize<Type>::NODE_SIZE + 1];
};


#include "memory.h"
#include <cstring>
template<typename Type>
class TypeUtil {
public:
    using Conv = const char *;
    static void set(Type &a, Type &b){
        strcpy(a, b);
    }
    static void set(Type &a, const char * b){
        strcpy(a, b);
    }
    static int cmp(Type a, Type b){
        return strcmp((const char *)a, (const char *)b);
    }
    static int cmp(Type a, const char * b){
        return strcmp((const char *)a, b);
    }
};

template<>
class TypeUtil<int>{
public:
    static int cmp(int & a, int & b){
        if( a < b ) return 1;
        if( a > b ) return -1;
        return 0;
    }
};

template<>
class TypeUtil<float>{
public:
    static int cmp(float & a, float & b){
        if( a < b ) return 1;
        if( a > b ) return -1;
        return 0;
    }
};



#define NODE_SIZE_HALF ((NODE_SIZE + 1) / 2)

template<typename Type>
class BPlusTree{
    static const int NODE_SIZE;
    using Node = Node<Type>;

    Node * root = nullptr;
    std::vector<Node *> stack; // record the searching chain
    std::vector<int> stackPos; // record the searching chain position
    int now = -1;
public:
    Node * createNode(){
        Node * node = new Node();
        for(int i = 0; i < NODE_SIZE + 1; i++){
            node->s[i] = nullptr;
        }
        node->size = 0;
        return node;
    }

    void print(Node * x, int indent = 0){
        for(int i=0;i<indent;i++)printf(" ");
        for(int i = 0; i < x->size; i++){
            printf("%s ", x->v[i]);
        }
        printf("\n");
        if(x->isLeaf)return;
        for(int i = 0;i <= x->size; i++){
            print(x->s[i], indent + 1);
        }
    }
    // [INSERT] insert the overflow index-node into non-leaf node
    void insertInto(const Type v, Node * next){
        if( now == -1){ // root
            root = createNode();
            root->isLeaf = false;
            root->size = 1;
            root->s[0] = stack[0];
            root->s[1] = next;
            TypeUtil<Type>::set(root->v[0], v);
            //root->v[0] = v;
        }
        else{
            Node * x = stack[now];
            int pos = stackPos[now];
            // !IMPORTANT: the insertion pair is RIGHT pair (Vi, Si+1), i = pos
            // printf("%d %d$\n", v, pos);
            if( x->size < NODE_SIZE){
                for(int i = x->size; i > pos; i--){
                    TypeUtil<Type>::set(x->v[i], x->v[i - 1]);
                    //x->v[i] = x->v[i - 1];
                    x->s[i + 1] = x->s[i];
                }
                TypeUtil<Type>::set(x->v[pos],v);
                //x->v[pos] = v;
                x->s[pos + 1] = next;
                x->size++;
            }
            else{
                // When the target non-leaf node is full, split is required
                // unlike split the leaf-node, the left-most son ptr has no
                // index value, so the result size is (HALF, HALF - 1)
                Node * target = createNode();
                Type parentValue;
                target->isLeaf = false;
                x->size = NODE_SIZE_HALF;
                if( pos > NODE_SIZE_HALF){
                    // origin node is well-done
                    // target node requires insert new value & ptr
                    // the node spilt method is like following
                    // v: [* * *] (*) [& *]   ( throw the (*)
                    // s:[0 1 2 3]   [4 & 5]  ( find place to insert
                    TypeUtil<Type>::set(parentValue, x->v[NODE_SIZE_HALF]);
                    target->s[0] = x->s[NODE_SIZE_HALF + 1];
                    for(int i = NODE_SIZE_HALF + 1; i <= NODE_SIZE; i ++){
                        if( i == pos){
                            TypeUtil<Type>::set(target->v[target->size] , v);
                            target->s[++target->size] = next;
                        }
                        if( i != NODE_SIZE){
                            TypeUtil<Type>::set(target->v[target->size] , x->v[i]);
                            target->s[++target->size] = x->s[i+1];
                        }
                    }
                }
                else{
                    // target node requires simple copy
                    target->s[0] = x->s[NODE_SIZE_HALF];
                    for(int i = NODE_SIZE_HALF; i< NODE_SIZE; i++){
                        TypeUtil<Type>::set(target->v[target->size++] , x->v[i]);
                        target->s[target->size] = x->s[i + 1];
                    }
                    if( pos == NODE_SIZE_HALF){
                        // do simple replacement
                        // v: [* * *] (&) [* *]   ( throw the (&)
                        // s:[0 1 2 3]   [& 4 5]  ( do the simple replacement
                        target->s[0] = next;
                        TypeUtil<Type>::set(parentValue, v);
                    }
                    else{
                        // origin node requires insertion
                        // v: [* * &] (*) [* *]   ( throw the (*)
                        // s:[0 1 2 &]   [3 4 5]  ( find place to insert
                        TypeUtil<Type>::set(parentValue, x->v[NODE_SIZE_HALF - 1]);
                        for(int i = NODE_SIZE_HALF - 1; i >= 0; i--){
                            if( i == pos){
                                x->s[i + 1] = next;
                                TypeUtil<Type>::set(x->v[i] , v);
                                break;
                            }
                            x->s[i + 1] = x->s[i];
                            TypeUtil<Type>::set(x->v[i] , x->v[i-1]);
                        }
                    }
                }
                //printf("**%d %d\n",target->v[0], now);
                now--; // recursive insertInto
                insertInto(parentValue, target);
            }
        }
    }

    Node * findNode(const Type v){
        now = -1; stack.clear();stackPos.clear();
        Node * x = root;
        // search the target node for insertion by linear
        // recursive search
        while(!x->isLeaf){
            ++now;
            stack.push_back(x);
            if( TypeUtil<Type>::cmp(x->v[x->size - 1], v) <= 0 ){ // v <=
                //if( v >= x->v[x->size - 1]){
                stackPos.push_back(x->size);
                x = x->s[x->size];
                continue;
            }
            for(int i = 0; i < x->size; i++){
                if( TypeUtil<Type>::cmp(x->v[i], v) > 0){
                    stackPos.push_back(i);
                    x = x->s[i];
                    break;
                }
            }
        }
        
        return x;
    }

    void insert(const Type v){
        Node * x = findNode(v);
        if( x->size < NODE_SIZE){
            // When the node is not full, do simple insertion
            for(int i = x->size; i>=1; i--){
                if(TypeUtil<Type>::cmp(x->v[i - 1], v) < 0){
                    TypeUtil<Type>::set(x->v[i] , v);
                    x->size ++;
                    return;
                }
                else{
                    TypeUtil<Type>::set(x->v[i] , x->v[i - 1]);
                }
            }
            TypeUtil<Type>::set(x->v[0] , v);
            x->size ++;
        }
        else{
            // For split of leaf node, the result size is
            // [HALF, HALF], a recursive
            // insertion to parent node is required
            Node * next = createNode();
            next->isLeaf = true;
            x->size = NODE_SIZE_HALF;
            if(TypeUtil<Type>::cmp(x->v[ NODE_SIZE_HALF - 1], v) < 0){
                // origin node is well-done
                // target node requires linear insertion
                bool flag = false;
                for(int i = NODE_SIZE_HALF; i < NODE_SIZE; i ++){
                    if(!flag && TypeUtil<Type>::cmp(x->v[i], v) > 0){
                        TypeUtil<Type>::set(next->v[next->size++] , v);
                        flag = true;
                    }
                    TypeUtil<Type>::set(next->v[next->size++] , x->v[i]);
                }
                if(!flag)TypeUtil<Type>::set(next->v[next->size++] , v);
            }
            else{
                bool flag = false;
                // target node requires simple copy
                for(int i = NODE_SIZE_HALF - 1; i < NODE_SIZE; i ++){
                    TypeUtil<Type>::set(next->v[next->size++] , x->v[i]);
                }
                // origin node requires insertion
                for(int i = NODE_SIZE_HALF - 1; i >=1 ; i --){
                    if(TypeUtil<Type>::cmp(x->v[i - 1], v) < 0){
                        TypeUtil<Type>::set(x->v[i] , v);
                        flag = true;
                        break;
                    }
                    else{
                        TypeUtil<Type>::set(x->v[i] , x->v[i - 1]);
                    }
                }
                if(!flag)TypeUtil<Type>::set(x->v[0] , v);
            }
            stack.push_back(x);
            insertInto(next->v[0], next);
        }
    }

    void removeFrom(){
        Node * x = stack[now];
        int pos = stackPos[now];
        for(int i = pos == 0 ? 0 : pos - 1; i < x->size; i++){
            TypeUtil<Type>::set(x->v[i] ,  x->v[i + 1]);
            x->s[i + 1] = x->s[i + 2];
        }
        x->size--;
        if( now == 0 ){ // root
            if( !root->isLeaf && root->size == 0){
                root = root->s[0];
            }
        }else{
            Node * parent = stack[now - 1];
            int parentPos = stackPos[now - 1];
            if( x->size < NODE_SIZE_HALF - 1){
                if( parentPos != 0 ){
                    Node * sib = parent->s[parentPos - 1];
                    if( sib->size > NODE_SIZE_HALF){ // borrow
                        for(int i = x->size; i >= 1; i--){
                            TypeUtil<Type>::set(x->v[i] , x->v[i - 1]);
                            x->s[i + 1] = x->s[i];
                        }
                        x->s[1] = x->s[0];
                        TypeUtil<Type>::set(x->v[0] , parent->v[parentPos - 1]);
                        TypeUtil<Type>::set(parent->v[parentPos - 1] , sib->v[sib->size - 1]);
                        x->s[0] = sib->s[sib->size];
                        x->size++;
                        sib->size--;
                    }
                    else{ //merge [TESTED]
                        TypeUtil<Type>::set(sib->v[sib->size] , parent->v[parentPos - 1]);
                        sib->s[sib->size + 1] = x->s[0];
                        for(int i = 0; i < x->size; i++ ){
                            TypeUtil<Type>::set(sib->v[sib->size + i + 1] , x->v[i]);
                            sib->s[sib->size + i + 2] = x->s[i + 1];
                        }
                        sib->size += x->size + 1;
                        now -- ;
                        removeFrom();
                    }
                }
                else{
                    Node * sib = parent->s[parentPos + 1];
                    if( sib->size > NODE_SIZE_HALF){ // borrow
                        x->s[x->size + 1] = sib->s[0];
                        TypeUtil<Type>::set(x->v[x->size], parent->v[parentPos]);
                        TypeUtil<Type>::set(parent->v[parentPos], sib->v[0]);
                        //x->v[x->size] = parent->v[parentPos];
                        //parent->v[parentPos] = sib->v[0];
                        sib->s[0] = sib->s[1];
                        for(int i = 0; i < sib->size - 1; i++){
                            TypeUtil<Type>::set(sib->v[i], sib->v[i + 1]);
                            //sib->v[i] = sib->v[i + 1];
                            sib->s[i + 1] = sib->s[i + 2];
                        }
                        sib->size --;
                        x->size ++;
                    }
                    else{ //merge [TESTED]
                        TypeUtil<Type>::set(x->v[x->size], parent->v[parentPos]);
                        //x->v[x->size] = parent->v[parentPos];
                        x->s[x->size + 1] = sib->s[0];
                        for(int i = 0; i < sib->size; i++ ){
                            TypeUtil<Type>::set(x->v[x->size + i + 1], sib->v[i]);
                            //x->v[x->size + i + 1] = sib->v[i];
                            x->s[x->size + i + 2] = sib->s[i + 1];
                        }
                        x->size += sib->size + 1;
                        now -- ;
                        removeFrom();
                    }
                }
            }
        }
    }

    void remove(const Type v){
        Node * x = findNode(v);
        int pos;
        for(pos = 0; pos < x->size; pos++){
            if(TypeUtil<Type>::cmp(x->v[pos], v) == 0)break;
        }
        if(pos == x->size) throw "error"; // NOT FOUND

        for(int i = pos; i < x->size - 1; i++){
            TypeUtil<Type>::set(x->v[i], x->v[i+1]);
            //x->v[i] = x->v[i+1];
        }
        x->size --;

        if( x == root ){
            return;
        }


        Node * parent = stack[now];
        int parentPos = stackPos[now];

        if( x->size >= NODE_SIZE_HALF){
            if( parentPos != 0 ){
                TypeUtil<Type>::set(parent->v[parentPos - 1], x->v[0]);
                //parent->v[parentPos - 1] = x->v[0];
            }
            return;
        }

        if( parentPos != 0 ){
            Node * sib = parent->s[parentPos - 1];
            if( sib->size > NODE_SIZE_HALF){ // borrow
                // move
                for(int i = x->size; i>=1; i--){
                    TypeUtil<Type>::set(x->v[i], x->v[i-1]);
                    //x->v[i] = x->v[i-1];
                }
                TypeUtil<Type>::set(x->v[0], sib->v[sib->size - 1]);
                TypeUtil<Type>::set(parent->v[parentPos - 1], x->v[0]);
                //x->v[0] = sib->v[sib->size - 1];
                //parent->v[parentPos - 1] = x->v[0];
                sib->size--;
                x->size++;
            }
            else{ // merge
                for(int i = 0; i< x->size; i++){
                    TypeUtil<Type>::set(sib->v[sib->size + i], x->v[i]);
                    //sib->v[sib->size + i] = x->v[i];
                }
                sib->size += x->size;
                removeFrom();
            }
        }
        else{
            Node * sib = parent->s[parentPos + 1];
            if( sib->size > NODE_SIZE_HALF){ // borrow
                // move
                TypeUtil<Type>::set(x->v[x->size++], sib->v[0]);
                TypeUtil<Type>::set(parent->v[parentPos], sib->v[1]);
                //x->v[x->size++] = sib->v[0];
                //parent->v[parentPos] = sib->v[1];
                for(int i = 0; i < sib->size - 1; i++){
                    TypeUtil<Type>::set(sib->v[i], sib->v[i+1]);
                    //sib->v[i] = sib->v[i+1];
                }
                sib->size--;
            }
            else{ // merge
                for(int i = 0; i < sib->size; i++){
                    TypeUtil<Type>::set(x->v[x->size + i], sib->v[i]);
                    //x->v[x->size + i] = sib->v[i];
                }
                x->size += sib->size;
                removeFrom();
            }
        }
    }

    void T_BPLUS_TEST(){
        root = createNode();
        root->isLeaf = true;
        for(int i=1;i<=28;i++){
            insert(itos(i));
            printf("======\n");
            print(root);
        }
        for(int i=1;i<=28;i++){
            remove(itos(i));
            print(root);
        }
        printf("=====TEST1=====\n");
        root = createNode();
        root->isLeaf = true;
        for(int i=28;i>=1;i--){
            insert(itos(i));
            printf("========\n");
            print(root);
        }
        for(int i=28;i>=1;i--){
            remove(itos(i));
        }
        printf("=====TEST2=====\n");
        root = createNode();
        root->isLeaf = true;
        for(int i=1;i<=28;i+=2){
            insert(itos(i));
            printf("======\n");
            print(root);
        }
        for(int i=2;i<=29;i+=2){
            insert(itos(i));
            printf("======\n");
            print(root);
        }
        for(int i=1;i<=28;i+=2){
            remove(itos(i));
            print(root);
        }
        for(int i=2;i<=29;i+=2){
            remove(itos(i));
        }
    }
};

template<typename Type>
const int BPlusTree<Type>::NODE_SIZE = NodeSize<Type>::NODE_SIZE;

BPlusTree<char[16]> bPlusTree;
int main(){
    bPlusTree.T_BPLUS_TEST();
    return 0;
}