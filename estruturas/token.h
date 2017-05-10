/* ESTA CLASSE IMPLEMENTA O OBJETO TOKEN QUE POR PADRÃO É UMA STRING (PALAVRA) COM
   SUAS PRINCIPAIS OPERAÇÕES. POR OPCAO AQUI TAMBÉM É IMPLEMENTADO OS TIPOS
   FLOAT E LONG 
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TOKEN "TOKEN"
#define LONG "LONG"
#define FLOAT "FLOAT"
#define MAX_INT 0x7fffffff /*2147483647*/
#define MAX_DOUBLE (double)1.7976931348623157e+308
#define LIMIAR_PADRAO (float)0.85
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#define FALSE  0
#define TRUE   1
#define EOS    '\0' /* Fim da string */
#define TAM_MAT_ERRO_TOK 5

/* Estrutura que representa o token */
typedef struct reg_tok Token;
typedef struct reg_tok* PToken;
struct reg_tok{
   char *valor; // token em si
};

/* Estrutura que representa um long */
typedef struct reg_long Long;
typedef struct reg_long* PLong;
struct reg_long{
   long valor;
};

/* Estrutura que representa um float */
typedef struct reg_float Float;
typedef struct reg_float* PFloat;
struct reg_float{
   double valor;
};

/*----------------------------------------------------------------------------------------------------------------*/

PFloat allocate_float(); // Aloca um novo float
PFloat novo_float(double val); // Cria um novo float
double getFloat(Float f); // Obtem o valor do long
double setFloat(PFloat f, double val); // Seta um valor ao long
void floatToString(Float f); // Imprime um float
//------------------------------------------------------------------------------
PLong allocate_long(); // Aloca um novo long
PLong novo_long(long val); // Cria um novo long
long getLong(Long f); // Obtem o valor do long
long setLong(PLong f, long val); // Seta um valor ao long
void longToString(Long l); // Imprime um long
//------------------------------------------------------------------------------
char* erro_token(int coderro); // Funcao de tipos de erros em array
PToken allocate_token(); // Aloca um novo token
void inicia_token(PToken t, char *valor); // Inicializa um novo token
PToken novo_token(char *valor); // Cria e inicializa um novo token
PToken novo_token2(char *valor); // Idem ao anterior mas otimizada
void* setToken(PToken t, char* valor); // Seta um novo valor a um token
unsigned short isNuloT(PToken t); // Verifica se um token é nulo
char* getValorT(Token t); // Obtem o valor do token
unsigned int getSizeT(Token t); // Obtem o tamanho do token
char getChT(Token t,int index); // Obtem o valor do caracter de um token
char* setValorT(PToken t,char* valor); // Seta um novo valor ao token
short compareTo(Token t1, Token t2); // Compara dois token
void toStringT(PToken t); // Transforma um token para string (para printf)
long long hashCodeT(Token t); // Calcula o hash do token de outra forma
unsigned long long hashCodeT01(Token t); // Para teste...
long long hashCodeT2(Token t); // Outra opcao de hash
void concat(PToken dest, PToken orig); // Concatena dois tokens
void concatChar(PToken dest, char* val); // Concatena uma string a um token
void toLower(PToken val); // Transforma pra minusculo o token
void toUpper(PToken t); // Transforma pra maiusculo o token
char* toChar(char val); // Funcao auxiliar q transforma um char numa string
void trim(PToken t); // Retira os espacos iniciais e finais
PToken toCopy(Token t); // Copia um token
void clearT(PToken t); // Limpa o token
void freeT(PToken t);  // Deleta o token
//------------------------------------------------------------------------------
int equals_tokens_hash(void* data1 , void* data2 );
long hash_token_hash(void *key);
int equals_tokens_dist(PToken data1, PToken data2 );
int equals_tokens( char* data1, char* data2 );
int equals_tokens_bag(PToken data1, PToken data2 );
int equals_resp( PFloat data1, PFloat data2 );
unsigned long equals_resp_token( void* data1, void* data2 );
long def_equals_long_tm( PLong data1, PLong data2 );
/*----------------------------------------------------------------------------------------------------------------*/

/** Aloca um novo float*/
PFloat allocate_float(){
  PFloat nf = malloc(sizeof(Float));
  if(nf == NULL){
      printf("\n***%s: %s.\n",FLOAT,erro_token(0));
      exit(0);
  }
  return nf;
}

/** Cria um novo float*/
PFloat novo_float(double val){
  PFloat nf = allocate_float();
  nf->valor = val;
  return nf;
}

/** Obtem o valor de um float*/
double getFloat(Float f){
  return f.valor;    
}

/** Seta um valor de float*/
double setFloat(PFloat f, double val){
  double oldfloat = f->valor;
  f->valor = val;
  return oldfloat;
}

/** Imprime um float */
void floatToString(Float f){
	printf("\"%f\"",f.valor);
}

/******************************************************************************************************************/
/** Aloca na um long */
PLong allocate_long(){
  PLong nl = malloc(sizeof(Long));
  if(nl == NULL){
      printf("\n***%s: %s.\n",LONG,erro_token(0));
      exit(0);
  }
  return nl;
}

