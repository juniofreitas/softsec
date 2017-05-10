/* CLASSE QUE IMPLEMENTA UM HASH TABLE MAP BASEADO NO MESMO HASHMAP IMPLEMENTADO NO JAVA.
   DE ACORDO COM O JAVA DOC ESSE HASH TEM UM ALTO DESEMPENHO VEJA MAIS:

 * This implementation provides constant-time performance for the basic
 * operations (get and put), assuming the hash function
 * disperses the elements properly among the buckets.  Iteration over
 * collection views requires time proportional to the "capacity" of the
 * HashMap instance (the number of buckets) plus its size (the number
 * of key-value mappings).  Thus, it's very important not to set the initial
 * capacity too high (or the load factor too low) if iteration performance is
 * important.
 * An instance of HashMap has two parameters that affect its
 * performance: initial capacity and load factor.  The
 * capacity is the number of buckets in the hash table, and the initial
 * capacity is simply the capacity at the time the hash table is created.  The
 * load factor is a measure of how full the hash table is allowed to
 * get before its capacity is automatically increased.  When the number of
 * entries in the hash table exceeds the product of the load factor and the
 * current capacity, the capacity is roughly doubled by calling the
 * rehash method.
 * As a general rule, the default load factor (.75) offers a good tradeoff
 * between time and space costs.  Higher values decrease the space overhead
 * but increase the lookup cost (reflected in most of the operations of the
 * HashMap class, including get and put).  The
 * expected number of entries in the map and its load factor should be taken
 * into account when setting its initial capacity, so as to minimize the
 * number of rehash operations.  If the initial capacity is greater
 * than the maximum number of entries divided by the load factor, no
 * rehash operations will ever occur.
 * If many mappings are to be stored in a HashMap instance,
 * creating it with a sufficiently large capacity will allow the mappings to
 * be stored more efficiently than letting it perform automatic rehashing as
 * needed to grow the table.   
 */
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>

#define HASHMAP "HASHMAP"
#define TAM_MAT_ERRO_HM 5

const int DEFAULT_INITIAL_CAPACITY = 16;// capacidade inicial do hash
const int MAXIMUM_CAPACITY = 1 << 30; // capacidade máxima do hash
const float DEFAULT_LOAD_FACTOR = 0.75f; // fator padrao

// Estrutura do tipo do item do hashmap
typedef struct reg_dadohash Itemh; // tipo item do hashmap
typedef struct reg_dadohash* PItemh; // ponteiro do tipo item do hashmap
struct reg_dadohash{
	void   *key; // chave do item
	void   *value; // valor do item
	long hash_code; // codigo hash do item
	PItemh next; // ponteiro pro próximo
};

// Estrutura do hashmap em si
typedef struct reg_hashmap   HashMap; // tipo hashmap
typedef struct reg_hashmap*  PHashMap; // ponteiro do tipo hashmap
struct reg_hashmap{
	unsigned long size; // tamanho do hashmap
	int threshold; // limiar do hash
	float loadFactor; // fator de carga
	PItemh* table; // vetor dos items do hashmap
   int tablesize;	// tamanho do vetor de itens
   int  (*equals)(void *, void *); // função genérica de igualdade do hashmap
	long (*hash)(void *);	// função genérica de hash do hashmap
};

/*----------------------------------------------------------------------------------------------------------------*/

int default_equals_hm( void *data1, void *data2 ); // Equacacao de igualdade padrao
long default_hash_hm( void *data ); // Funcao de hash padrao
char* erro_hm(int coderro);
void init_hashmap(PHashMap hm,int(*equals)(void *data1, void *data2),
                  long(*hash)(void *data) ); // Inicializa um ponteiro de hashmap
void init_itemh(PItemh,void *key, void* value, PItemh next, 
                 long hash); // Inicializa um ponteiro de item de hashmap
PHashMap allocate_hashmap(); // Aloca um novo hashmap
PItemh allocate_itemh(); // Aloca um novo item de hashmap
PHashMap create_hashmap(int(*equals)(void *data1, void *data2),
                        long(*hash)(void *data)); // Cria novo hashmap
PItemh create_itemh(void *key,void* value, 
                    PItemh next, long hash); // Cria um novo item de hashmap
