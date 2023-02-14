#include <stdio.h>

int main(void){

int a = 100; //variavel normal com valor 100
int *pnt; // declaração variavel ponteiro
pnt = &a; // endereço para qual o ponteiro deve apontar
int q = *pnt; // q assume o valor armazenado no espaço de memoria que o ponteiro aponta
printf("valor de a: %d\n",a);
printf("endereço da variavel a (&a): %x\n",&a);
printf("endereço de memoria que o ponteiro aponta (pnt): %x\n",pnt);
printf("conteudo da variavel q: %d\n",q);
printf("conteudo do ponteiro (*pnt): %d\n",*pnt);

return 0;


}