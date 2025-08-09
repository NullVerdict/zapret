/* Shared KAVL header: inline copy to avoid duplication. */
#ifndef KAVL_H
#define KAVL_H
#ifdef __STRICT_ANSI__
#define inline __inline__
#endif
#define KAVL_MAX_DEPTH 64
#define kavl_size(head, p) ((p)? (p)->head.size : 0)
#define kavl_size_child(head, q, i) ((q)->head.p[(i)]? (q)->head.p[(i)]->head.size : 0)
#define KAVL_HEAD(__type) \
    struct { \
        __type *p[2]; \
        signed char balance; \
        unsigned size; \
    }
#define __KAVL_FIND(suf, __scope, __type, __head,  __cmp) \
    __scope __type *kavl_find_##suf(const __type *root, const __type *x, unsigned *cnt_) { \
        const __type *p = root; unsigned cnt = 0; \
        while (p != 0) { int cmp = __cmp(x, p); if (cmp >= 0) cnt += kavl_size_child(__head, p, 0) + 1; if (cmp < 0) p = p->__head.p[0]; else if (cmp > 0) p = p->__head.p[1]; else break; } \
        if (cnt_) *cnt_ = cnt; return (__type*)p; }
#define __KAVL_ROTATE(suf, __type, __head) \
    static inline __type *kavl_rotate1_##suf(__type *p, int dir) { int opp = 1 - dir; __type *q = p->__head.p[opp]; unsigned size_p = p->__head.size; p->__head.size -= q->__head.size - kavl_size_child(__head, q, dir); q->__head.size = size_p; p->__head.p[opp] = q->__head.p[dir]; q->__head.p[dir] = p; return q; } \
    static inline __type *kavl_rotate2_##suf(__type *p, int dir) { int b1, opp = 1 - dir; __type *q = p->__head.p[opp], *r = q->__head.p[dir]; unsigned size_x_dir = kavl_size_child(__head, r, dir); r->__head.size = p->__head.size; p->__head.size -= q->__head.size - size_x_dir; q->__head.size -= size_x_dir + 1; p->__head.p[opp] = r->__head.p[dir]; r->__head.p[dir] = p; q->__head.p[dir] = r->__head.p[opp]; r->__head.p[opp] = q; b1 = dir == 0? +1 : -1; if (r->__head.balance == b1) q->__head.balance = 0, p->__head.balance = -b1; else if (r->__head.balance == 0) q->__head.balance = p->__head.balance = 0; else q->__head.balance = b1, p->__head.balance = 0; r->__head.balance = 0; return r; }
#define __KAVL_INSERT(suf, __scope, __type, __head, __cmp) \
    __scope __type *kavl_insert_##suf(__type **root_, __type *x, unsigned *cnt_) { unsigned char stack[KAVL_MAX_DEPTH]; __type *path[KAVL_MAX_DEPTH]; __type *bp, *bq; __type *p, *q, *r = 0; int i, which = 0, top, b1, path_len; unsigned cnt = 0; bp = *root_, bq = 0; for (p = bp, q = bq, top = path_len = 0; p; q = p, p = p->__head.p[which]) { int cmp = __cmp(x, p); if (cmp >= 0) cnt += kavl_size_child(__head, p, 0) + 1; if (cmp == 0) { if (cnt_) *cnt_ = cnt; return p; } if (p->__head.balance != 0) bq = q, bp = p, top = 0; stack[top++] = which = (cmp > 0); path[path_len++] = p; } if (cnt_) *cnt_ = cnt; x->__head.balance = 0, x->__head.size = 1, x->__head.p[0] = x->__head.p[1] = 0; if (q == 0) *root_ = x; else q->__head.p[which] = x; if (bp == 0) return x; for (i = 0; i < path_len; ++i) ++path[i]->__head.size; for (p = bp, top = 0; p != x; p = p->__head.p[stack[top]], ++top) if (stack[top] == 0) --p->__head.balance; else ++p->__head.balance; if (bp->__head.balance > -2 && bp->__head.balance < 2) return x; which = (bp->__head.balance < 0); b1 = which == 0? +1 : -1; q = bp->__head.p[1 - which]; if (q->__head.balance == b1) { r = kavl_rotate1_##suf(bp, which); q->__head.balance = bp->__head.balance = 0; } else r = kavl_rotate2_##suf(bp, which); if (bq == 0) *root_ = r; else bq->__head.p[bp != bq->__head.p[0]] = r; return x; }
