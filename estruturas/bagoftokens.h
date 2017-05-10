/* ESTA CLASSE IMPLEMENTA O BAG OF TOKEN QUE REPRESENTA UM REGISTRO E O VALOR DE SEUS ITENS...
	O BAG REPRESENTA O REGISTRO TRATATO. JA TENDO AS ESTATISTICAS DA INDEXACAO E É USADO PRINCIPALMENTE
	NA BUSCA VETORIAL.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BAGOFTOKEN "BAGOFTOKEN"
#define TAM_MAT_ERRO_BAG 5

/* Esta classe representa o bag de token*/
typedef struct reg_bag Bag;
typedef struct reg_bag* PBag;
struct reg_bag{
   unsigned int s; // ID do Token
   TreeMap wmap; // Arvore que armazena os palavras do token
   double total; // Peso do bag
   double norma; // Norma do bag
};

/*----------------------------------------------------------------------------------------------------------------*/
int equals_bag(PBag data1, PBag data2);
long hash_bag(PBag key);
char* erro_bag(int coderro);
PBag allocate_bag();
void init_bag(unsigned index, PBag b);
void inicia_bag(unsigned index, PBag b,Tokenizer toks);
PBag novo_bag(unsigned index,Tokenizer toks);
PBag novo_bag2(unsigned index);
PArrayList bagList(Bag b);
unsigned short bcontains(Bag b, PToken t);
void* setBag(PBag b, PToken t);
PFloat getPeso(Bag b, PToken t);
double getPesoN(Bag b, PToken t);
void setPeso(PBag b, PToken t, PFloat d);
void setNorma(PBag b, double norma);
unsigned int size_bag(Bag b);
double getTotalPeso(Bag b);
double getNorma(Bag b);
PToken getBagKey(PItemtm);
PFloat getBagVal(PItemtm);
PToken getBagInicial(Bag b);
void mostra_bag(Bag b);
PToken bagToToken(Bag b);
void clear_bag(PBag b);
void clear_bag2(PBag b);
void freeBag(PBag b);
void freeBag2(PBag b);
/*----------------------------------------------------------------------------------------------------------------*/

/** Funcao de igualdade padrao para Bag */
int equals_bag(PBag data1, PBag data2){
   if(!data1 || !data2) return;
   long d1 = data1->s;
   long d2 = data2->s;
   return (d1==d2);
}

/** Hash padrao para Bag */
long hash_bag(PBag key){
   long d = key->s;
   return (d!=-1)?d:1;
}

/** Erro Bag */
char* erro_bag(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_BAG] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "DADO INEXISTENTE",
	  "ERRO GENERICO"
	};
	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_BAG)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_BAG-1]);
}

/** Aloca um Bag na memoria */
PBag allocate_bag(){
  return malloc(sizeof(Bag));
}

/** Inicia um novo bag vazio */
void init_bag(unsigned index, PBag b){
	if( !b ){
      printf("\n***%s: %s.\n",BAGOFTOKEN,erro_bag(1));
      exit(0);
   }
   b->s = index;
   b->total = 0;
   b->norma = 0;
   init_treemap(&b->wmap,(void*)equals_tokens_bag,NULL);
}

/** Inicia um novo bag a partir de um Tokenizer */
void inicia_bag(unsigned index, PBag b, Tokenizer toks){
	if( !b ){
      printf("\n***%s: %s.\n",BAGOFTOKEN,erro_bag(1));
      exit(0);
   }
   unsigned int i;
   b->s = index;
   b->total = toks.size;
   b->norma = 0;
   init_treemap(&b->wmap,(void*)equals_tokens_bag,NULL);
   for(i=0; i<toks.size; i++){
      PFloat d = getPeso(*b,toks.tokens[i]);
      setFloat(d,getFloat(*d)+1);
      put_tm(&b->wmap,(void*)toks.tokens[i],(void*)d);
   }           
}

/** Cria um novo Bag a partir de um tokenizer */
PBag novo_bag(unsigned index, Tokenizer toks){
   PBag nb = allocate_bag();
	if( !nb ){
      printf("\n***%s: %s.\n",BAGOFTOKEN,erro_bag(0));
      exit(0);
   }
   inicia_bag(index,nb,toks);
   return nb;
}