/* ---------------------------------------------------*/
void* getHashKey(Itemh ih); // Obtem a chave de um item
void* setHashValue(PItemh itm, void* value); // Seta o valor de um item
unsigned short equals_hm(PItemh ith1, PItemh ith2); // Funcao de comparacao entre itens
long getHashCodeHM(Itemh ih); // Retorna o hash code do item
void resize(PHashMap hm, int newCapacity); // Atualiza o tamanho do hash
PItemh* transfer(PHashMap hm, int newCapacity); // Transfere itens de um hash para outro
void addEntry(PHashMap hm, long hash, void* key, void* value, long bucketIndex); // Adiciona uma nova entrada
void createEntry(PHashMap hm, long hash, void* key, void* value, int bucketIndex); // Cria uma nova entrada
/* ---------------------------------------------------*/
long HASH(HashMap hm, void* x); // Hash do hash
long indexFor(long h,long length); // Retorna a posicao do item no hash
unsigned long hsize(HashMap hm); // Retorna o tamanho do hash
unsigned short hisEmpty(HashMap hm) ; // Verifica se o hash esta vazio
unsigned short eq(HashMap hm, void* x, void* y); // Compara dois valores genericamente
void* hget(HashMap hm,void* key) ; // Obtem um item do hash
unsigned short hcontainsKey(HashMap hm, void* key); // Verifica se um hash contem uma determinada chave
PItemh hgetEntry(HashMap hm, void* key); // Obtem uma entrada
void* hput(PHashMap hm, void* key, void* value); // Insere um item no hash
PArrayList hgetListaItem(HashMap hm); // Retorna a lista de itens do hash
void clear_hm(PHashMap hm,void(*free_key)(void *key),void(*free_value)(void *value));
void free_hm(PHashMap hm,void(*free_key)(void *key),void(*free_value)(void *value));

/*----------------------------------------------------------------------------------------------------------------*/

/** Funcao padrao de igualdade do hash */
int default_equals_hm( void *data1, void *data2 ){
   return((int)data1 == (int)data2);
}

/** Funcao padrao de hash do tipo do item do hash */
long default_hash_hm( void *data ){
	return (long)data;
}

/** Funcao de tratamento de erro em hashmap */
char* erro_hm(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_HM] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "TAMANHO MAXIMO ATINGIDO",
	  "ERRO GENERICO"
	};
	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_HM)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_HM-1]);
}

/** Inicializa o hash map */
void init_hashmap(PHashMap hm, int(*equals)(void *data1, void *data2),long(*hash)(void *data) ){
	if( !hm ){
      printf("\n***%s: %s.\n",HASHMAP,erro_hm(1));
		exit(0);
   }

	unsigned int i;
   	
	hm->size       = 0;	
	hm->threshold  = (int)(DEFAULT_INITIAL_CAPACITY * DEFAULT_LOAD_FACTOR);
	hm->loadFactor = DEFAULT_LOAD_FACTOR;	
   hm->table      = (PItemh*)malloc(sizeof(Itemh)*DEFAULT_INITIAL_CAPACITY);	
   hm->tablesize  = DEFAULT_INITIAL_CAPACITY;
	hm->equals     = equals     ? equals     : default_equals_hm;
	hm->hash       = hash       ? hash       : default_hash_hm;

	for(i=0; i<DEFAULT_INITIAL_CAPACITY; i++){      
      hm->table[i] = (PItemh)malloc(sizeof(Itemh)*1);      
      hm->table[i]->next = NULL;
      hm->table[i]->key  = hm->table[i]->value = NULL;
      hm->table[i]->hash_code = 0;
   }                                        
}

/** Inicializa um item do hash */
void init_itemh(PItemh ihm,void *key, void* value, PItemh next, long hash){
	 if( !ihm ){
       printf("\n***%s: %s.\n",HASHMAP,erro_hm(1));
		 exit(0);
    }

    ihm->key       = key?key:NULL;
    ihm->value     = value?value:NULL;
    ihm->next      = next?next:NULL;
    ihm->hash_code = hash;
}

/** Aloca um hashmap na memoria */
PHashMap allocate_hashmap(){
   return malloc(sizeof(HashMap));
}

/** Aloca um item de hashmap na memoria */
PItemh allocate_itemh(){
   return malloc(sizeof(Itemh));
}