#define __KAVL_ERASE(suf, __scope, __type, __head, __cmp) \
    __scope __type *kavl_erase_##suf(__type **root_, const __type *x, unsigned *cnt_) { __type *p, *path[KAVL_MAX_DEPTH], fake; unsigned char dir[KAVL_MAX_DEPTH]; int i, d = 0, cmp; unsigned cnt = 0; fake.__head.p[0] = *root_, fake.__head.p[1] = 0; if (cnt_) *cnt_ = 0; if (x) { for (cmp = -1, p = &fake; cmp; cmp = __cmp(x, p)) { int which = (cmp > 0); if (cmp > 0) cnt += kavl_size_child(__head, p, 0) + 1; dir[d] = which; path[d++] = p; p = p->__head.p[which]; if (p == 0) { if (cnt_) *cnt_ = 0; return 0; } } cnt += kavl_size_child(__head, p, 0) + 1; } else { for (p = &fake, cnt = 1; p; p = p->__head.p[0]) dir[d] = 0, path[d++] = p; p = path[--d]; } if (cnt_) *cnt_ = cnt; for (i = 1; i < d; ++i) --path[i]->__head.size; if (p->__head.p[1] == 0) { path[d-1]->__head.p[dir[d-1]] = p->__head.p[0]; } else { __type *q = p->__head.p[1]; if (q->__head.p[0] == 0) { q->__head.p[0] = p->__head.p[0]; q->__head.balance = p->__head.balance; path[d-1]->__head.p[dir[d-1]] = q; path[d] = q, dir[d++] = 1; q->__head.size = p->__head.size - 1; } else { __type *r; int e = d++; for (;;) { dir[d] = 0; path[d++] = q; r = q->__head.p[0]; if (r->__head.p[0] == 0) break; q = r; } r->__head.p[0] = p->__head.p[0]; q->__head.p[0] = r->__head.p[1]; r->__head.p[1] = p->__head.p[1]; r->__head.balance = p->__head.balance; path[e-1]->__head.p[dir[e-1]] = r; path[e] = r, dir[e] = 1; for (i = e + 1; i < d; ++i) --path[i]->__head.size; r->__head.size = p->__head.size - 1; } } while (--d > 0) { __type *q = path[d]; int which, other, b1 = 1, b2 = 2; which = dir[d], other = 1 - which; if (which) b1 = -b1, b2 = -b2; q->__head.balance += b1; if (q->__head.balance == b1) break; else if (q->__head.balance == b2) { __type *r = q->__head.p[other]; if (r->__head.balance == -b1) { path[d-1]->__head.p[dir[d-1]] = kavl_rotate2_##suf(q, which); } else { path[d-1]->__head.p[dir[d-1]] = kavl_rotate1_##suf(q, which); if (r->__head.balance == 0) { r->__head.balance = -b1; q->__head.balance = b1; break; } else r->__head.balance = q->__head.balance = 0; } } } *root_ = fake.__head.p[0]; return p; }
#define kavl_free(__type, __head, __root, __free) do { __type *_p, *_q; for (_p = __root; _p; _p = _q) { if (_p->__head.p[0] == 0) { _q = _p->__head.p[1]; __free(_p); } else { _q = _p->__head.p[0]; _p->__head.p[0] = _q->__head.p[1]; _q->__head.p[1] = _p; } } } while (0)
#define __KAVL_ITR(suf, __scope, __type, __head, __cmp) \
    struct kavl_itr_##suf { const __type *stack[KAVL_MAX_DEPTH], **top, *right; }; \
    __scope void kavl_itr_first_##suf(const __type *root, struct kavl_itr_##suf *itr) { const __type *p; for (itr->top = itr->stack - 1, p = root; p; p = p->__head.p[0]) *++itr->top = p; itr->right = (*itr->top)->__head.p[1]; } \
    __scope int kavl_itr_find_##suf(const __type *root, const __type *x, struct kavl_itr_##suf *itr) { const __type *p = root; itr->top = itr->stack - 1; while (p != 0) { int cmp = __cmp(x, p); if (cmp < 0) *++itr->top = p, p = p->__head.p[0]; else if (cmp > 0) p = p->__head.p[1]; else break; } if (p) { *++itr->top = p; itr->right = p->__head.p[1]; return 1; } else if (itr->top >= itr->stack) { itr->right = (*itr->top)->__head.p[1]; return 0; } else return 0; } \
    __scope int kavl_itr_next_##suf(struct kavl_itr_##suf *itr) { for (;;) { const __type *p; for (p = itr->right, --itr->top; p; p = p->__head.p[0]) *++itr->top = p; if (itr->top < itr->stack) return 0; itr->right = (*itr->top)->__head.p[1]; return 1; } }
#define kavl_insert(suf, proot, x, cnt) kavl_insert_##suf(proot, x, cnt)
#define kavl_find(suf, root, x, cnt) kavl_find_##suf(root, x, cnt)
#define kavl_erase(suf, proot, x, cnt) kavl_erase_##suf(proot, x, cnt)
#define kavl_erase_first(suf, proot) kavl_erase_##suf(proot, 0, 0)
#define kavl_itr_t(suf) struct kavl_itr_##suf
#define kavl_itr_first(suf, root, itr) kavl_itr_first_##suf(root, itr)
#define kavl_itr_find(suf, root, x, itr) kavl_itr_find_##suf(root, x, itr)
#define kavl_itr_next(suf, itr) kavl_itr_next_##suf(itr)
#define kavl_at(itr) ((itr)->top < (itr)->stack? 0 : *(itr)->top)
#define KAVL_INIT2(suf, __scope, __type, __head, __cmp) \
    __KAVL_FIND(suf, __scope, __type, __head,  __cmp) \
    __KAVL_ROTATE(suf, __type, __head) \
    __KAVL_INSERT(suf, __scope, __type, __head, __cmp) \
    __KAVL_ERASE(suf, __scope, __type, __head, __cmp) \
    __KAVL_ITR(suf, __scope, __type, __head, __cmp)
#define KAVL_INIT(suf, __type, __head, __cmp) KAVL_INIT2(suf,, __type, __head, __cmp)
#endif

