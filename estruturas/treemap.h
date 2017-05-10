/* ESTA CLASSE IMPLEMENTA UMA ARVORE RED-BLACK BASEADA NUM SORTEDMAP DO JAVA.
   VEJA AS ESPECIFICAÇÕES ABAIXO:
 * This class guarantees that the map will be in ascending key order, sorted
 * according to the natural order for the key's class , 
 * or by the comparator provided at creation time,
 * depending on which constructor is used.
 *
 * This implementation provides guaranteed log(n) time cost for the
 * containsKey, get, put and remove
 * operations.  Algorithms are adaptations of those in Cormen, Leiserson, and
 * Rivest's Introduction to Algorithms.
 * Note that the ordering maintained by a sorted map (whether or not an
 * explicit comparator is provided) must be consistent with equals if
 * this sorted map is to correctly implement the Map interface.
 * This is so because the Map
 * interface is defined in terms of the equals operation, but a map performs
 * all key comparisons using its compareTo (or compare)
 * method, so two keys that are deemed equal by this method are, from the
 * standpoint of the sorted map, equal.  The behavior of a sorted map
 * is well-defined even if its ordering is inconsistent with equals; it
 * just fails to obey the general contract of the Map interface.  
 */

#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>

#define TREEMAP "TREEMAP"
#define TAM_MAT_ERRO_TM 4

unsigned short RED   = 0; // Constante que representa a cor vermelha
unsigned short BLACK = 1; // Constante que representa a cor preta

/* Estrutura auxiliar : Estrutura que representa a informação principal do No */
typedef struct reg_info Infotm;
typedef struct reg_info* PInfotm;
struct reg_info{
   void *key; // Chave do No (unica)
   void *value; // Valor do No
   int  hash_code; // Hash da chave
};

/* Estrutura que representa um item da arvore => NO */
typedef struct reg_itemmap Itemtm;
typedef struct reg_itemmap* PItemtm;
struct reg_itemmap{
	void   *key; // Chave do No (unica)
	void   *value; // Valor do No
	int    hash_code; // Hash da chave
	unsigned short color; // Cor do No
	PItemtm left; // Filho da esquerda
	PItemtm right; // Filho da direita
	PItemtm parent; // Pai do No
};

/* Estrutura que representa a arvore */
typedef struct reg_treemap   TreeMap;
typedef struct reg_treemap*  PTreeMap;
struct reg_treemap{
	unsigned int size; // Tamanho da Arvore
	PItemtm root; // Raiz da Arvore
   int  (*equals)(void *, void *); // Funcao de Igualdade da Arvore
	int  (*hash)(void *); // Hash da Arvore
};

/*----------------------------------------------------------------------------------------------------------------*/

