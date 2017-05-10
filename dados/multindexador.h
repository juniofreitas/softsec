/* INDEXADOR PARA BASES PADRONIZADAS DE REGISTROS COM MULTIPLOS ATRIBUTOS. POR JÚNIO FREITAS
 */
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<time.h>

#define MULTINDEXADOR "MULTINDEXADOR"

#define TAMLONG sizeof(long)
#define TAMINT sizeof(int)
#define TAMSHORT sizeof(short)
#define TAMFLOAT sizeof(double)
#define TAMCHAR sizeof(char)

#define TAMBLOCOM 4096 // Tamanho do bloco, pode ser PARAMETRIZADO
#define QTDITEMBLOCO 5 // Numero de ItemBlocos de cada RegBloco, pode ser PARAMETRIZADO
#define TAMTITULOM TAMLONG*3 // Tamanho dos enderecadores
#define TAMDADOS (TAMBLOCOM - TAMTITULOM) // Tamanho dos dados esperados
#define TAMITEMBLOCO 2*TAMLONG // Tamanho do ItemBloco
#define TAMREGBLOCO (TAMLONG + TAMITEMBLOCO*QTDITEMBLOCO) //Tamanho da RegBloco
#define QTDREGBLOCO (TAMDADOS / TAMREGBLOCO) // Numero de RegBloco por bloco
#define TAMCORPOM (QTDREGBLOCO*TAMREGBLOCO) // Tamanho da lista de RegBloco
#define TAMRESTOM  abs(TAMDADOS-(QTDREGBLOCO*TAMREGBLOCO)) // Resto de bytes

/*******************************************************************************
 * VALHASH: estrutura que representa o valor do hash de VOCABUARIO DE IDS
 ******************************************************************************/
typedef struct tvalhash ValHash;
typedef struct tvalhash* PValHash;
struct tvalhash{
   PToken valor; // Token, ou seja, um vocabulario
   short tamvalor; // Tamanho do token
	short atrib; // Atributo a qual pertence o token
   long tamlista; // Tamanho da lista de documentos que onde o token se encontra
   long endbloco; // Endereço do BlocoM onde os documentos se encontram
   long endultbloco; // Ultimo Endereço do BlocoM de Overflow
};

/*******************************************************************************
 * VOCABULARIO: estrutura que representa o hash em si (VOCABULARIO DE IDS)
 ******************************************************************************/
typedef struct tvocabulario Vocabulario;
typedef struct tvocabulario* PVocabulario;
struct tvocabulario{
   long numbloco; // Numero de blocos usados na indexacao
   long qtdcolecao; // Numero de registros
   long qtdtok; // Quantidade de tokens, ou seja, tamanho do vocabulario
   double tindex; // Tempo de Indexacao
   PHashMap vochash; // Hash em si: ID -> VAL
};

/*******************************************************************************
 * ITEMBLOCO: tipo que representa dados de um token na lista invertida
 ******************************************************************************/
typedef struct titembloco ItemBloco;
typedef struct titembloco* PItemBloco;
struct titembloco{
   long id; // Id do Token
   long qtd; // Quantidade de vez que o token aparece no registro
};

/*******************************************************************************
 * REGBLOCO: tipo que representa os dados de um conjunto de ItemBlocos, ou seja, a lista invertida
 ******************************************************************************/
typedef struct tregbloco RegBloco;
typedef struct tregbloco* PRegBloco;
struct tregbloco{
   long iddoc; // Id do Registro
   ItemBloco listaitem[QTDITEMBLOCO]; // Token do registro, lista invertida!
};

/*******************************************************************************
 * BLOCOM: tipo que representa o bloco de disco usado para indexar
 ******************************************************************************/
typedef struct tblocom BlocoM;
typedef struct tblocom* PBlocoM;
struct tblocom{
   long endbloco; // Endereco do bloco no disco
   long endproxbloco; // Endereco do proximo bloco, caso as listas nao caibam em um so
   long numregbloco; // Numero de listas invertidas contidas no bloco
   RegBloco listareg[QTDREGBLOCO]; // As listas em si
   char resto[TAMRESTOM]; // Informacao para complementar o bloco, nao usada no processo
};

/*******************************************************************************
 * ITEMEND: estrutura auxiliar que relaciona o par ItemBloco -> EndBloco
 ******************************************************************************/
typedef struct titemend ItemEnd;
typedef struct titemend* PItemEnd;
struct titemend{
   ItemBloco item; // Item de uma lista invertida no bloco
   long endbloco; // Endereço do bloco
};

/*******************************************************************************
 * INDATR: estrutura para a indexacao posicional da base de texto
 ******************************************************************************/
typedef struct tindatr IndAtr;
typedef struct tindatr* PIndAtr;
struct tindatr{
   int inicio; // Posicao inicial do atributo
   int tamanho; // Tamanho do atributo
};
 
/*******************************************************************************
 * INDBASEM: estrutura para a indexacao posicional da base de texto
 ******************************************************************************/