/** Cria um long */
PLong novo_long(long val){
  PLong nf = allocate_long();
  nf->valor = val;
  return nf;
}

/** Obtem o valor do long */
long getLong(Long f){
  return f.valor;    
}

/** Seta o valor de um long */
long setLong(PLong f, long val){
  long oldLong = f->valor;
  f->valor = val;
  return oldLong;
}

/** Imprime um long */
void longToString(Long l){
	printf("\"%d\"",l.valor);
}

/******************************************************************************************************************/
/** Funcao de tipos de erros em token geral */
char* erro_token(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_TOK] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "ERRO DE INDICE CARACTER",
	  "ERRO GENERICO"
	};

	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_TOK)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_TOK-1]);
}

/** Aloca um novo token na memoria */
PToken allocate_token(){
   PToken nt = malloc(sizeof(Token));
   if(nt == NULL){
      printf("\n***%s: %s.\n",TOKEN,erro_token(0));
      exit(0);
   }
   return nt;
}

/** Inicializa um token */
void inicia_token(PToken t, char *valor){
     if(valor){
        t->valor = malloc(sizeof(char)*(strlen(valor)+1));
        strcpy(t->valor,valor);
     }
     else{
        t->valor = malloc(sizeof(char)*2);
        strcpy(t->valor,"");          
     }
}

/** Cria um novo token com Copia do valor*/
PToken novo_token(char *valor){
    PToken novo = allocate_token();
    inicia_token(novo,valor);
    return novo;   
}

/** Cria um novo token */
PToken novo_token2(char *valor){
    PToken novo = allocate_token();
    novo->valor = NULL;
    setToken(novo,(valor)?valor:NULL);
    return novo;
}

/** Seta o valor de um novo token */
void* setToken(PToken t, char* valor){
	char* oldval = t->valor;
	t->valor = valor;
	return oldval;
}

/** Verifica se token eh nulo */
unsigned short isNuloT(PToken t){
    return (t == NULL)||(t->valor);
} 

/** Obtem o valor do Token */
char* getValorT(Token t){
      return (t.valor)?t.valor:0;
}

/** Obte um caracter do token*/
char getChT(Token t,int index){
	if (index >= strlen(t.valor)){
       printf("\n***%s: %s. Onde: Index=%d, mas Size=%d\n",TOKEN,erro_token(3),index,strlen(t.valor));
	    exit(0);
   }
   return (t.valor)?t.valor[index]:0;
}

/** Obtem o tamanho do token*/
unsigned int getSizeT(Token t){
      return (t.valor)?strlen(t.valor):0;
}

/** Seta um novo valor ao token*/
char* setValorT(PToken t,char* valor){
     char* oldvalor = t->valor;
     t->valor = valor;
     return oldvalor;
} 

/** Compara dois tokens como caracteres*/
short compareTo(Token t1, Token t2){
	return strcmp(getValorT(t1),getValorT(t2));
} 

/** Escreve uma strings */
void toStringT(PToken t){
   if(isNuloT(t))
      printf(" ");
	else
      printf("\nToken = %s",getValorT(*t));
}

/** Hash da Strings */
long long hashCodeT(Token t){
	int len = getSizeT(t);
   int r ;
	long long h = 0;
	int b = 26;
   register int i;
   char valor[len];

   strcpy(valor,t.valor);
   /* MODELO: s[0]*b^(r) + s[1]*b^(r-1) + ... + s[n-1] */
   for(i = 0, r =0; i < len; i++) {
       long long p = llround( pow((float)b,((float)r)) );
       h = h + (p)*(valor[i]-96);
       if(r<5)r++;
       else
          r = 0;
   }
   if(h<0)printf("\nErro Hash");
   return h ;
}

/** Hash da Strings, segunda opcao */
long long hashCodeT2(Token t){
	int len = getSizeT(t);
	long long h = 0;
   register int i;
   char valor[len];

   strcpy(valor,t.valor);
   /* MODELO: s[0]*b^(r) + s[1]*b^(r-1) + ... + s[n-1] */
   for(i = 0; i < len; i++) {
       h = 31*h + (valor[i]-96);
   }
   return h ;
}

/** Hash: Teste de funcionamento */
unsigned long long hashCodeT01(Token t){
	unsigned int len = getSizeT(t); // Tamanho do token
   unsigned register int i/* indice geral*/, r /* limitador*/;
	unsigned short b = 26; // base
   unsigned short SIMP = 96; // Simplificador do valor asccii, a=1, b=2,c=3,...
   unsigned long long h = 0; // Valor do hash
   int fibval[26] = {26,25,24,23,22,21,20,19,18,17,16,15,
                     14,13,12,11,10,9,8,7,6,5,4,3,2,1};

   for(i=0, r=0; i < len; i++) {
       long long p = llround( pow((double)b,((double)r)) );
       long val = (fibval[t.valor[i]-SIMP-1])+i;
       h = h + p*(val);
       if(r<=3) r++;
       else r=0;
   }
   return h ;
}

