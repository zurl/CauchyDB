//
// Created by 张程易 on 2017/3/20.
//
#ifndef DB_BPLUSTREE_H
#define DB_BPLUSTREE_H

#include "../Common.h"
#include "../Services/BlockService.h"


#define NEXT_NODE(x) ((x)->s[NODE_SIZE])


const int NODE_DELETED = 2;

const char * itos(int i);

template<typename Type>
class NodeSize{
public:
    static const int NODE_SIZE;
};


template<typename Type>
const int NodeSize<Type>::NODE_SIZE = (BLOCK_SIZE - 4 - sizeof(int) ) / (sizeof(Type) + sizeof(int));
#define NODE_SIZE_HALF ((NODE_SIZE + 1) / 2)
template<typename Type>
struct BasicNode{
    short isLeaf; short size;
    Type v[NodeSize<Type>::NODE_SIZE];
    int s[NodeSize<Type>::NODE_SIZE + 1];
};




template<typename Type>
class BPlusTree{
    static const int NODE_SIZE;
    using Node = BasicNode<Type>;

    BlockItem * root = 0;
    std::vector<BlockItem *> stack; // record the searching chain
    std::vector<int> stackPos; // record the searching chain position
    int now = -1;

    int fid;
    BlockService * blockService;
public:
    BPlusTree(BlockService * blockService, int fid)
            :fid(fid), blockService(blockService)
    {
        root = blockService->getBlock(fid, 0);
    }



    BlockItem * createNode(){
        int offset = blockService->allocBlock(fid);
        BlockItem * blockItem = blockService->getBlock(fid, offset);
#ifdef CAUCHY_DEBUG
        printf("create node at fid=%d, offset=%d\n", blockItem->fid, (int)blockItem->offset);
#endif
        return blockItem;
    }

