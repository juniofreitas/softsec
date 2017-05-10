/* IMPLEMENTA PAR GENERICO
 */
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>

#define PARES "PARES"
#define TAM_MAT_ERRO_PAR 5

/* */
typedef struct reg_par Par;
typedef struct reg_par* PPar;
struct reg_par{
	void* pa;
	void* pb;
};

/*----------------------------------------------------------------------------------------------------------------*/
char* erro_par(int coderro);
PPar aloca_par();
void inicia_par(PPar p, void* pa, void* pb);
PPar novo_par(void *pa, void* pb);
void* getA(Par p);
void* getB(Par p);
void* setA(PPar p, void* a);
void* setB(PPar p, void* b);
short verPar(Par p);
long hash_par(Par p);
void free_par(PPar p, void(*free_a)(void *a),void(*free_b)(void *b));
/*----------------------------------------------------------------------------------------------------------------*/

/** Funcao de tratamento de erro em PARES*/
char* erro_par(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_PAR] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "ERRO DE PAR",
	  "ERRO GENERICO"
	};

	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_PAR)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_PAR-1]);
}

/** */
PPar aloca_par(){
	return malloc(sizeof(Par));
}

/** */
void inicia_par(PPar p, void* pa, void* pb){
	if( !p ){
      printf("\n***%s: %s.\n",PARES,erro_par(1));
      exit(0);
   }
	if( pa==pb || (!pa && !pb)){
      printf("\n***%s: %s. Os pares nao podem ser iguais ou os valores estao incorretos.\n",PARES,erro_par(3));
      exit(0);
   }
	p->pa = (pa)?pa:NULL;
	p->pb = (pb)?pb:NULL;
}

/** */
PPar novo_par(void *pa, void* pb){
   PPar np = aloca_par();
	if( !np ){
	   printf("\n***%s: %s.\n",PARES,erro_par(0));
	   exit(0);
   }
   inicia_par(np,pa,pb);
   return np;
}

/******************************************************************************************************************/
/** */
void* getA(Par p){
	return (p.pa)?p.pa:NULL;
}

/** */
void* getB(Par p){
	return (p.pb)?p.pb:NULL;
}

/** */
void* setA(PPar p, void* a){
	if( !p ){
	   printf("\n***%s: %s.\n",PARES,erro_par(0));
	   exit(0);
   }
	void* old = getA(*p);
	p->pa = (a)?a:NULL;
	return old;
}

/** */
void* setB(PPar p, void* b){
	if( !p ){
	   printf("\n***%s: %s.\n",PARES,erro_par(0));
	   exit(0);
   }
	void* old = getB(*p);
	p->pb = (b)?b:NULL;
	return old;
}

/** */
short verPar(Par p){
	return getA(p) && getB(p);
}

/** */
long hash_par(Par p){
	return log(((int)getA(p) * (int)getB(p)) +1);
}

/** */
void free_par(PPar p, void(*free_a)(void *a),void(*free_b)(void *b)){
	if(free_a){
		free_a(p->pa);
		p->pa = NULL;
	}
	if(free_b){
		free_b(p->pb);
		p->pb = NULL;
	}
	if(p)free(p);
	p = NULL;
}
