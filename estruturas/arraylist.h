/* IMPLEMENTA UMA LISTA GENERICA BASEADO NUM VETOR. SEU ACESSO SE DÁ UM O(1) E SUA INCLUSÃO
   E/OU EXCLUSAO SE DA EM O(N). BASEADA NA CLASSE ARRAYLIST DO JAVA.
 */
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>

#define ARRAYLIST "ARRAYLIST"
#define TAM_MAT_ERRO 5

/* Estrutura que representa um item(objeto) da lista*/
typedef struct reg_item Item;
typedef struct reg_item* PItem;
struct reg_item{
	void   *data; // Dado em si (Generico no caso)
	long hash_code; // Hash do dado (Especifico para cada tipo de objeto)
};

/* Estrutura que representa a lista generica*/
typedef struct reg_arraylist   ArrayList;
typedef struct reg_arraylist*  PArrayList;
struct reg_arraylist{
	unsigned long size; // Tamanho da lista
   PItem elements; // Elementos da lista
   int  (*equals)(void *, void *); // Função generica de comparacao 
	int  (*hash)(void *); // Funcao generica de hash da lista
};

/*----------------------------------------------------------------------------------------------------------------*/

int default_equals_array( void *data1, void *data2 ); // Funcao de comparacao padrao
int default_hash_array( void *data ); // Funcao de hash padrao
char* erro_array(int coderro); // Funcao de tipos de erros em array
void init_arraylist(PArrayList al,int(*equals)(void *data1, void *data2),int(*hash)(void *data) ); // Inicializa a lista
PItem allocate_element(int tam); // Aloca um novo elemento
PArrayList allocate_arraylist(); // Aloca uma nova lista
PArrayList create_arraylist(int(*equals)(void *data1, void *data2),int(*hash)(void *data)); // Cria e inicializa uma nova lista
Item create_element(void *data, long hash_code); // Cria e inicializa um novo elemento
unsigned long size(ArrayList); // Retorna o tamanho da lista
unsigned short isEmpty(ArrayList al); // Verifica se a lista esta vazia
int indexOf( void *, ArrayList al ); // Retorna a posicao de um elemento na lista
unsigned short contains(void *data, ArrayList al); // Verifica se um elemento esta contido na lista
void RangeCheck(unsigned index, ArrayList al); // Func. Aux. que verifica se um indice abrange o tamanho da lista
Item get(unsigned  index, ArrayList al); // Retorna o elemento da lista de determinado indice em O(1)
void *getData(unsigned  index, ArrayList al); // Retorna o valor do elemento da lista de determinado indice
long getHash(unsigned  index, ArrayList al); // Retorna o hash do elemento da lista de determinado indice
Item set(unsigned  index, Item element, PArrayList al); // Seta um elemento na lista em detrerminada posicao
void* setData(unsigned  index, void* data, PArrayList al);
void ensureCapacity(unsigned minCapacity, PArrayList al); // Func. Aux. que aumenta a capacidade de uma lista
unsigned short add(Item o, PArrayList al); // Adiciona um novo elemento a lista, em O(n)                     
unsigned short add_data(void* data, PArrayList al); // Adiciona um novo elemento a lista atraves do valor, em O(n) 
void add_pos(unsigned index, Item o, PArrayList al); // Adiciona um novo elemento em determinada posicao da lista
void* del(unsigned index, PArrayList al); // Remove um elemento da lista de determinado indice
long hashCode(ArrayList al); // Retorna o hash da lista
void clear(PArrayList al, void(*free_data)(void *data)); // Limpa uma lista, ou seja, remove todos os seus elementos
void free_arraylist(PArrayList al, void(*free_data)(void *data)); // Deleta o arraylist

/*----------------------------------------------------------------------------------------------------------------*/

/** Funcao padrao para comparar dados genericamente*/
int default_equals_array( void *data1, void *data2 ){
	return data1 == data2;
}

/** Funcao pasdrao de hash para dados genericos*/
int default_hash_array( void *data ){
	return (int)data;
}

/** Funcao de tratamento de erro em array*/
char* erro_array(int coderro){
	const char* mat_erro[TAM_MAT_ERRO] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "ERRO DE RANGE",
	  "ERRO GENERICO"
	};
	
	return ((coderro>=0 && coderro<=TAM_MAT_ERRO)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO-1]);
}

/** Aloca um novo Item(objeto) na memoria*/
PItem allocate_element(int tam){
   return malloc(sizeof(Item)*tam);
}

/** Aloca um novo array na memoria*/
PArrayList allocate_arraylist(){
   return malloc(sizeof(ArrayList));
}  

/** Inicializa o array com valores padroes e passados por parametros*/
void init_arraylist( PArrayList al,
                     int(*equals)(void *data1, void *data2),
                     int(*hash)(void *data)  ){
	
	if( !al ){
      printf("\n***%s: %s.\n",ARRAYLIST,erro_array(1));
      exit(0);
   }
	al->size       = 0;
	al->equals     = equals     ? equals     : default_equals_array;
	al->hash       = hash       ? hash       : default_hash_array;
	al->elements   = allocate_element(1);
}

/** Cria ja inicializando um novo array*/
PArrayList create_arraylist(int(*equals)(void *data1, void *data2),
                            int(*hash)(void *data)){
	PArrayList new_al = allocate_arraylist();
	if( !new_al ){
	   printf("\n***%s: %s.\n",ARRAYLIST,erro_array(0));
	   exit(0);
   }
	init_arraylist( new_al, equals, hash);
	return new_al;                                           
}

/** Cria um novo tipo elemento, nesse caso eh realizdo apenas uma inicializacao */
Item create_element(void *data, long hash_code){
	Item el;

   el.data  = (data)?data:NULL;
	el.hash_code = hash_code;
	return el;
}