/** Cria um novo Bag vazio */
PBag novo_bag2(unsigned index){
   PBag bag = allocate_bag();
	if( !bag ){
      printf("\n***%s: %s.\n",BAGOFTOKEN,erro_bag(0));
      exit(0);
   }
   init_bag(index,bag);
   return bag;
}

/** Retorna uma lista contendo os dados do Bag */
PArrayList bagList(Bag b){
   PArrayList ltoken = create_arraylist((void*)equals_tokens_bag,NULL);  
   lista_tm2(getRoot(b.wmap),ltoken);
   return ltoken;
}

/** Verifica se um token esta no Bag */
unsigned short bcontains(Bag b, PToken t){
   return get_tm(b.wmap, (void *)t) != NULL;
} 

/** Seta um token ao Bag */
void* setBag(PBag b, PToken t){
   if(b == NULL)return;
   PFloat d = getPeso(*b,t);
   setFloat(d,getFloat(*d)+1);
   b->total += getFloat(*d)+1;
   return(put_tm(&b->wmap,(void*)t,(void*)d));
}

/** Obtem o peso de um Token no Bag */
PFloat getPeso(Bag b, PToken t){
   PFloat val = (PFloat)get_tm(b.wmap, (void *)t); 
   return (val==NULL)?novo_float(0):val;
}

/** Obtem o peso de um Token no Bag, nesse caso o peso eh normalizado */
double getPesoN(Bag b, PToken t){
   PFloat val = (PFloat)get_tm(b.wmap, (void *)t);
   double norma = sqrt(getNorma(b));
   norma = (norma!=0)?norma:1;
   return (val==NULL)?0:getFloat(*val)/(norma) ;
}

/** Seta um peso ao token de um Bag */
void setPeso(PBag b, PToken t, PFloat d){
	PFloat oldWeight = (PFloat) get_tm(b->wmap,(void*)t);
	b->total += (!oldWeight) ? getFloat(*d) : (getFloat(*d) - getFloat(*oldWeight));
	free(oldWeight);
	put_tm(&b->wmap,(void*)t,(void*)d);
}

/** Seta a norma do Bag */
void setNorma(PBag b, double norma){
   b->norma = (norma<0)?0:norma;
}

/** Obtem o tamanho do Bag */
unsigned int size_bag(Bag b){
   return size_tm(b.wmap);
}

/** Obtem o peso total do Bag */
double getTotalPeso(Bag b) {
   return b.total;
}

/** Obtem a norma do Bag */
double getNorma(Bag b){
   return b.norma;
}

/** Obtem um token do Bag */
PToken getBagKey(PItemtm itm){
   return (PToken)itm->key;
}

/** Obtem um peso do Bag */
PFloat getBagVal(PItemtm itm){
  return (PFloat)itm->value;
}

/** Obtem o primeiro token do Bag */
PToken getBagInicial(Bag b){
   PItemtm inf = firstEntry(b.wmap);
   return inf->key;
}

/** Imprime um Bag */
void mostra_bag(Bag b){
  PItemtm inf = firstEntry(b.wmap);
  printf("[%d:",b.s);
  while(inf != NULL){
     PToken tok = getBagKey(inf);
	  printf(" %s",getValorT(*tok));
     inf = successor(inf);
  }
  printf("]",b.s);
}

/** Transforma um Bag num token */
PToken bagToToken(Bag b){
  PToken tok = novo_token(NULL);
  PItemtm inf = firstEntry(b.wmap);
  while(inf != NULL){
     PToken tb = getBagKey(inf);
     concat(tok,tb);
     concatChar(tok," ");
     inf = successor(inf);
  }
  return tok;
}

/******************************************************************************************************************/
/** Limpa o Bag eliminando os Tokens e os Pesos */
void clear_bag(PBag b){
  PTreeMap tm = &b->wmap;
  clear_tm(tm,(void*)freeT,free);
}

/** Limpa o Bag eliminando somente os Pesos */
void clear_bag2(PBag b){
  PTreeMap tm = &b->wmap;
  clear_tm(tm,NULL,free);
}

/** Elimina um Bag */
void freeBag(PBag b){
   clear_bag(b);
   if(b)free(b);
   b = NULL;
}

/** Elimina um Bag, opicao 2 */
void freeBag2(PBag b){
   clear_bag2(b);
   if(b)free(b);
   b = NULL;
}
//---FIM BAGOFTOKENS---