typedef struct tindbase IndBaseM;
typedef struct tindbase* PIndBaseM;
struct tindbasem{
   long linha; // Linha do registro
   long tam; // Tamanho do registro
   long pos; // posicao do registro em bytes
   long endbloco; // endereco do bloco do registro
   PIndAtr atrs; // Posisionamento dos atribitos
};

/*----------------------------------------------------------------------------------------------------------------*/
PValHash aloca_valhash();
PVocabulario  aloca_vocabulario();
PItemBloco       aloca_itembloco();
PRegBloco    aloca_regbloco();
PBlocoM    aloca_blocom();
PItemEnd    aloca_itemend();
void inicia_valhash(PValHash infoh, long endbloco, PToken valor);
void inicia_vocabulario(PVocabulario vet);
void inicia_itembloco(PItemBloco no, long id);
void inicia_regbloco(PRegBloco frase, long iddoc);
void inicia_blocom(PBlocoM bloco, long endbloco );
PValHash novo_valhash(long endbloco, PToken valor);
PVocabulario novo_vocabulario();
PItemBloco novo_itembloco(long id);
PRegBloco novo_regbloco(long iddoc);
PItemEnd novo_itemend(long id, long endbloco);
//---------------------------------------------------------------------

/*----------------------------------------------------------------------------------------------------------------*/
/********************************************************************************
 * IMPLEMENTACOES DE FUNCAO
 *******************************************************************************/
/** */
PValHash aloca_valhash(){
   return malloc(sizeof(ValHash));
}

/** */
PVocabulario  aloca_vocabulario(){
   return malloc(sizeof(Vocabulario));
}

/** */
PItemBloco       aloca_itembloco(){
   return malloc(sizeof(ItemBloco));
}

/** */
PRegBloco    aloca_regbloco(){
   return malloc(sizeof(RegBloco));
}

/** */
PBlocoM    aloca_blocom(){
   return malloc(sizeof(BlocoM));
}

/** */
PItemEnd    aloca_itemend(){
   return malloc(sizeof(ItemEnd));
}

/******************************************************************************************************************/
/** */
void inicia_valhash(PValHash infoh, long endbloco, PToken valor){
	if( !infoh ){
      printf("\n***%s: %s.\n",INDEXADOR,"ValHash NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   infoh->endbloco = (endbloco>=0)?endbloco:-1;
   infoh->endultbloco = infoh->endbloco;
   infoh->valor    = valor;
   infoh->tamvalor = getSizeT(*valor);
   infoh->tamlista = 0;
}

/** */
void inicia_vocabulario(PVocabulario vet){
	if( !vet ){
      printf("\n***%s: %s.\n",INDEXADOR,"Vocabulario NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   vet->numbloco = vet->qtdcolecao = vet->qtdtok = vet->tindex = 0;
   vet->vochash     = create_hashmap(def_equals,def_hash);
}

/** */
void inicia_itembloco(PItemBloco no, long id){
	if( !no ){
      printf("\n***%s: %s.\n",INDEXADOR,"NO NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   no->id  = (id>=0)?id:-1;
   no->qtd = 0;
}

/** */
void inicia_regbloco(PRegBloco frase, long iddoc){
	if( !frase ){
      printf("\n***%s: %s.\n",INDEXADOR,"FRASE NAO ALOCADA NA MEMORIA");
      exit(0);
   }
   unsigned int i;
   frase->iddoc = (iddoc>0)?iddoc:0;
}

/** */
void inicia_blocom(PBlocoM bloco, long endbloco ){
	if( !bloco ){
      printf("\n***%s: %s.\n",INDEXADOR,"BLOCO NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   unsigned int i,j;
   bloco->endproxbloco = -1;
   bloco->endbloco = (endbloco>=0)?endbloco:-1;
   bloco->numregbloco = 0;
}

/** */
PValHash novo_valhash(long endbloco, PToken valor){
   PValHash infoh = aloca_valhash();
	if( !infoh ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE ValHash");
	   exit(0);
   }
   inicia_valhash(infoh,endbloco,valor);
   return infoh;
}

/** */
PVocabulario novo_vocabulario(){
   PVocabulario vet = aloca_vocabulario();
	if( !vet ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE Vocabulario");
	   exit(0);
   }
   inicia_vocabulario(vet);
   return vet;
}

/** */
PItemBloco novo_itembloco(long id){
   PItemBloco no = aloca_itembloco();
	if( !no ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE NO");
	   exit(0);
   }
   inicia_itembloco(no,id);
   return no;
}

/** */
PRegBloco novo_regbloco(long iddoc){
   PRegBloco frase = aloca_regbloco();
	if( !frase ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE FRASE");
	   exit(0);
   }
   RegBloco f = {0};
   *frase = f;
   inicia_regbloco(frase,iddoc);
   return frase;
}

/** */
PItemEnd novo_itemend(long id, long endbloco){
   PItemEnd noend = aloca_itemend();
	if( !noend ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE NOEND");
	   exit(0);
   }
   noend->endbloco = endbloco;
   inicia_itembloco(&noend->item,id);
   return noend;
}