int default_equals_tm( void *data1, void *data2 ); // Equalcao de igualdade padrao da Tree
int default_hash_tm( void *data ); // Hash pasdrao da Tree
char* erro_tm(int coderro);
void init_treemap(PTreeMap tm,int(*equals)(void *data1, void *data2),int(*hash)(void *data) ); // Inicializa a Tree
void init_itemtm(PItemtm,void *key, void* value,void *parent, int hash_code); // Inicializa um No da Tree
PTreeMap allocate_treemap(); // Aloca uma nova Tree
PItemtm allocate_itemtm(); // Aloca um novo no da Treee
PTreeMap create_treemap(int(*equals)(void *data1, void *data2),int(*hash)(void *data)); // Cria uma nova Tree
PItemtm create_itemtm(void *key,void* value, void *parent); // Cria um nov No
/* ---------------------------------------------------*/
void* getKey(Itemtm itm); // Obtem a chava de um No
void* getValue(Itemtm itm); // Obtem o valor de um No
void* setValue(PItemtm itm, void* value); // Seta um novo valor a um No
unsigned short equals_tm(PItemtm itm1, PItemtm itm2); // Verifica se dois Nos sao iguais
int getHashCodeTM(Itemtm itm); //  Hash de toda Tree
/* ---------------------------------------------------*/
PItemtm  getRoot(TreeMap tm); // Obtem a raiz da Tree
PItemtm  firstEntry(TreeMap tm); // Obtem o primeiro No da Tree
PItemtm  lastEntry(TreeMap tm); // Obtem o ultimo No da Tree
PItemtm successor(PItemtm itm); // Retorna o sucessor de um No
PItemtm antecessor(PItemtm itm); // Retorna o antecessor de um No
unsigned short colorOf(PItemtm p); // Cor de um No
PItemtm parentOf(PItemtm p); // Pai de um No
void setColor(PItemtm p, unsigned short c); // Seta cor a um No
PItemtm leftOf(PItemtm p); // Retorna o filho esquerdo de um No
PItemtm rightOf(PItemtm p); // Retorna o filho direito de um No
void rotateLeft(PTreeMap tm, PItemtm p); // Processo de rotacao pra esquerda
void rotateRight(PTreeMap tm, PItemtm p); // Processo de rotacao pra direita
void fixAfterInsertion(PTreeMap tm, PItemtm x); // Reestrutura tree apos insercao
void incrementSize(PTreeMap tm); // Diminui tamanho da Tree
void decrementSize(PTreeMap tm); // Aumenta tamanho da Tree
void fixAfterDeletion(PTreeMap tm, PItemtm x); // Reestrutura tree apos delecao
PInfotm deleteEntry(PTreeMap tm, PItemtm p); // Deleta um No
unsigned int size_tm(TreeMap tm); // Retorna o tamanho da Tree
/* -----------------------------------------------------*/         
PItemtm getEntry(TreeMap tm, void* key); // Obtem um No a partir de uma chave
unsigned short containsKey(TreeMap tm, void* key); // Verifica se a Tree contem uma chave
unsigned short valueSearchNull(PItemtm n); // Verifica se um No eh nulo
unsigned short valueSearchNonNull(TreeMap tm, PItemtm n, void * value); // Procura na Tree pelo valor nao nulo
void* get_tm(TreeMap tm, void *key); // Obtem o valor de um No atraves de uma chave
void* put_tm(PTreeMap tm, void* key, void* value); // Seta um novo valor e chave a um no da Tree
void* key_tm(PItemtm e); // Retorna a chave de um no
void* firstKey(TreeMap tm); // Retorna o primeiro valor da Tree
void* lastKey(TreeMap tm); // Retorna o ultimo valor da Tree
int hashCode_tm(PItemtm itm); // Retorna o hash_code de um No
void lista_tm2(PItemtm itm, PArrayList al); // Lista os elementos da Tree
void lista_data_tm2(PItemtm itm, PArrayList al); // Lista os elementos da Tree, segunda opcao
void* remove_tm(PTreeMap tm, void* key); // Remove um No da Tree atraves de uma chave
void* remove_tm2(PTreeMap tm, void* key); // Remove um No da Tree atraves de uma chave, opcao 2
void clear_tm(PTreeMap tm,void(*free_key)(void *key),void(*free_value)(void *value)); // Limpa uma Tree...
void free_tm(PTreeMap tm,void(*free_key)(void *key),void(*free_value)(void *value)); // Elimina uma Tree...
/*----------------------------------------------------------------------------------------------------------------*/

/** */
int default_equals_tm( void *data1, void *data2 ){
   if(data1 == data2) 
      return 0;
   else
   if((int)data1 > (int)data2)    
     return 2;
   else
   if((int)data1 < (int)data2)         
	  return -1;
}

/** */
int default_hash_tm( void *data ){
	return (int)data;
}

/** Funcao de tratamento de erro em treemap */
char* erro_tm(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_TM] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "ERRO GENERICO"
	};
	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_TM)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_TM-1]);
}

/** */
void init_treemap( PTreeMap tm, int(*equals)(void *data1, void *data2), int(*hash)(void *data)  ){
	if( !tm ){
      printf("\n***%s: %s.\n",TREEMAP,erro_tm(1));
		exit(0);
   }
		
	tm->size       = 0;
	tm->root       = NULL;
	tm->equals     = equals     ? equals     : default_equals_tm;
	tm->hash       = hash       ? hash       : default_hash_tm;
}

