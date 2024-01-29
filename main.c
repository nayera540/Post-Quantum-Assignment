#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "toy.h"

void print_bin(int num, int num_bits) {
    for (int i = num_bits - 1; i >= 0; --i) {
        putchar((num & (1 << i)) ? '1' : '0');
    }
}

int main(int argc, char **argv)
{
#if 0
#endif

#if 1
    {
        short A[TK_K*TK_K*TK_N], t[TK_K*TK_N];//public key
        short s[TK_K*TK_N];//private key
        toy_gen(A, t, s);
        for(int msg=0;msg<16;++msg)
        {
            short u[TK_K*TK_N], v[TK_N] ;//ciphertext
            toy_enc(A, t, msg, u, v);
            short m2=toy_dec(s,u, v);
            printf("%2d %2d ", msg, m2);
            print_bin(msg, TK_N) ;
            printf(" ");
            print_bin(m2, TK_N) ;
            printf(" ");
            print_bin(msg^m2, TK_N) ;
            printf("\n");
        }
        printf("Done.\n");
        pause();
        return 0;
    }
#endif
}