/** Retona o tamanho atual do array*/
unsigned long size(ArrayList al){
   return (unsigned long)al.size;
}

/** Verifica se o array esta vazio*/
unsigned short isEmpty(ArrayList al){
   return al.size == 0;      
}

/** Retorna o posicao de dado elemento (se existir) no array*/
int indexOf( void *data, ArrayList al){
	register unsigned i;
   	
   if(isEmpty(al)) return -1;
	long hash_code = al.hash( data );
	Item actual;
	for( i= 0; i < al.size; ++i ){
	   actual = al.elements[i];
		if( hash_code == al.hash(actual.data) )
			if( al.equals( data, actual.data ) ) return i;
	}
	return -1;    
}

/** Verifica se um elemento esta contido no array*/
unsigned short contains(void *data, ArrayList al){
	return indexOf(data,al) >= 0;
}

/** Verifica se determinado indice esta no range(intervalo do 1° ao ultimo inddice) do array*/
void RangeCheck(unsigned index, ArrayList al){
	if (index < 0){
	    printf("\n***%s: %s. Onde: Index=%d negativo\n",ARRAYLIST,erro_array(0),index);
	    exit(0);
   }
	if (index >= al.size){
	    printf("\n***%s: %s. Onde: Index=%d, mas Size=%d\n",ARRAYLIST,erro_array(0),index,al.size);
	    exit(0);
   }    
}

/** Obtem um Item(elemento) do array */
Item get(unsigned  index, ArrayList al) {
	RangeCheck(index,al);
	return al.elements[index];
}

/** Obtem o valor de um elemento do array */
void *getData(unsigned  index, ArrayList al){
	RangeCheck(index,al);
   Item i = al.elements[index];  
   return i.data;     
}

/** Obtem o valor do hash de um elemento */
long getHash(unsigned  index, ArrayList al){
	RangeCheck(index,al);
   Item i = al.elements[index];  
   return i.hash_code;     
}

/** Seta um Item(elemento) em determinada posicao do array */
Item set(unsigned  index, Item element, PArrayList al){
	RangeCheck(index,*al);

	Item oldValue = al->elements[index];
	al->elements[index] = element;
	return oldValue;
}

/** Seta o valor de um elemento em determinada posicao do array */
void* setData(unsigned  index, void* data, PArrayList al){
   RangeCheck(index,*al);

   long h;
   if(data) h = al->hash(data);
   else h = -1;
   Item oldValue = al->elements[index];
   Item novo = create_element(data,h);
   al->elements[index] = novo;

	return oldValue.data;
}

/** Aloca espcos no array a mais para que a carga de alocacao diminua*/
void ensureCapacity(unsigned minCapacity, PArrayList al) {
	unsigned long oldCapacity = al->size;

	if (minCapacity > oldCapacity) {
	    unsigned newCapacity = (oldCapacity * 3)/2 + 1;
    	 if (newCapacity < minCapacity)
		     newCapacity = minCapacity;
	    al->elements = (PItem) realloc(al->elements,newCapacity*sizeof(Item));
   }
}

/** Adiciona um novo item no fim do array*/
unsigned short add(Item o, PArrayList al) {
	ensureCapacity((al->size) + 1,al);  
	al->elements[(al->size)++] = o;
	return 1;
}

/** Adiciona o valor de um novo item no fim do array*/
unsigned short add_data(void* data, PArrayList al) {
   long h;
   if(data)
      h = al->hash(data);
   else
      h = 0;
   Item novo = create_element(data,h);
	return add(novo,al);
}

/** Adiciona um novo item na posicao passada*/
void add_pos(unsigned index, Item o, PArrayList al) {
	RangeCheck(index,*al);

   ensureCapacity((al->size)+1,al); 
	(al->size)++;
   unsigned i;
   for(i=index;i<(al->size)-1;++i){
	   al->elements[i+1] = al->elements[i];
   }
	al->elements[index] = o;
}

/** Deleta um Item do array */
void* del(unsigned index, PArrayList al){
	RangeCheck(index,*al);

   Item old = (al->elements[index]);   
   unsigned i;

   for(i=index;i<(al->size)-1;++i){
	   al->elements[i] = al->elements[i+1];
   }
   ensureCapacity((al->size)-1,al); 
   (al->size)--;

	return old.data;
}

/** Retorna o hash do array */
long hashCode(ArrayList al) {
	long hashCode = 1;
	unsigned int i=0;
	Item el;
   for (i=0; i<al.size; ++i) {
	    el = al.elements[i];
	    hashCode = 9*hashCode + el.hash_code;
	}
	return hashCode;
}

/******************************************************************************************************************/

/** Limpa um array, ou seja, deleta todos os elementos do array */
void clear(PArrayList al, void(*free_data)(void *data)) {
   int i;
	// Apaga os dados do array de acordo com seu tipo, caso se tenha passado a funcoes adequada
	if(free_data){
	   for(i=0;i<size(*al);i++){
			void* item = getData(i,*al);
			free_data(item);
			item=NULL;
		}
	}
   free(al->elements);
   al->elements = NULL;
   // Reinicia a lista
	init_arraylist(al,al->equals,al->hash);
}

/** Elimina o array liste */
void free_arraylist(PArrayList al, void(*free_data)(void *data)){
   int i;
	// Apaga os dados do array de acordo com seu tipo, caso se tenha passado a funcao adequada
	if(free_data){
	   for(i=0;i<size(*al);i++){
			void* item = getData(i,*al);
			free_data(item);
			item=NULL;
		}
	}
   free(al->elements);
   al->elements = NULL;
   al->size = 0;
   free(al);
   al = NULL;
}
//---FIM ARRAYLIST---