/** */
void init_itemtm(PItemtm itemt,void *key, void* value, void *parent, int hash_code){
    if ( !itemt ){
         printf("\n***%s: %s.\n",TREEMAP,erro_tm(1));
			exit(0);
	 }
    
    itemt->left      = itemt->right = NULL;
    itemt->parent    = parent ? parent : NULL;
    itemt->color     = (unsigned short)BLACK;
    itemt->hash_code = hash_code;
    itemt->key       = key ? key : NULL;
    itemt->value     = value ? value : NULL; 
}

/** */
PItemtm allocate_itemtm(){
   return malloc(sizeof(Itemtm));
}

/** */
PTreeMap allocate_treemap(){
   return malloc(sizeof(TreeMap));
}

/** */
PTreeMap create_treemap(int(*equals)(void *data1, void *data2),
                        int(*hash)(void *data)){
	PTreeMap new_tm = allocate_treemap();
	if( !new_tm ){
	   printf("\n***%s: %s.\n",TREEMAP,erro_tm(0));
	   exit(0);
	}
	init_treemap( new_tm, equals, hash);
	return new_tm;                                                                                      
}

/** */
PItemtm create_itemtm(void *key,void* value, void *parent){
   PItemtm new_itm = allocate_itemtm();
   if( !new_itm ){
	   printf("\n***%s: %s.\n",TREEMAP,erro_tm(0));
	   exit(0);
   }
   unsigned int hash_code = hashCode_tm(new_itm);
   init_itemtm(new_itm,key,value,parent,hash_code);
   return new_itm;
}

//---------------------------------------------------------------------------- */
/** */
void* getKey(Itemtm itm){
   return itm.key;   
}

/** */
void* getValue(Itemtm itm){
   return itm.value;   
}

/** */
void* setValue(PItemtm itm, void* value) {
   void* oldValue = itm->value;
   itm->value = value;
   return oldValue;
}

/** */
unsigned short equals_tm(PItemtm itm1, PItemtm itm2) {
   if (!(itm1) || !(itm2)) return 0;
   
   return ((itm1->key == itm2->key) && (itm1->value == itm2->value));
}

/** */
int getHashCodeTM(Itemtm itm) {
   return itm.hash_code;
}

// --------------------------------------------------------------------------- */
/** */
PItemtm  getRoot(TreeMap tm){
   return tm.root;
}

/** */
PItemtm firstEntry(TreeMap tm){
   PItemtm p = tm.root;
   if (p != NULL)
   while (p->left != NULL)
          p = p->left;
   return p;
}

/** */
PItemtm lastEntry(TreeMap tm) {
   PItemtm itm = tm.root;
   if (itm != NULL)
       while (itm->right != NULL)
           itm = itm->right;
   return itm;
}

/** */
PItemtm successor(PItemtm itm) {
   PItemtm t = itm;
        
   if (t == NULL)
      return NULL;
   else if (t->right != NULL) {
      PItemtm p = t->right;
      while (p->left != NULL)
          p = p->left;
      return p;
   } else {
      PItemtm p = t->parent;
      PItemtm ch = t;
      while (p != NULL && ch == p->right) {
          ch = p;
          p = p->parent;
      }
      return p;
  }
}

/** */
PItemtm antecessor(PItemtm itm) {
   PItemtm t = itm;

   if (t == NULL)
      return NULL;
   else if (t->left != NULL) {
      PItemtm p = t->left;
      while (p->right != NULL)
          p = p->right;
      return p;
   } else {
      PItemtm p = t->parent;
      PItemtm ch = t;
      while (p != NULL && ch == p->left) {
          ch = p;
          p = p->parent;
      }
      return p;
  }
}

/** */
unsigned short colorOf(PItemtm p) {
  return (p == NULL ? BLACK : p->color);
}