    void print(BlockItem * xBlk, int indent = 0){
        assert(indent <= 50);
        Node * x = (Node *)xBlk->value;
        for(int i=0;i<indent;i++)printf(" ");
        for(int i = 0; i < x->size; i++){
            if(x->isLeaf)printf("(%d)", x->s[i]);
            printf("%s ", x->v[i]);

        }
        printf("\n");
        if(x->isLeaf)return;
        for(int i = 0;i <= x->size; i++){
            print(blockService->getBlock(fid, x->s[i]), indent + 1);
        }
        //printf("asd");
    }
    // [INSERT] insert the overflow index-node into non-leaf node
    void insertInto( Type v, BlockItem * next){
        if( now == -1){ // root
            BlockItem * tempBlk = createNode();
            memcpy(tempBlk->value, stack[0]->value, BLOCK_SIZE);
            tempBlk->modified = 1;

            Node * rootPtr = (Node *)root->value;
            rootPtr->isLeaf = false;
            rootPtr->size = 1;
            rootPtr->s[0] = tempBlk->offset;
            rootPtr->s[1] = next->offset;
            TypeUtil<Type>::set(rootPtr->v[0], v);
            root->modified = 1;
            //root->v[0] = v;

        }
        else{
            BlockItem * xBlk = stack[now];
            Node * x = (Node *) xBlk->value;
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
                x->s[pos + 1] = next->offset;
                x->size++;
            }
            else{
                // When the target non-leaf node is full, split is required
                // unlike split the leaf-node, the left-most son ptr has no
                // index value, so the result size is (HALF, HALF - 1)
                BlockItem * targetBlk = createNode();
                Node * target = (Node *)targetBlk->value;
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
                            target->s[++target->size] = next->offset;
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
                        target->s[0] = next->offset;
                        TypeUtil<Type>::set(parentValue, v);
                    }
                    else{
                        // origin node requires insertion
                        // v: [* * &] (*) [* *]   ( throw the (*)
                        // s:[0 1 2 &]   [3 4 5]  ( find place to insert
                        TypeUtil<Type>::set(parentValue, x->v[NODE_SIZE_HALF - 1]);
                        for(int i = NODE_SIZE_HALF - 1; i >= 0; i--){
                            if( i == pos){
                                x->s[i + 1] = next->offset;
                                TypeUtil<Type>::set(x->v[i] , v);
                                break;
                            }
                            x->s[i + 1] = x->s[i];
                            TypeUtil<Type>::set(x->v[i] , x->v[i-1]);
                        }
                    }
                }
                //printf("**%d %d\n",target->v[0], now);
                xBlk->modified = 1;
                now--; // recursive insertInto
                insertInto(parentValue, targetBlk);
            }
        }
    }

    BlockItem * findLeftMostNode(){
        BlockItem * xBlk = root;
        Node * x = (Node *) root->value;
        while(!x->isLeaf) {
            xBlk = blockService->getBlock(fid, x->s[0]);
            x = (Node *) xBlk->value;
        }
        return xBlk;
    }

    BlockItem * findNode( Type v){
        stack.clear();
        stackPos.clear();
        BlockItem * xBlk = root;
        Node * x = (Node *) root->value;
        //printf("$$ %d, %d\n",x->s[0], x->s[1]);
        // search the target node for insertion by linear
        // recursive search
        while(!x->isLeaf){
            stack.push_back(xBlk);
            if( TypeUtil<Type>::cmp(x->v[x->size - 1], v) <= 0 ){ // v <=
                //if( v >= x->v[x->size - 1]){
                stackPos.push_back(x->size);
                xBlk = blockService->getBlock(fid, x->s[x->size]);
                x = (Node *) xBlk->value;
                continue;
            }
            for(int i = 0; i < x->size; i++){
                if( TypeUtil<Type>::cmp(x->v[i], v) > 0){
                    stackPos.push_back(i);
                    xBlk = blockService->getBlock(fid, x->s[i]);
                    x = (Node *) xBlk->value;
                    break;
                }
            }
        }
        return xBlk;
    }

    int findOne( Type key){
        BlockItem * xBlk = findNode(key);
        Node * node = (Node *) xBlk->value;
        for(int i = 0; i < node->size; i++){
            if(TypeUtil<Type>::cmp(node->v[i], key) == 0){
                return node->s[i];
            }
        }
        return 0;
    }

    int findByRange(
            bool withLeft,  Type left, bool leftEqu,
            bool withRight,  Type right, bool rightEqu,
            std::function<void(int, int)> consumer
    ){
        int counter = 0;
        BlockItem * xBlk;
        Node * node;
        int now = 0;
        if( withLeft ) {
            xBlk = findNode(left);
            node = (Node *) xBlk->value;
            bool find = false;
            for (int i = 0; i < node->size; i++) {
                if (TypeUtil<Type>::cmp(node->v[i], left) > 0
                    || (TypeUtil<Type>::cmp(node->v[i], left) == 0 && leftEqu)) {
                    find = true;
                    now = i;
                }
            }
            if(!find){
                if(NEXT_NODE(node) == 0) return 0;
                xBlk = blockService->getBlock(fid, NEXT_NODE(node));
                node = (Node *)xBlk;
            }
        }
        else{
            xBlk = findLeftMostNode();
            node = (Node *) xBlk->value;
        }
        while( true ){
            for(int i = now; i < node->size; i++){
                if(withRight && (TypeUtil<Type>::cmp(node->v[i], right) > 0
                   ||(TypeUtil<Type>::cmp(node->v[i], right) == 0 && !rightEqu))){
                    return counter;
                }
                consumer(counter++, node->s[i]);
            }
            now = 0;
            if(NEXT_NODE(node) == 0) return counter;
            xBlk = blockService->getBlock(fid, NEXT_NODE(node));
            node = (Node *)xBlk->value;
        }
        assert(0);
    }

    bool insert( Type key, int value){
        BlockItem * xBlk = findNode(key);
        Node * x = (Node *)xBlk->value;
        if( x->size < NODE_SIZE){
            // When the node is not full, do simple insertion
            for(int i = x->size; i>=1; i--){
                if(TypeUtil<Type>::cmp(x->v[i - 1], key) < 0){
                    TypeUtil<Type>::set(x->v[i] , key);
                    x->s[i] = value;
                    x->size ++;
                    return true;
                }
                else if(TypeUtil<Type>::cmp(x->v[i - 1], key) == 0){
                    return false;
                }
                else{
                    TypeUtil<Type>::set(x->v[i] , x->v[i - 1]);
                    x->s[i] = x->s[i - 1];
                }
            }
            TypeUtil<Type>::set(x->v[0] , key);
            x->s[0] = value;
            x->size ++;
        }
        else{
            // check the equal situation
            for(int i = x->size; i>=1; i--) {
                if(TypeUtil<Type>::cmp(x->v[i - 1], key) == 0){
                    return false;
                }
            }
            // For split of leaf node, the result size is
            // [HALF, HALF], a recursive
            // insertion to parent node is required
            BlockItem * nextBlk = createNode();
            Node * next = (Node *) nextBlk->value;
            NEXT_NODE(next) = NEXT_NODE(x);
            NEXT_NODE(x) = nextBlk->offset;
            next->isLeaf = true;
            next->size = 0;
            x->size = NODE_SIZE_HALF;
            if(TypeUtil<Type>::cmp(x->v[ NODE_SIZE_HALF - 1], key) < 0){
                // origin node is well-done
                // target node requires linear insertion
                bool flag = false;
                for(int i = NODE_SIZE_HALF; i < NODE_SIZE; i ++){
                    if(!flag && TypeUtil<Type>::cmp(x->v[i], key) >= 0){
                        TypeUtil<Type>::set(next->v[next->size] , key);
                        next->s[next->size++] = value;
                        flag = true;
                    }
                    TypeUtil<Type>::set(next->v[next->size] , x->v[i]);
                    next->s[next->size++] = x->s[i];
                }
                if(!flag) {
                    TypeUtil<Type>::set(next->v[next->size], key);
                    next->s[next->size++] = value;
                }
            }
            else{
                bool flag = false;
                // target node requires simple copy
                for(int i = NODE_SIZE_HALF - 1; i < NODE_SIZE; i ++){
                    TypeUtil<Type>::set(next->v[next->size] , x->v[i]);
                    next->s[next->size++] = x->s[i];
                }
                // origin node requires insertion
                for(int i = NODE_SIZE_HALF - 1; i >=1 ; i --){
                    if(TypeUtil<Type>::cmp(x->v[i - 1], key) < 0){
                        TypeUtil<Type>::set(x->v[i] , key);
                        x->s[i] = value;
                        flag = true;
                        break;
                    }
                    else{
                        TypeUtil<Type>::set(x->v[i] , x->v[i - 1]);
                        x->s[i] = x->s[i - 1];
                    }
                }
                if(!flag){
                    TypeUtil<Type>::set(x->v[0] , key);
                    x->s[0] = value;
                }
            }
            stack.push_back(xBlk);
            insertInto(next->v[0], nextBlk);
            nextBlk->modified = 1;
        }
        xBlk->modified = 1;
        return true;
    }

    void removeFrom(){
        BlockItem * xBlk = stack[now];
        Node * x = (Node *)xBlk->value;
        int pos = stackPos[now];
        for(int i = pos == 0 ? 0 : pos - 1; i < x->size; i++){
            TypeUtil<Type>::set(x->v[i] ,  x->v[i + 1]);
            x->s[i + 1] = x->s[i + 2];
        }
        xBlk->modified = 1;
        x->size--;
        if( now == 0 ){ // root
            Node * rootPtr = (Node *)root->value;
            if( !rootPtr->isLeaf && rootPtr->size == 0){
                //TODO :: deleteblk
                memcpy(root->value, blockService->getBlock(fid, rootPtr->s[0])->value, BLOCK_SIZE);
                root->modified = 1;
            }
        }else{
            BlockItem * parentBlk = stack[now - 1];
            Node * parent = (Node *)parentBlk->value;
            int parentPos = stackPos[now - 1];
            if( x->size < NODE_SIZE_HALF - 1){
                if( parentPos != 0 ){
                    BlockItem * sibBlk = blockService->getBlock(fid, parent->s[parentPos - 1]);
                    Node * sib = (Node *)sibBlk->value;
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
                        x->isLeaf = NODE_DELETED;
                        now -- ;
                        removeFrom();
                    }
                    sibBlk->modified = 1;
                }
                else{
                    BlockItem * sibBlk = blockService->getBlock(fid, parent->s[parentPos + 1]);
                    Node * sib = (Node *)sibBlk->value;
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
                        sib->isLeaf = NODE_DELETED;
                        now -- ;
                        removeFrom();
                    }
                    sibBlk->modified = 1;
                }
            }
            parentBlk->modified = 1;
        }
    }

    bool remove( Type v){
        BlockItem * xBlk = findNode(v);
        Node * x = (Node *) xBlk->value;
        int pos;
        for(pos = 0; pos < x->size; pos++){
            if(TypeUtil<Type>::cmp(x->v[pos], v) == 0)break;
        }
        if(pos == x->size) return false; // NOT FOUND

        for(int i = pos; i < x->size - 1; i++){
            TypeUtil<Type>::set(x->v[i], x->v[i+1]);
            x->s[i] = x->s[i + 1];
            //x->v[i] = x->v[i+1];
        }
        x->size --;
        xBlk->modified = 1;

        if( xBlk->offset == root->offset ){
            return true;
        }


        BlockItem * parentBlk = stack[now];
        Node * parent = (Node *)parentBlk->value;
        int parentPos = stackPos[now];

        if( x->size >= NODE_SIZE_HALF){
            if( parentPos != 0 ){
                TypeUtil<Type>::set(parent->v[parentPos - 1], x->v[0]);
                //parent->v[parentPos - 1] = x->v[0];
                parentBlk->modified = 1;
            }
            return true;
        }

        if( parentPos != 0 ){
            BlockItem * sibBlk = blockService->getBlock(fid, parent->s[parentPos - 1]);
            Node * sib = (Node *)sibBlk->value;
            if( sib->size > NODE_SIZE_HALF){ // borrow
                // move
                for(int i = x->size; i>=1; i--){
                    TypeUtil<Type>::set(x->v[i], x->v[i-1]);
                    x->s[i] = x->s[i - 1];
                    //x->v[i] = x->v[i-1];
                }
                TypeUtil<Type>::set(x->v[0], sib->v[sib->size - 1]);
                x->s[0] = sib->s[sib->size - 1];
                TypeUtil<Type>::set(parent->v[parentPos - 1], x->v[0]);
                //x->v[0] = sib->v[sib->size - 1];
                //parent->v[parentPos - 1] = x->v[0];
                sib->size--;
                x->size++;
            }
            else{ // merge
                for(int i = 0; i< x->size; i++){
                    TypeUtil<Type>::set(sib->v[sib->size + i], x->v[i]);
                    sib->s[sib->size + i] = x->s[i];
                    //sib->v[sib->size + i] = x->v[i];
                }
                sib->size += x->size;
                NEXT_NODE(sib) = NEXT_NODE(x);
                x->isLeaf = NODE_DELETED;
                removeFrom();
            }
            sibBlk->modified = 1;
        }
        else{
            BlockItem * sibBlk = blockService->getBlock(fid, parent->s[parentPos + 1]);
            Node * sib = (Node *)sibBlk->value;
            if( sib->size > NODE_SIZE_HALF){ // borrow
                // move
                TypeUtil<Type>::set(x->v[x->size], sib->v[0]);
                x->s[x->size++] = sib->s[0];
                TypeUtil<Type>::set(parent->v[parentPos], sib->v[1]);
                //x->v[x->size++] = sib->v[0];
                //parent->v[parentPos] = sib->v[1];
                for(int i = 0; i < sib->size - 1; i++){
                    TypeUtil<Type>::set(sib->v[i], sib->v[i+1]);
                    sib->s[i] = sib->s[i + 1];
                    //sib->v[i] = sib->v[i+1];
                }
                sib->size--;
            }
            else{ // merge
                for(int i = 0; i < sib->size; i++){
                    TypeUtil<Type>::set(x->v[x->size + i], sib->v[i]);
                    x->s[x->size + i] = sib->s[i];
                    //x->v[x->size + i] = sib->v[i];
                }
                x->size += sib->size;
                NEXT_NODE(x) = NEXT_NODE(sib);
                sib->isLeaf = NODE_DELETED;
                removeFrom();
            }
            sibBlk->modified = 1;
        }
        parentBlk->modified = 1;
        return true;
    }

    void initialize(){
        Node * rootPtr;
        root = blockService->getBlock(fid, 0);
        rootPtr = (Node *)root->value;
        NEXT_NODE(rootPtr) = 0;
        rootPtr->isLeaf = true;
        rootPtr->size = 0;
        root->modified = 1;
    }

    void DO_LINEAR_TEST(){
        printf("====linear test===\n");
        BlockItem * blk = findNode("1");
        Node * node = (Node *)blk->value;
        while( true ){
            for(int i = 0; i < node->size; i++){
                printf("%s ", node->v[i]);
            }
            if(NEXT_NODE(node) == 0)break;
            blk = blockService->getBlock(fid, NEXT_NODE(node));
            node = (Node *) blk->value;
        }
        printf("\n");
    }
    void T_BPLUS_TEST(){
        Node * rootPtr;
        root = blockService->getBlock(0, 0);
        rootPtr = (Node *)root->value;
        NEXT_NODE(rootPtr) = 0;
        rootPtr->isLeaf = true;
        rootPtr->size = 0;
        root->modified = 1;
        for(int i=1;i<=28;i++){
            insert(itos(i), i);
            printf("======\n");
            print(root);
        }
        printf("%d", (int)findOne("24"));
        printf("\n%d\n", (int)findByRange(true,
                "13", true, true, "2", false,
                [](const int index, const int value){
                    printf("#%d ", (int)value);
                }
        ));
        printf("===fuck1===\n");
        for(int i=1;i<=28;i++){
            remove(itos(i));
            print(root);
        }
        printf("=====TEST1=====\n");
        root = createNode();
        rootPtr = (Node *)root->value;
        NEXT_NODE(rootPtr) = 0;
        rootPtr->isLeaf = true;
        root->modified = 1;
        for(int i=28;i>=1;i--){
            insert(itos(i), i);
            printf("========\n");
            print(root);
        }
        for(int i=28;i>=1;i--){
            remove(itos(i));
        }
        printf("=====TEST2=====\n");
        root = createNode();
        rootPtr = (Node *)root->value;
        rootPtr->isLeaf = true;
        NEXT_NODE(rootPtr) = 0;
        root->modified = 1;
        for(int i=1;i<=28;i+=2){
            insert(itos(i), i);
            printf("======\n");
            print(root);
            DO_LINEAR_TEST();
        }
        for(int i=2;i<=29;i+=2){
            insert(itos(i), i);
            printf("======\n");
            print(root);
            DO_LINEAR_TEST();
        }
        for(int i=1;i<=28;i+=2){
            remove(itos(i));
            print(root);
            DO_LINEAR_TEST();
        }
        for(int i=2;i<=29;i+=2){
            remove(itos(i));
        }
    }
};

template<typename Type>
const int BPlusTree<Type>::NODE_SIZE = NodeSize<Type>::NODE_SIZE;


#endif