/** Cria um novo hash */
PHashMap create_hashmap(int(*equals)(void *data1, void *data2), long(*hash)(void *data)){
	PHashMap new_hm = allocate_hashmap();
	if( !new_hm ){
	   printf("\n***%s: %s.\n",HASHMAP,erro_hm(0));
	   exit(0);
	}
	init_hashmap( new_hm, equals, hash);
	return new_hm;                                                                                                                           
}

/** Cria um novo item de hash */
PItemh create_itemh(void *key, void* value, PItemh next, long hash){
   PItemh new_ih = allocate_itemh();
   if ( !new_ih ){
	   printf("\n***%s: %s.\n",HASHMAP,erro_hm(0));
	   exit(0);
   }
   init_itemh(new_ih,key,value,next,hash);
   return new_ih;                                         
}                           

/******************************************************************************************************************/
/** Obtem a chave de um item de hash */
void* getHashKey(Itemh ih) {
   return ih.key;
}

/** Obtem o valor de um item de hash */
void* getHashValue(Itemh ih) {
   return ih.value;
}

/** Seta um novo valor de um item de hash */
void* setHashValue(PItemh itm, void* value) {
   void* oldValue = value;
   itm->value = value;
   return oldValue;
}

/** Verifica se dois itens de hash sao iguais */
unsigned short equals_hm(PItemh ith1, PItemh ith2) {
   if (!(ith1) || !(ith2)) return 0;
   return ((ith1->key == ith2->key) && (ith1->value == ith2->value));
}

/** Obtem o hash_code calculado de um item de hash */
long getHashCodeHM(Itemh ih) {
   return ih.hash_code;
}

/** Reajusta a capacidade do hash */
void resize(PHashMap hm, int newCapacity) {
  PItemh* oldTable = hm->table;
  long oldCapacity = hm->tablesize;
  if (oldCapacity == MAXIMUM_CAPACITY) {
      hm->threshold = MAX_INT;
	   printf("\n***%s: %s.\n",HASHMAP,erro_hm(3));
	   exit(0);
  }  
  hm->table = (PItemh*)transfer(hm,newCapacity);
  hm->tablesize = newCapacity;

  hm->threshold = (int)(newCapacity * hm->loadFactor);  
}

/** Transfere os valores do hash para um novo hash de tamanho diferente */
PItemh* transfer(PHashMap hm, int newCapacity) {
  PItemh* src = hm->table;
  PItemh* newTable = (PItemh*)malloc(sizeof(Itemh)* newCapacity);
  long j, i;

  for(i=0; i<newCapacity; i++){
      newTable[i] = (PItemh)malloc(sizeof(Itemh)*1);
      newTable[i]->next = NULL;
      newTable[i]->key  = newTable[i]->value = NULL;
      newTable[i]->hash_code = 0;
  }

  for (j = 0; j < hm->tablesize; j++) {
      PItemh e = src[j];
      if (e != NULL) {
          src[j] = NULL;
          do {
              PItemh next = e->next;
              long index = indexFor(e->hash_code, newCapacity);
              e->next = newTable[index];
              newTable[index] = e;
              e = next;
          }while(e != NULL);
      }
  }
  free(src);
  src = NULL;
  return newTable;
}

/** Adiciona uma nova entrada no hash */
void addEntry(PHashMap hm, long hash, void* key, void* value, long bucketIndex) {
  hm->table[bucketIndex] = create_itemh(key,value,hm->table[bucketIndex],hash);
  if ((hm->size)++ >= hm->threshold) 
      resize(hm,2 * (hm->tablesize));      
}

/** Cria uma nova entrada no hash */
void createEntry(PHashMap hm, long hash, void* key, void* value, int bucketIndex) {
  hm->table[bucketIndex] = create_itemh(key, value, hm->table[bucketIndex],hash);
  hm->size++;
}

/** Funcao hash adicional ... */
long HASH(HashMap hm, void* x){
  long h = hm.hash(x);
/*
  h += ~(h << 9);
  h ^=  (h >> 14);
  h +=  (h << 4); 
  h ^=  (h >> 10);
*/
  return h;
}

/** Calcula a posicao no hash */
long indexFor(long h, long length) {
  return h & (length-1);
}

/** Obtem o tamanho atual do hash */
unsigned long hsize(HashMap hm) {
  return hm.size;
}

/** Verifica se o hash esta vazio */
unsigned short hisEmpty(HashMap hm) {
  return hm.size == 0;
}