/** */
PItemtm parentOf(PItemtm p) { 
  return (p == NULL ? NULL: p->parent);
}

/** */
void setColor(PItemtm p, unsigned short c) { 
  if (p != NULL)  p->color = c; 
}

/** */
PItemtm leftOf(PItemtm p) { 
  return (p == NULL)? NULL: p->left; 
}

/** */
PItemtm rightOf(PItemtm p) { 
  return (p == NULL)? NULL: p->right; 
}

/** From CLR **/
void rotateLeft(PTreeMap tm, PItemtm p) {
  PItemtm  r = p->right;
  p->right = r->left;
  if (r->left != NULL)
      r->left->parent = p;
  r->parent = p->parent;
  if (p->parent == NULL)
      tm->root = r;
  else if (p->parent->left == p)
      p->parent->left = r;
  else
      p->parent->right = r;
  r->left = p;
  p->parent = r;
}

/** From CLR **/
void rotateRight(PTreeMap tm, PItemtm p) {
  PItemtm l = p->left;
  p->left = l->right;
  if (l->right != NULL) l->right->parent = p;
  l->parent = p->parent;
  if (p->parent == NULL)
      tm->root = l;
  else if (p->parent->right == p)
      p->parent->right = l;
  else p->parent->left = l;
  l->right = p;
  p->parent = l;
}

/** From CLR **/
void fixAfterInsertion(PTreeMap tm, PItemtm x) {
  x->color = RED;

  while (x != NULL && x != tm->root && x->parent->color == RED) {
      if (parentOf(x) == leftOf(parentOf(parentOf(x)))) {
          PItemtm y = rightOf(parentOf(parentOf(x)));
          if (colorOf(y) == RED) {
              setColor(parentOf(x), BLACK);
              setColor(y, BLACK);
              setColor(parentOf(parentOf(x)), RED);
              x = parentOf(parentOf(x));
          } else {
              if (x == rightOf(parentOf(x))) {
                  x = parentOf(x);
                  rotateLeft(tm,x);
              }
              setColor(parentOf(x), BLACK);
              setColor(parentOf(parentOf(x)), RED);
              if (parentOf(parentOf(x)) != NULL) 
                  rotateRight(tm,parentOf(parentOf(x)));
          }
      } else {
          PItemtm y = leftOf(parentOf(parentOf(x)));
          if (colorOf(y) == RED) {
              setColor(parentOf(x), BLACK);
              setColor(y, BLACK);
              setColor(parentOf(parentOf(x)), RED);
              x = parentOf(parentOf(x));
          } else {
              if (x == leftOf(parentOf(x))) {
                  x = parentOf(x);
                  rotateRight(tm,x);
              }
              setColor(parentOf(x),  BLACK);
              setColor(parentOf(parentOf(x)), RED);
              if (parentOf(parentOf(x)) != NULL) 
                  rotateLeft(tm,parentOf(parentOf(x)));
          }
      }
  }
  tm->root->color = BLACK;
}

/** */
void incrementSize(PTreeMap tm){ 
    (tm->size)++;
}

/** */
void decrementSize(PTreeMap tm){
    (tm->size)--; 
}

