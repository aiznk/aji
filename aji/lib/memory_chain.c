#include <aji/lib/memory_chain.h>

typedef struct Node {
    struct Node *next;
    bool is_using;
    size_t size;
    void *ptr;
} Node;

static Node root;

void
AjiMemChain_Destroy(void) {
    for (Node *cur = &root; cur; ) {
        Node *del = cur;
        cur = cur->next;
        free(del->ptr);
        free(del);
    }
}

void
AjiMemChain_Free(void *ptr) {
    if (!ptr) {
        return;
    }

    for (Node *cur = &root; cur; cur = cur->next) {
        if (cur->ptr == ptr) {
            cur->is_using = false;
            break;
        }
    }
}

void *
AjiMemChain_Malloc(size_t size) {
    if (!size) {
        return NULL;
    }

    size_t max = size * 3;
    for (Node *cur = &root; cur; cur = cur->next) {
        if (!cur->is_using && cur->size >= size && cur->size < max) {
            return cur->ptr;
        }
    }

    Node *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->ptr = malloc(size);
    if (!node->ptr) {
        free(node);
        return NULL;
    }

    node->is_using = true;
    node->size = size;

    Node *tail;
    for (Node *cur = &root; cur; cur = cur->next) {
        if (!cur->next) {
            tail = cur;
            break;
        }
    }

    tail->next = node;

    return node->ptr;
}

void *
AjiMemChain_Calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = AjiMemChain_Malloc(total);
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, total);

    return ptr;
}

void *
AjiMemChain_Realloc(void *ptr, size_t size) {
    if (!ptr || !size) {
        return NULL;
    }

    Node *found = NULL;

    for (Node *cur = &root; cur; cur = cur->next) {
        if (cur->is_using && cur->ptr == ptr) {
            found = cur;
            break;
        }
    }

    if (!found) {
        return NULL;
    }

    ptr = realloc(found->ptr, size);
    if (!ptr) {
        return NULL;
    }

    found->ptr = ptr;
    found->size = size;

    return ptr;
}
