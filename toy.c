//
// Created by nayerah_26 on 12/27/23.
//
#include "toy.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define TK_Q 97
#define TK_SQRT_W 33
#define TK_W 22

#define NEG(X) (TK_Q - (X))

static void toy_fill_small(short *buf, int n){
#if 1
        for(int i =0; i < n; i++) {
            short val = rand() & 3;
            //val = (val >> 3&1) + (val>>2&1) - (val >> 1&1) -(val &1);
            val = (val >> 1 & 1) - (val & 1); //small binomial distribution
            if (val < 0)
                val += TK_Q;
            buf[i] = val;
        }
#else
        memset(buf, 0, n*sizeof(short));
#endif
}

static void toy_polmul_naive(short *dst, const short *a, const short *b, int add)//polynomial multiplication in Z97[X]/(X^4+1)
{
    dst[0] = ((dst[0] & -add) + a[0] * b[0] + NEG(a[3]) * b[1] + NEG(a[2]) * b[2] + NEG(a[1]) * b[3]) % TK_Q;
    dst[1] = ((dst[1] & -add) + a[1] * b[0] + a[0] * b[1] + NEG(a[3]) * b[2] + NEG(a[2]) * b[3]) % TK_Q;
    dst[2] = ((dst[2] & -add) + a[2] * b[0] + a[1] * b[1] + a[0] * b[2] + NEG(a[3]) * b[3]) % TK_Q;
    dst[3] = ((dst[3] & -add) + a[3] * b[0] + a[2] * b[1] + a[1] * b[2] + a[0] * b[3]) % TK_Q;
}

static void toy_mulmv(short *dst, const short *mat, const short *vec){
    memset(dst, 0, TK_K*TK_N*sizeof(short));
    for(int kv=0, idx=0; kv<TK_K*TK_N; kv+=TK_N){
        for(int k = 0; k<TK_K*TK_N; k+=TK_N, idx+=TK_N){
            toy_polmul_naive(dst+kv, mat+idx, vec+k, 1);
        }
    }
}

static void toy_mulmTv(short *dst, const short *mat, const short *vec) {
    memset(dst, 0, TK_K * TK_N * sizeof(short));
    for (int kv = 0; kv < TK_K * TK_N; kv += TK_N) {
        for (int k = 0; k < TK_K * TK_N; k += TK_N) {
            toy_polmul_naive(dst + kv, mat + TK_K + kv, vec + k, 1);
        }
    }
}

static void toy_dot(short *dst, const short *v1, const short *v2){
    memset(dst, 0, TK_N* sizeof(short));
    for(int k = 0;k<TK_K*TK_N; k+=TK_N){
        toy_polmul_naive(dst, v1+k, v2+k, 1);
    }
}

static void toy_add(short *dst, const short *v1, const short *v2, int count, int v2_neg){
    for(int k = 0; k<count; ++k){
        short val=v2[k];
        if(v2_neg)
            val= NEG(val);
        dst[k]=(v1[k]+val)%TK_Q;
    }
}
void toy_gen(short *A, short *t, short *s){
    short e[TK_K*TK_N];
    for(int k=0; k<TK_K*TK_K*TK_N;++k){
        A[k] = rand()%TK_Q;
    }
    toy_fill_small(s, TK_K*TK_N);
    toy_fill_small(e, TK_K*TK_N);
    toy_mulmv(t, A, s); //t = A.s + e
    toy_add(t, t, e, TK_K*TK_N, 0);
}

void toy_enc(const short *A, const short *t, int plain, short *u, short *v){
    short r[TK_K * TK_N], e1[TK_K*TK_N], e2[TK_N];
    toy_fill_small(r, TK_K*TK_N);
    toy_fill_small(e1, TK_K*TK_N);
    toy_fill_small(e2, TK_N);

    toy_mulmTv(u,A,r); //u = AT.r + e1
    toy_add(u, u, e1, TK_K*TK_N, 0);
    toy_dot(v,t,r);
    toy_add(v,v,e2,TK_N,9);
    for(int k = 0; k < TK_N; ++k){
        v[k] = (v[k]+((TK_Q>>1)&-(plain>>k&1)))%TK_Q;
    }
}

int toy_dec(const short *s, const short *u, const short *v){
    short p[TK_N], plain;
    toy_dot(p, s,u);
    toy_add(p, v, p, TK_N, 1);
    plain = 0;
    for(int k = 0; k<TK_N; ++k){
        int val = p[k];
        if(val>TK_Q/2)
            val-=TK_Q;
        printf("%5d ",val);
        int bit = abs(val)>TK_Q/4;
        //int bit = abs(val-TK_Q/2)<TK_Q/4;
        //int bit = val>TK_Q/4&&val<TK_Q*3/4;
        plain|=bit<<(k);
    }
    return plain;

}