/** From CLR **/
void fixAfterDeletion(PTreeMap tm, PItemtm x) {
  while (x != tm->root && colorOf(x) == BLACK) {
      if (x == leftOf(parentOf(x))) {
          PItemtm sib = rightOf(parentOf(x));

          if (colorOf(sib) == RED) {
              setColor(sib, BLACK);
              setColor(parentOf(x), RED);
              rotateLeft(tm,parentOf(x));
              sib = rightOf(parentOf(x));
          }

          if (colorOf(leftOf(sib))  == BLACK && 
              colorOf(rightOf(sib)) == BLACK) {
              setColor(sib,  RED);
              x = parentOf(x);
          } else {
              if (colorOf(rightOf(sib)) == BLACK) {
                  setColor(leftOf(sib), BLACK);
                  setColor(sib, RED);
                  rotateRight(tm,sib);
                  sib = rightOf(parentOf(x));
              }
              setColor(sib, colorOf(parentOf(x)));
              setColor(parentOf(x), BLACK);
              setColor(rightOf(sib), BLACK);
              rotateLeft(tm,parentOf(x));
              x = tm->root;
          }
      } else { 
          PItemtm sib = leftOf(parentOf(x));

          if (colorOf(sib) == RED) {
              setColor(sib, BLACK);
              setColor(parentOf(x), RED);
              rotateRight(tm,parentOf(x));
              sib = leftOf(parentOf(x));
          }

          if (colorOf(rightOf(sib)) == BLACK && 
              colorOf(leftOf(sib)) == BLACK) {
              setColor(sib,  RED);
              x = parentOf(x);
          } else {
              if (colorOf(leftOf(sib)) == BLACK) {
                  setColor(rightOf(sib), BLACK);
                  setColor(sib, RED);
                  rotateLeft(tm,sib);
                  sib = leftOf(parentOf(x));
              }
              setColor(sib, colorOf(parentOf(x)));
              setColor(parentOf(x), BLACK);
              setColor(leftOf(sib), BLACK);
              rotateRight(tm,parentOf(x));
              x = tm->root;
          }
      }
  }
  setColor(x, BLACK); 
}

/*** Delete node p, and then rebalance the tree. */
PInfotm deleteEntry(PTreeMap tm, PItemtm p) {
  decrementSize(tm);
  // Guarda os dados do No a ser deletado
  PInfotm inf = (PInfotm) malloc(sizeof(Infotm));
  inf->hash_code = p->hash_code;
  inf->key       = p->key;
  inf->value     = p->value;

  /* If strictly internal, copy successor's element to p and then make p
   point to successor. */
  if (p->left != NULL && p->right != NULL) {
      PItemtm s = successor(p);
      p->key = s->key;
      p->value = s->value;  
      p = s;
  } /* p has 2 children */

  /* Start fixup at replacement node, if it exists. */
  PItemtm replacement = (p->left != NULL ? p->left : p->right);

  if(replacement != NULL){
      /* Link replacement to parent */
      replacement->parent = p->parent;
      if (p->parent == NULL)
          tm->root = replacement;
      else if (p == p->parent->left)
          p->parent->left  = replacement;
      else
          p->parent->right = replacement;
      /* Null out links so they are OK to use by fixAfterDeletion. */
      p->left = p->right = p->parent = NULL;
      /* Fix replacement */
      if (p->color == BLACK)
          fixAfterDeletion(tm,replacement);
  }
  else if (p->parent == NULL){ /* return if we are the only node. */
      tm->root = NULL;
  }
  else{ /*  No children. Use self as phantom replacement and unlink. */
      if (p->color == BLACK)
          fixAfterDeletion(tm,p);
      if (p->parent != NULL) {
          if (p == p->parent->left)
              p->parent->left = NULL;
          else
			 if (p == p->parent->right)
              p->parent->right = NULL;
          p->parent = NULL;
      }
      //
  }
  free(p);p=NULL; // Delete da memoria
  return inf;
}

/******************************************************************************************************************/

/** */
unsigned int size_tm(TreeMap tm) {
  return tm.size;
}

/** */
PItemtm getEntry(TreeMap tm, void* key) {
  PItemtm p = tm.root;
  while (p != NULL) {
      int cmp = tm.equals(key,p->key);
      if (cmp == 0)
          return p;
      else if (cmp < 0)
          p = p->left;
      else
          p = p->right;
  }
  return NULL;
}

/** */
unsigned short containsKey(TreeMap tm, void* key) {
  return getEntry(tm,key) != NULL;
}

/** */
unsigned short valueSearchNull(PItemtm n) {
  if (n->value == NULL) return 1;
  /* Check left and right subtrees for value */
  return (n->left  != NULL && valueSearchNull(n->left)) ||
         (n->right != NULL && valueSearchNull(n->right));
}