/** Funcao principal de igualdade do hash */
unsigned short eq(HashMap hm, void* x, void* y) {
  if(!x || !y)return 0;
  else
     return (int)x == (int)y || hm.equals(x,y);
}

/** Obtem o valor de uma entrada do hash dado uma chave */
void* hget(HashMap hm, void* key) {

  long hash = HASH(hm,key);
  long i = indexFor(hash, hm.tablesize);
 
  PItemh e = hm.table[i];
  while (1) {
      if (e == NULL)
          return e;
      if (e->hash_code == hash && eq(hm,key, e->key))
          return e->value;
      e = e->next;
  }
}

/** Adiciona uma nova entrada pela chave e valor */
void* hput(PHashMap hm, void* key, void* value) {
  long hash = HASH(*hm,key);

  long i = indexFor(hash, hm->tablesize);
  PItemh e;

  for (e = hm->table[i]; e != NULL ; e = e->next) {
      if (e->hash_code == hash && eq(*hm,key, e->key)) {
          void* oldValue = e->value;
          e->value = value;
          return oldValue;
      }
  }
  addEntry(hm,hash, key, value, i);
  return key;
}

/** Verifica se uma chave esta contida no hash */
unsigned short hcontainsKey(HashMap hm, void* key) {
  long hash = HASH(hm,key);
  long i = indexFor(hash, hm.tablesize);
  PItemh e = hm.table[i]; 
  while (e != NULL) {
      if (e->hash_code == hash && eq(hm,key, e->key))
          return 1;
      e = e->next;
  }
  return 0;
}

/** Obtem uma entradas dado uma chave */
PItemh hgetEntry(HashMap hm, void* key) {
  long hash = HASH(hm,key);
  long i = indexFor(hash, hm.tablesize);
  PItemh e = hm.table[i]; 
  while (e != NULL && !(e->hash_code == hash && eq(hm,key, e->key)))
      e = e->next;
  return e;
}

/** Retorna uma lista de itens do hash */
PArrayList hgetListaItem(HashMap hm){
   register long i;
   PArrayList listah = create_arraylist(NULL,NULL);

   for(i=0; i<hm.tablesize; i++){
      PItemh e = hm.table[i];
      while (e->key != NULL) {
         add_data(e,listah);
         e = e->next;
      }
   }
   return listah;
}

/** Limpa o hash */
void clear_hm(PHashMap hm,void(*free_key)(void *key),void(*free_value)(void *value)){
   register long i;

   for(i=0; i<hm->tablesize; i++){
      PItemh e = hm->table[i];
      while (e != NULL) {
         PItemh ep = e;
         e = e->next;
         if(free_key && ep->key){
				free_key(ep->key);ep->key=NULL;
			}
			if(free_value && ep->value){
				free_value(ep->value);ep->value=NULL;
			}
         free(ep);
         ep = NULL;
      }
   }
   free(hm->table);
   hm->table = NULL;

	hm->size       = 0;
	hm->threshold  = (int)(DEFAULT_INITIAL_CAPACITY * DEFAULT_LOAD_FACTOR);
	hm->loadFactor = DEFAULT_LOAD_FACTOR;
   hm->table      = (PItemh*)malloc(sizeof(Itemh)*DEFAULT_INITIAL_CAPACITY);
   hm->tablesize  = DEFAULT_INITIAL_CAPACITY;

   for(i=0; i<DEFAULT_INITIAL_CAPACITY; i++){
      hm->table[i] = (PItemh)malloc(sizeof(Itemh)*1);
      hm->table[i]->next = NULL;
      hm->table[i]->key  = hm->table[i]->value = NULL;
      hm->table[i]->hash_code = 0;
   }
}

/** Elimina o hash */
void free_hm(PHashMap hm,void(*free_key)(void *key),void(*free_value)(void *value)){
   register long i;

   for(i=0; i<hm->tablesize; i++){
      PItemh e = hm->table[i];
      while (e != NULL) {
         PItemh ep = e;
         e = e->next;
         if(free_key && ep->key){
				free_key(ep->key);ep->key=NULL;
			}
			if(free_value && ep->value){
				free_value(ep->value);ep->value=NULL;
			}
         free(ep);
         ep = NULL;
      }
   }
   free(hm->table);
   hm->table = NULL;

   free(hm);
   hm = NULL;
}

//---FIM HASHMAP---