/** Concatena tokens */
void concat(PToken dest, PToken orig){
     unsigned int td = getSizeT(*dest);
     unsigned int to = getSizeT(*orig);     
     register unsigned int i;     
     
     unsigned int desti = td+to;
     char *aux = malloc(sizeof(char)*(desti+1));
     if(!aux){
		 printf("\nSEM MEMORIA\n");exit(0);
	  }

     strcpy(aux,dest->valor);
     strcat(aux,orig->valor);      
     aux[strlen(aux)] = '\0';
     char *old = setValorT(dest,aux);
     free(old);
     old = NULL;
}

/** Concatena caracteres ao token */
void concatChar(PToken dest, char* val){
     char* vd = getValorT(*dest);
     unsigned int td = strlen(vd);
     register unsigned int i;

     vd = realloc(vd,td+strlen(val)+1);
     strcat(vd,val);
     char* old = setValorT(dest,vd);
     //free(old);
}

/** Converte um caracter numa string */
char* toChar(char val){   
    char* novo = malloc(sizeof(char)*2);
    if(!novo){
		 printf("\nSEM MEMORIA\n");exit(0);
	 }
    novo[0] = val;
    novo[1] = '\0';
    return novo;
}

/** Converte para minusculo um token */
void toLower(PToken t){
   register int i;
   for(i=0;i<getSizeT(*t);i++){
      t->valor[i] = tolower(t->valor[i]);
   }
}

/** Converte para maisuculo um token */
void toUpper(PToken t){
   register unsigned int i;
   char* lower = malloc(sizeof(char)*(getSizeT(*t)+1));
   for(i=0;i<getSizeT(*t);i++){
		lower[i] = toupper(t->valor[i]);
   }
   lower[i] = '\0';
   char* old = setToken(t,lower);
   if(old)free(old);
}

/** Copia um token */
PToken toCopy(Token t){
   PToken nt = novo_token(getValorT(t));
   return nt;
}

/** Retira os espacos em branco inicial e final*/
void trim(PToken t){
   if(!t)return;
   char ch;
   int i=0,j,ini,fim;
   
   ch = t->valor[i];
   while(isspace(ch)&&i<getSizeT(*t)){
      i++;
      ch = t->valor[i];
   }

   ini = (i<getSizeT(*t))?i:1;

   i = getSizeT(*t)-1;
   do{
      ch = t->valor[i];
      i--;
   }while(isspace(ch) && i>=0);

   fim = (i>=-1)?i+1:0;

   char* nval  = malloc(sizeof(char)*(fim-ini+1));
   for(i=ini,j=0;i<=fim;i++,j++){
      nval[j] = t->valor[i];
   }
   nval[j] = '\0';

   char* old = setValorT(t,nval);
   free(old);
   old = NULL;
}

/** Limpa um token*/
void clearT(PToken t){
   char* valor = t->valor;
   free(valor);
   valor = NULL;
   t->valor = NULL;
}

/** Elimina um token*/
void freeT(PToken t){
   clearT(t);
   free(t);
   t = NULL;
}

//------------------------------------------------------------------------------
// FUNCOES GERAIS DE HASH E IGUALDADE
/** Funcao de igualdade entre token, tal que igualdade significa 1 e nao 0 */
int equals_tokens_hash(void* data1 , void* data2 ){
	if(!data1 || !data2)
	   return 0;
	else{
	   PToken d1 = (PToken)data1;
	   PToken d2 = (PToken)data2;
	   return (strcmp(getValorT(*d1),getValorT(*d2))==0);
	}
}

/** Funcao de hash que usa a funcao de hash principal para token */
long hash_token_hash(void *key){
   if(!key)
      return -1;
   else{
 	 PToken t = (PToken)key;
      return hashCodeT2(*t);
   }
}

/** Funcao de igualdade para token em TreeMap */
int equals_tokens_dist(PToken data1, PToken data2 ){
   return strcmp(getValorT(*data1),getValorT(*data2));
}

/*Funcao de igualdade padrao para long em TreeMap*/
long def_equals_long_tm( PLong data1, PLong data2 ){
   if(getLong(*data1) == getLong(*data2))
      return 0;
   else
   if(getLong(*data1) > getLong(*data2))
     return 2;
   else
   if(getLong(*data1) < getLong(*data2))
	  return -1;
}

/** Compara dois tokens considerados como strings*/
int equals_tokens( char* data1, char* data2 ){
  return strcmp(data1,data2);
}

/** */
int equals_tokens_bag(PToken data1, PToken data2 ){
  return strcmp(getValorT(*data1),getValorT(*data2));
}

/* Funcao auxiliar para comparação de floats*/
int equals_resp( PFloat data1, PFloat data2 ){
   if(!data1 || !data2) return;
   float d1 = getFloat(*data1);
   float d2 = getFloat(*data2);
   if(d1 >= d2)return 2;
   else
   if(d1 < d2)return -1;
}

/* Funcao auxiliar para comparacao de longs*/
unsigned long equals_resp_token( void* data1, void* data2 ){
   if(!data1 || !data2) return;
   unsigned long d1 = getLong(*(PLong)data1);
   unsigned long d2 = getLong(*(PLong)data2);
   return  (d1==d2)?0:(d1>d2)?1:-1;
}

//---FIM TOKEN---