/** */
unsigned short valueSearchNonNull(TreeMap tm, PItemtm n, void * value) {
  /* Check this node for the value */
  if ( !tm.equals(value,n->value) ) return 1;

  /* Check left and right subtrees for value */
  return (n->left  != NULL && valueSearchNonNull(tm,n->left, value)) ||
         (n->right != NULL && valueSearchNonNull(tm,n->right, value));
}

/** */
unsigned short containsValue(TreeMap tm, void* value) {
  return (tm.root==NULL ? 0 :
         (value==NULL   ? valueSearchNull(tm.root)
                       : valueSearchNonNull(tm, tm.root, value)));
}

/** */
void* get_tm(TreeMap tm, void *key) {
  PItemtm p = getEntry(tm,key);    
  return (p==NULL ? NULL : p->value);
}

/** */
void* put_tm(PTreeMap tm, void* key, void* value) {
  PItemtm t = tm->root;

  if (t == NULL) {
      incrementSize(tm);
      tm->root = create_itemtm(key, value, NULL);
      return NULL;
  }

  while (1) {
      int cmp = tm->equals(key, t->key);
      if (cmp == 0) {
          return setValue(t,value);
      } else if (cmp < 0) {
          if (t->left != NULL) {
              t = t->left;
          } else {
              incrementSize(tm);
              t->left = create_itemtm(key, value, t);
              fixAfterInsertion(tm,t->left);
              return NULL;
          }
      } else { /* cmp > 0 */
          if (t->right != NULL) {
              t = t->right;
          } else {
              incrementSize(tm);
              t->right = create_itemtm(key, value, t);
              fixAfterInsertion(tm,t->right);
              return NULL;
          }
      }
  }
}

/** */
void* key_tm(PItemtm e) {
  if (e==NULL) return NULL;   
  return e->key;
}

/** */
void* firstKey(TreeMap tm) {
  return key_tm(firstEntry(tm));
}

/** */
void* lastKey(TreeMap tm) {
  return key_tm(lastEntry(tm));
}

/** */
int hashCode_tm(PItemtm itm) {
	return (int)itm;
}

/** */
void lista_tm2(PItemtm itm, PArrayList al){
  if(itm == NULL) return;
  lista_tm2(itm->left,al);
  PInfotm inf;
  inf = (PInfotm) malloc(sizeof(Infotm));
  inf->hash_code = itm->hash_code;
  inf->key       = itm->key;
  inf->value     = itm->value;
  Item o = create_element(inf,inf->hash_code);
  add(o,al);
  lista_tm2(itm->right,al);
}

/** */
void lista_data_tm2(PItemtm itm, PArrayList al){
  if(itm == NULL) return;
  lista_data_tm2(itm->left,al);
  Item o = create_element(itm->value,itm->hash_code);
  add(o,al);
  lista_data_tm2(itm->right,al);
}

/** */
void* remove_tm(PTreeMap tm, void* key) {
  PItemtm p = getEntry(*tm,key);
  if (p == NULL) return;
  void* oldValue = p->value;
  deleteEntry(tm,p);
  return oldValue;
}

/** */
void* remove_tm2(PTreeMap tm, void* key){
  PItemtm p = getEntry(*tm,key);
  if (p == NULL) return;
  return deleteEntry(tm,p);
}

/** */
void clear_tm(PTreeMap tm,void(*free_key)(void *key),void(*free_value)(void *value)){
  PItemtm inf = getRoot(*tm);
  while(inf != NULL){
     PItemtm atual = inf;
     PInfotm itm = deleteEntry(tm,atual);
     if(free_key){
	     free_key(itm->key);itm->key = NULL;
	  }
	  if(free_value){
	     free_value(itm->value);itm->value = NULL;
	  }
	  free(itm);itm=NULL;
     inf = getRoot(*tm);
  }
  tm->size = 0;
  tm->root = NULL;
}

/** */
void free_tm(PTreeMap tm,void(*free_key)(void *key),void(*free_value)(void *value)){
	clear_tm(tm,free_key,free_value);
	free(tm);
	tm = NULL;
}
//---FIM TREEMAP---
