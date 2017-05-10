/* INDEXADOR PARA BASES SIMPLES DE TEXTO. POR JÚNIO FREITAS
 */
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<time.h>

#define INDEXADOR "INDEXADOR"

#define TAMLONG sizeof(long)
#define TAMINT sizeof(int)
#define TAMSHORT sizeof(short)
#define TAMFLOAT sizeof(double)
#define TAMCHAR sizeof(char)

#define TAMBLOCO 4096 // Tamanho do bloco, pode ser PARAMETRIZADO
#define M 5 // Numero de Nos por frase, pode ser PARAMETRIZADO
#define TAMTITULO TAMLONG*3 // Tamanho dos enderecadores
#define TAMLISTA (TAMBLOCO - TAMTITULO) // Tamanho dos dados esperado
#define TAMNO 2*TAMLONG // Tamanho do no
#define TAMFRASE (TAMLONG + TAMNO*M) //Tamanho da frase
#define N (TAMLISTA / TAMFRASE) // Numero de Frases por bloco
#define TAMCORPO (N*TAMFRASE) // Tamanho da lista de frases
#define TAMRESTO  abs(TAMLISTA-(N*TAMFRASE)) // Resto de bytes

/*******************************************************************************
 * INFOVETHASH: estrutura que representa o valor do hash de VOCABUARIO DE IDS
 ******************************************************************************/
typedef struct tinfohash InfoHash;
typedef struct tinfohash* PInfoHash;
struct tinfohash{
   PToken valor; // Token, ou seja, um vocabulario
   short tamvalor; // Tamanho do token
   long tamlista; // Tamanho da lista de documentos que onde o token se encontra
   long endbloco; // Endereço do Bloco onde os documentos se encontram
   long endultbloco; // Ultimo Endereço do Bloco de Overflow
};

/*******************************************************************************
 * VETHASH: estrutura que representa o hash em si (VOCABULARIO DE IDS)
 ******************************************************************************/
typedef struct tvethash VetHash;
typedef struct tvethash* PVetHash;
struct tvethash{
   long numbloco; // Numero de blocos usados na indexacao
   long qtdcolecao; // Numero de registros
   long qtdtok; // Quantidade de tokens, ou seja, tamanho do vocabulario
   double tindex; // Tempo de Indexacao
   PHashMap info; // Hash em si: ID -> END
};

/*******************************************************************************
 * NO: tipo que representa dados de um token na lista invertida
 ******************************************************************************/
typedef struct tno No;
typedef struct tno* PNo;
struct tno{
   long id; // Id do Token
   long qtd; // Quantidade de vez que o token aparece no registro
};

/*******************************************************************************
 * FRASE: tipo que representa os dados de um conjunto de nos, ou seja, a lista invertida
 ******************************************************************************/
typedef struct tfrase Frase;
typedef struct tfrase* PFrase;
struct tfrase{
   long iddoc; // Id do Registro
   No listano[M]; // Token do registro, lista invertida!
};

/*******************************************************************************
 * BLOCO: tipo que representa o bloco de disco usado para indexar
 ******************************************************************************/
typedef struct tbloco Bloco;
typedef struct tbloco* PBloco;
struct tbloco{
   long endbloco; // Endereco do bloco no disco
   long endproxbloco; // Endereco do proximo bloco, caso as listas nao caibam em um so
   long numfrase; // Numero de listas invertidas contidas no bloco
   Frase listafrase[N]; // As listas em si
   char resto[TAMRESTO]; // Informacao para complementar o bloco, nao usada no processo
};

/*******************************************************************************
 * NOEND: estrutura auxiliar que relaciona o par No -> EndBloco
 ******************************************************************************/
typedef struct tnoend NoEnd;
typedef struct tnoend* PNoEnd;
struct tnoend{
   No no; // Item de uma lista invertida no bloco
   long endbloco; // Endereço do bloco
};

/*******************************************************************************
 * INDBASE: estrutura para a indexacao posicional da base de texto
 ******************************************************************************/
typedef struct tindbase IndBase;
typedef struct tindbase* PIndBase;
struct tindbase{
   long linha; // Linha do registro
   long tam; // Tamanho do registro
   long pos; // posicao do registro em bytes
   long endbloco; // endereco do bloco do registro
};

/*----------------------------------------------------------------------------------------------------------------*/

PInfoHash aloca_infohash();
PVetHash  aloca_vethash();
PNo       aloca_no();
PFrase    aloca_frase();
PBloco    aloca_bloco();
PNoEnd    aloca_noend();
int def_equals(void* data1, void* data2 );
long def_hash(void *data);
void inicia_infohash(PInfoHash infoh, long endbloco, PToken valor);
void inicia_vethash(PVetHash vet);
void inicia_no(PNo no, long id);
void inicia_frase(PFrase frase, long iddoc);
void inicia_bloco(PBloco bloco, long endbloco );
PInfoHash novo_infohash(long endbloco, PToken valor);
PVetHash novo_vethash();
PNo novo_no(long id);
PFrase nova_frase(long iddoc);
PNoEnd novo_noend(long id, long endbloco);
//---------------------------------------------------------------------
long getSize_vet(VetHash vet);
PInfoHash getVetHash(VetHash vet, PLong key);
unsigned short contemVetHash(VetHash vet, PLong key);
void* addVetHash(PVetHash vet, PLong key, PInfoHash infoh);
unsigned short add_frase(PBloco bloco, PFrase frase);
PFrase calc_frase(long iddoc, ArrayList lend, long ini);
int calc_listafrase2(long iddoc, PArrayList lfrase, No n, int qtd);
HashMap calcHashTok(VetHash vet);
//---------------------------------------------------------------------
void gravaVetHash(VetHash vet,char* arqvoc);
void gravaLogIndex(VetHash vet,char* nomebase);
void carregaVetHash(PVetHash vet, char arquivol[255]);
void carregaInfo(PLong numbloco, PLong qtdcolecao, PLong qtdtok, char arquivol[255]);
PVetHash carrega_voc(char* nomearqvoc);
PBloco novo_bloco(PVetHash vet,FILE *arq);
void diskRead(FILE *arq, long endbloco, PBloco bloco);
void diskWrite(FILE *arq, long endbloco, PBloco bloco);
void diskReadBase(FILE *arq, long linha, PIndBase ib);
void diskWriteBase(FILE *arq, long linha, PIndBase ib);
void cria_indexador(PVetHash vet, char* arqlista, FILE **arq);
long indexa_base(char* nomearq, char* nomearqx);
PToken retLinha(FILE *f,FILE *x, long linha, PIndBase ib);
PToken retLinha2(char* nomebase,char* nomeindbase, long linha, PIndBase ib);
//---------------------------------------------------------------------
long add_bloco(PVetHash vet, PBloco bloco, long endultbloco, ArrayList lfrase,FILE *arq);
unsigned short add_doc2(PVetHash vet, TreeMap treetok, ArrayList lfrase, FILE *arq);
PArrayList get_doc_frase(long endbloco, FILE *arq);
PArrayList get_doc_bags(VetHash vet, long endbloco, FILE *arq);
PArrayList get_doc_toks(VetHash vet, long endbloco, FILE *arq);
//---------------------------------------------------------------------
PVetHash indexador_online(char* nomearq, char* nomearqx, char *nomearqvoc, char *nomearqinv, char sep);

/*----------------------------------------------------------------------------------------------------------------*/
/********************************************************************************
 * IMPLEMENTACOES DE FUNCAO
 *******************************************************************************/
/** */
PInfoHash aloca_infohash(){
   return malloc(sizeof(InfoHash));
}

/** */
PVetHash  aloca_vethash(){
   return malloc(sizeof(VetHash));
}

/** */
PNo       aloca_no(){
   return malloc(sizeof(No));
}

/** */
PFrase    aloca_frase(){
   return malloc(sizeof(Frase));
}

/** */
PBloco    aloca_bloco(){
   return malloc(sizeof(Bloco));
}

/** */
PNoEnd    aloca_noend(){
   return malloc(sizeof(NoEnd));
}

/******************************************************************************************************************/
/** */
int def_equals(void* data1, void* data2 ){
   if(!data1 || !data2) return 0;

   long d1 = getLong(*(PLong)data1);
   long d2 = getLong(*(PLong)data2);
   return  (d1==d2);
}

/** */
long def_hash(void *data){
   if(!data)return -1;
   long d = getLong(*(PLong)data);
   return (long)(d);
}

/** */
void inicia_infohash(PInfoHash infoh, long endbloco, PToken valor){
	if( !infoh ){
      printf("\n***%s: %s.\n",INDEXADOR,"INFOHASH NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   infoh->endbloco = (endbloco>=0)?endbloco:-1;
   infoh->endultbloco = infoh->endbloco;
   infoh->valor    = valor;
   infoh->tamvalor = getSizeT(*valor);
   infoh->tamlista = 0;
}

/** */
void inicia_vethash(PVetHash vet){
	if( !vet ){
      printf("\n***%s: %s.\n",INDEXADOR,"VETHASH NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   vet->numbloco = vet->qtdcolecao = vet->qtdtok = vet->tindex = 0;
   vet->info     = create_hashmap(def_equals,def_hash);
}

/** */
void inicia_no(PNo no, long id){
	if( !no ){
      printf("\n***%s: %s.\n",INDEXADOR,"NO NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   no->id  = (id>=0)?id:-1;
   no->qtd = 0;
}

/** */
void inicia_frase(PFrase frase, long iddoc){
	if( !frase ){
      printf("\n***%s: %s.\n",INDEXADOR,"FRASE NAO ALOCADA NA MEMORIA");
      exit(0);
   }
   unsigned int i;
   frase->iddoc = (iddoc>0)?iddoc:0;
}

/** */
void inicia_bloco(PBloco bloco, long endbloco ){
	if( !bloco ){
      printf("\n***%s: %s.\n",INDEXADOR,"BLOCO NAO ALOCADO NA MEMORIA");
      exit(0);
   }
   unsigned int i,j;
   bloco->endproxbloco = -1;
   bloco->endbloco = (endbloco>=0)?endbloco:-1;
   bloco->numfrase = 0;
}

/** */
PInfoHash novo_infohash(long endbloco, PToken valor){
   PInfoHash infoh = aloca_infohash();
	if( !infoh ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE INFOHASH");
	   exit(0);
   }
   inicia_infohash(infoh,endbloco,valor);
   return infoh;
}

/** */
PVetHash novo_vethash(){
   PVetHash vet = aloca_vethash();
	if( !vet ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE VETHASH");
	   exit(0);
   }
   inicia_vethash(vet);
   return vet;
}

/** */
PNo novo_no(long id){
   PNo no = aloca_no();
	if( !no ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE NO");
	   exit(0);
   }
   inicia_no(no,id);
   return no;
}

/** */
PFrase nova_frase(long iddoc){
   PFrase frase = aloca_frase();
	if( !frase ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE FRASE");
	   exit(0);
   }
   Frase f = {0};
   *frase = f;
   inicia_frase(frase,iddoc);
   return frase;
}

/** */
PNoEnd novo_noend(long id, long endbloco){
   PNoEnd noend = aloca_noend();
	if( !noend ){
	   printf("\n***%s: %s.\n",INDEXADOR,"SEM MEMORIA PARA ALOCACAO DE NOEND");
	   exit(0);
   }
   noend->endbloco = endbloco;
   inicia_no(&noend->no,id);
   return noend;
}

/******************************************************************************************************************/
/** */
long getSize_vet(VetHash vet){
   return hsize(*vet.info);
}

/** */
PInfoHash getVetHash(VetHash vet, PLong key){
   return hget(*vet.info,key);
}

/** */
unsigned short contemVetHash(VetHash vet, PLong key){
   return hcontainsKey(*vet.info,key);
}

/** */
void* addVetHash(PVetHash vet, PLong key, PInfoHash infoh){
   if(!vet || !key || !infoh){
		printf("\n***%s: %s.\n",INDEXADOR,"ERRO NA ADICAO DE UM VETHASH");
		exit(0);
	}
   return hput(vet->info,key,infoh);
}

/** */
unsigned short add_frase(PBloco bloco, PFrase frase){
   if(!bloco || (bloco->numfrase) == N){
		printf("\n***%s: %s.\n",INDEXADOR,"ERRO NA ADICAO DE UMA FRASE");
		exit(0);
	}
   long pos = (bloco->numfrase);
   unsigned int i;
   bloco->listafrase[pos].iddoc = frase->iddoc;
   bloco->listafrase[pos] = *frase;
   (bloco->numfrase)++;
   return 1;
}

/** */
PFrase calc_frase(long iddoc, ArrayList lend, long ini){ // ***SUPORTADA
   int i,j;
   Frase fx = {0};
   PFrase frase = aloca_frase(); 
   *frase = fx;
   inicia_frase(frase,iddoc);
   for(i=ini,j=0;j<M && i<size(lend);i++,j++){
      PNoEnd noend = (PNoEnd)getData(i,lend);
      frase->listano[j] = noend->no;
   }
   return frase;
}

/** */
int calc_listafrase2(long iddoc, PArrayList lfrase, No n, int qtd){
  if(!lfrase)return qtd;
  
  int t = size(*lfrase);
  int i = (qtd%M);
  PFrase f;

  if(t == 0 || i == 0){
      f = nova_frase(iddoc);
      f->listano[0] = n;
      add_data(f,lfrase);
  }
  else{
      f = (PFrase)getData(t-1,*lfrase);
      f->listano[i] = n;
      setData(t-1,f,lfrase);
  }
  qtd++;
  return qtd;
}

/** */
HashMap calcHashTok(VetHash vet){
   HashMap hashtok;
   init_hashmap(&hashtok,equals_tokens_hash,hash_token_hash);
   unsigned long i;

   for(i=0;i<vet.info->tablesize;i++){
      PItemh e = vet.info->table[i];
      while (e->key != NULL) {
         PToken tok = ((PInfoHash)e->value)->valor;
         hput(&hashtok,tok,e->key);
         e = e->next;
      }
   }
   return hashtok;
}

/******************************************************************************************************************/
/** */
void gravaVetHash(VetHash vet,char* arqvoc){
   FILE* arq;
   unsigned int i;

   if(!(arq = abre_arquivo_bin(arqvoc,"wb+"))){
      exit(1);
   }

   vet.qtdtok = getSize_vet(vet);
   // Grava os campos da estrutura do vethash
   fwrite(&vet,sizeof(VetHash),1,arq);

   // Grava as informacoes do vethash
   for(i=0; i<vet.info->tablesize; i++){
      PItemh e = vet.info->table[i];
      while (e->key != NULL) {
         PInfoHash infoh = (PInfoHash)e->value;
         long pid = getLong(*(PLong)e->key);
         fwrite(infoh,sizeof(InfoHash),1,arq);
         fwrite(&pid,sizeof(long),1,arq);
         fwrite(getValorT(*infoh->valor),infoh->tamvalor,1,arq);
         e = e->next;
      }
   }
   fclose(arq);
}

/** */
void gravaLogIndex(VetHash vet,char* nomebase){
   char* arqlog;
   // Log da indexacao
   arqlog = malloc(sizeof(char)*(strlen(nomebase)+strlen("log.")+1));
   strcpy(arqlog,"log.");
   strcat(arqlog,nomebase);
   
   FILE* fal = abre_arquivo_texto(arqlog,"w+");
   fprintf(fal,"***LOG INDEXACAO***\n");
   fprintf(fal,"\nBase: %s",nomebase);
   fprintf(fal,"\nQtd. Colecao: %d",vet.qtdcolecao);
   fprintf(fal,"\nQtd. Bloco  : %d",vet.numbloco);
   fprintf(fal,"\nQtd. Token  : %d",vet.qtdtok);
   fprintf(fal,"\nTempo Index : %.4fs",vet.tindex);
   fclose(fal);
   free(arqlog);arqlog=NULL;
}

/** */
void carregaVetHash(PVetHash vet, char arquivol[255]){
   FILE *arq;
   char* str;
   long id=0;
   register unsigned long i=0;

   if(!(arq=abre_arquivo_bin(arquivol,"rb"))){
      exit(1);
   }
   // Carrega os campos de titulos
   fread(vet,sizeof(VetHash),1,arq);
   vet->info   = create_hashmap(def_equals,def_hash);
   // Carrega as informacoes do vethash
   while(!feof(arq)){
      PInfoHash infoh = aloca_infohash();
      if(fread(infoh,sizeof(InfoHash),1,arq)){ // infohash
         if(!fread(&id,sizeof(long),1,arq)){ // ID
            break;
         }
         str = calloc((infoh->tamvalor)+1,sizeof(char)) ;
         if(fread(str,infoh->tamvalor,1,arq)){
            PToken nt = novo_token2(str);//*** Atualizacao
            infoh->valor = nt;
            i++;
            addVetHash(vet,novo_long(id),infoh);
          }
          else break;
      }
      else break;
   }
   fclose(arq);
}

/** */
void carregaInfo(PLong numbloco, PLong qtdcolecao, PLong qtdtok, char arquivol[255]){
   FILE *arq;
   char* str;
   long id=0;
   VetHash vet;

   if(!(arq=abre_arquivo_bin(arquivol,"rb"))){
      exit(1);
   }
   // Carrega os campos de titulos
   inicia_vethash(&vet);
   if(fread(&vet,sizeof(VetHash),1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE LEITURA DE BLOCO");
      exit(0);
   }
   numbloco->valor = vet.numbloco;
   qtdcolecao->valor = vet.qtdcolecao;
   qtdtok->valor = vet.qtdtok;
   fclose(arq);
}

/** Carrega o vocabulario retornando a estrutura em si*/
PVetHash carrega_voc(char* nomearqvoc){
   PVetHash pvet;

   if(ver_arquivo(nomearqvoc,'b')){
      pvet = aloca_vethash();
      inicia_vethash(pvet);
      carregaVetHash(pvet, nomearqvoc); // Carrega o voc
   }
   else{
      printf("\n***%s: %s.\n",INDEXADOR,"ARQUIVO DE VOCABULARIO NAO EXISTE OU NAO FOI POSSIVEL ABRI-LO");
      pvet = NULL;
   }

   return pvet;
}

/** */
PBloco novo_bloco(PVetHash vet,FILE *arq){
   PBloco novobloco = aloca_bloco();

   inicia_bloco(novobloco,vet->numbloco);
   if(fseek(arq,0,SEEK_END)==-1 || fwrite(novobloco,TAMBLOCO,1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE ALOCACAO DE BLOCO");
      exit(0);
   }
   else{
      (vet->numbloco)++;
      return novobloco;
   }
}

/** */
void diskRead(FILE *arq, long endbloco, PBloco bloco){
   if(fseek(arq,endbloco*TAMBLOCO,SEEK_SET)==-1 ||
      fread(bloco,TAMBLOCO,1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE LEITURA DE BLOCO");
      exit(0);
   }
}

/** */
void diskWrite(FILE *arq, long endbloco, PBloco bloco){
   if(fseek(arq,endbloco*TAMBLOCO,SEEK_SET)==-1 ||
      fwrite(bloco,TAMBLOCO,1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO NA ESCRITA DE BLOCO");
      exit(0);
   }
}

/** */
void diskReadBase(FILE *arq, long linha, PIndBase ib){
   if(fseek(arq,(linha)*sizeof(IndBase),SEEK_SET)==-1 ||
      fread(ib,sizeof(IndBase),1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE LEITURA DE BLOCO DO ARQUIVO DE INDICE DA BASE");
      exit(0);
   }
}

/** */
void diskWriteBase(FILE *arq, long linha, PIndBase ib){
   if(fseek(arq,(linha)*sizeof(IndBase),SEEK_SET)==-1 ||
      fwrite(ib,sizeof(IndBase),1,arq)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO NA ESCRITA DE DO ARQUIVO DE INDICE DA BASE");
      exit(0);
   }
}

/** */
void cria_indexador(PVetHash vet, char* arqlista, FILE **arq){
   if(!vet){
      printf("\n***%s: %s.\n",INDEXADOR,"HASH DE VOCABULARIO NAO ALOCADO");
		exit(0);
	}
   // Cria  arquivo invertido
   if(!(*arq = fopen(arqlista,"wb+"))){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE ABERTURA DO ARQUIVO INVERTIDO");
      exit(0);
   }
}

/** */
long indexa_base(char* nomearq, char* nomearqx){
   FILE* f;
   FILE* x;

   f = abre_arquivo_texto(nomearq,"r");
   if(f == NULL){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE ABERTURA DA BASE");
      exit(0);
   }
   if(!(x = abre_arquivo_bin(nomearqx,"wb+"))){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE CRIACAO DO INDICE DA BASE");
      exit(0);
   }
   
   unsigned long i;
   Token tok;
   IndBase ib;
   setToken(&tok,NULL);
   i = 1;
   printf("\n");
   do{
      printf("\r %d",i);
      ib.pos = ftell(f); // posicao
      ler_linha(f,&tok);
      ib.tam = getSizeT(tok); // tamanho
      ib.linha = i; // linha
      ib.endbloco = -1;
      fwrite(&ib,sizeof(IndBase),1,x);
      i++;
      if(feof(f))break;
   }while(1);
   printf("\n");
   fclose(f);
   fclose(x);
   return i-1;
}

/** */
PToken retLinha(FILE *f,FILE *x, long linha, PIndBase ib){
   IndBase ibase;

   // Le indice da base
   if(fseek(x,(linha)*sizeof(IndBase),SEEK_SET)==-1 ||
      fread(&ibase,sizeof(IndBase),1,x)==-1){
      printf("\n***%s: %s.\n",INDEXADOR,"ERRO DE LEITURA DE REGISTRO BASE");
      exit(0);
   }
   char registro[ibase.tam+1]; // valor da linha

   // Le registro da base
   rewind(f);
   fseek(f,ibase.pos,SEEK_SET);
   fgets (&registro[0], ibase.tam+1, f);
   
   if(ib){
      ib->tam = ibase.tam;
      ib->pos = ibase.pos;
      ib->linha = ibase.linha;
      ib->endbloco = ibase.endbloco;
   }
   
   PToken tok = novo_token(registro);
   return tok;
}

/** */
PToken retLinha2(char* nomebase,char* nomeindbase, long linha, PIndBase ib){
   FILE*f,*x;

   if(!(f = abre_arquivo_texto(nomebase,"r"))){ // abre base em si
       return NULL;
   }
   if(!(x = abre_arquivo_bin(nomeindbase,"rb+"))){ // abre indice da base
       return NULL;
   }
   PToken res = retLinha(f,x,linha,ib);
   fclose(f);
   fclose(x);
   return res;
}

/******************************************************************************************************************/
/** */
long add_bloco(PVetHash vet, PBloco bloco, long endultbloco, ArrayList lfrase,FILE *arq){ // ***SUPORTADO
   if(!vet || !bloco){
		printf("\n***%s: %s.\n",INDEXADOR,"VETHASH OU BLOCO NAO ALOCADO NA FUNCAO \"ADD_BLOCO\"");
		exit(0);
	}
   unsigned int i;
   long endbloco;
   PBloco bloconovo;

   // Vai para o ultimo bloco de overflow, caso exista...
   /*while(bloco->endproxbloco!=-1){
      diskRead(arq,bloco->endproxbloco,bloco);
   }*/
   if(bloco->endproxbloco!=-1){
      diskRead(arq,endultbloco,bloco);
	}

   for(i=0;i<size(lfrase);i++){
      PFrase frase = (PFrase)getData(i,lfrase);
      // Se o Bloco Cheio...
      if(bloco->numfrase == N){
         bloconovo = novo_bloco(vet,arq);
         bloco->endproxbloco = bloconovo->endbloco;
         endbloco = bloco->endbloco;
         diskWrite(arq,endbloco,bloco);
         *bloco = *bloconovo;
         free(bloconovo);bloconovo=NULL;
      }
      add_frase(bloco,frase);
   }
   diskWrite(arq,bloco->endbloco,bloco);
   return bloco->endbloco;
}

/** */
unsigned short add_doc2(PVetHash vet, TreeMap treetok, ArrayList lfrase, FILE *arq){ // ** REPETICAO
   if(!vet){
		printf("\n***%s: %s.\n",INDEXADOR,"ERRO DURANTE ADICAO DE BLOCO");
		exit(0);
	}
   Bloco bloco;
   PItemtm inf;
   PLong pid = novo_long(0);

   for(inf = firstEntry(treetok);inf!=NULL;inf=successor(inf)){
       PNoEnd noend = (PNoEnd)inf->value;
       long endbloco = noend->endbloco;
       setLong(pid,noend->no.id);
       PInfoHash inf = getVetHash(*vet,pid);
       diskRead(arq,endbloco,&bloco);
       //if(!inf)printf("\nAqui");
       inf->endultbloco = add_bloco(vet,&bloco,inf->endultbloco,lfrase,arq);
   }
   (vet->qtdcolecao)++;
   free(pid);pid=NULL;
   return 1;
}

/** */
PArrayList get_doc_frase(long endbloco, FILE *arq){
   Bloco bloco;
   PArrayList lfrase = create_arraylist(NULL,NULL);
   int i;

   do{
      diskRead(arq,endbloco,&bloco);
      for(i=0;i<bloco.numfrase;i++){
         printf("\rDOC=%d",bloco.listafrase[i].iddoc);
         add_data(&bloco.listafrase[i],lfrase);
      }
      endbloco = bloco.endproxbloco;
   }while(bloco.endproxbloco!=-1);
   return lfrase;
}

/** */
PArrayList get_doc_bags(VetHash vet, long endbloco, FILE *arq){
   PArrayList ltoks = create_arraylist(NULL,NULL);
	PBag bag;
	Bloco bloco;
	Long lid;
	Frase frase;
   unsigned long i,j;
   long iddoc , iddocant = 0;
   double dfr, w;

   do{
      diskRead(arq,endbloco,&bloco);
      for(i=0;i<bloco.numfrase;i++){ // Para cada frase do bloco
         frase = bloco.listafrase[i];
         iddoc = frase.iddoc;
         if(iddoc != iddocant){
            if(iddocant != 0){
               add_data(bag,ltoks);
            }
            bag = novo_bag2(iddoc);
            iddocant = iddoc;
         }
         // Recupera os (nos)tokens da frase(registro)
         for(j=0;j<M;j++){ // Para cada no da frase
            if(frase.listano[j].id!=0){
               setLong(&lid,frase.listano[j].id);
               PInfoHash info = getVetHash(vet,&lid);
               PToken t = info->valor;
               setBag(bag,t);
               PFloat peso = getPeso(*bag,t);
               setFloat(peso,frase.listano[j].qtd);
               // Atualiza o peso
			      dfr = info==NULL ? 1.0 : info->tamlista;
			      w = log(frase.listano[j].qtd + 1) * log( vet.qtdcolecao/dfr );
               setFloat(peso,w);
      			setNorma(bag, getNorma(*bag)+(w*w));
            }
         }
      }
      endbloco = bloco.endproxbloco;
   }while(bloco.endproxbloco!=-1);
   add_data(bag,ltoks);
   return ltoks;
}

/** */
PArrayList get_doc_toks(VetHash vet, long endbloco, FILE *arq){
   Bloco bloco;
   PArrayList ltoks = create_arraylist(NULL,NULL);
   PBag bag;
   Frase frase;
   Long lid;
	unsigned long i,j;
   long iddoc, iddocant = 0;

   do{
      diskRead(arq,endbloco,&bloco);
      for(i=0;i<bloco.numfrase;i++){
         frase = bloco.listafrase[i];
         iddoc = frase.iddoc;

         if(iddoc != iddocant){
            if(iddocant != 0){
               add_data(bag,ltoks);
            }
            bag = novo_bag2(iddoc);
            iddocant = iddoc;
         }
         for(j=0;j<M;j++){
            if(frase.listano[j].id!=0){
               setLong(&lid,frase.listano[j].id);
               PInfoHash info = getVetHash(vet,&lid);
               PToken t = info->valor;
               setBag(bag,t);
               PFloat peso = getPeso(*bag,t);
               setFloat(peso,frase.listano[j].qtd);
            }
         }
      }
      endbloco = bloco.endproxbloco;
   }while(bloco.endproxbloco!=-1);
   add_data(bag,ltoks);
   return ltoks;
}

/******************************************************************************************************************/
/****************ESSE*EH*O*CARA*************************************************/
/*******************************************************************************/
PVetHash indexador_online(char* nomearq/*Base em si*/, char* nomearqx/*Indice da base*/,
                          char *nomearqvoc /*Vocabulario*/, char *nomearqinv/*Lista Invertida*/, char sep){
   FILE* arq,* f,* x;
   unsigned long i,j;
   long endbloco, id=0, tam=0, qtd = 0;
   clock_t tini, tfim;
   PVetHash vet = novo_vethash();
   PInfoHash inf;
   PArrayList lfrase;
   PLong pid;
   PBloco bloco;
   PToken tok;
   TreeMap treetok;
   HashMap hashtok;
   IndBase ib;
   PTokenizer toks = novo_tokenize();
   No n;

   init_hashmap(&hashtok,equals_tokens_hash,hash_token_hash);
   init_treemap(&treetok,(void*)equals_tokens_dist,NULL);
   lfrase = create_arraylist(NULL,NULL);

   printf("\n\n->Criando o arquivo de indice da Base...\n");
   tini = clock(); // Tempo inicial
   tam = indexa_base(nomearq,nomearqx); // Indexa posicionalmente a base = Indice da base
   printf("->Num. de Registros da base: %d\n",tam);
   if(tam == 0){
      printf("\n***%s: %s.\n",INDEXADOR,"A BASE ESTA VAZIA");
		exit(0);
	}

   cria_indexador(vet,nomearqinv,&arq);
   if(!(f = abre_arquivo_texto(nomearq,"r"))){ // abre base em si
       return NULL;
   }
   if(!(x = abre_arquivo_bin(nomearqx,"rb+"))){ // abre indice da base
       return NULL;
   }

   printf("->Criando Arquivo Invertido e Vocabulario...\n");
   for(i=0;i<tam;i++){ // Para cada registro da base
      PToken doc = retLinha(f,x,i,&ib);
      if(doc == NULL){
         printf("\nERRO DE LEITURA DE BLOCO DO ARQUIVO DE INDICE DA BASE,\n LINHA %d",i);
         exit(1);
      }
      if(sep==0)
         tokenize(toks,*doc,0,0); // Tokeniza para qualquer base simples
      else
      	tokenize_sep(toks,*doc,sep,0,0); // Tokeniza com separador especificos

		if((i+1)%20==0)printf("\r %d",(i+1));
      // Para cada token do Registro Atual
      for(j=0;j<toks->size;j++){
         tok = toks->tokens[j];
         pid = hget(hashtok,tok);
         if(pid == NULL){
            id++;
            pid = novo_long(id);
            hput(&hashtok,tok,pid);
            bloco = novo_bloco(vet,arq);
            endbloco = bloco->endbloco;
            inf = novo_infohash(endbloco,tok);
            addVetHash(vet,pid,inf);
         }
         else{
            inf = getVetHash(*vet,pid);
            endbloco = inf->endbloco;
         }
         // Forma o documento
         PNoEnd noend = (PNoEnd)get_tm(treetok,tok);
         if(noend == NULL){
            noend = novo_noend(getLong(*pid),endbloco);
            noend->no.qtd = 1;
            put_tm(&treetok,tok,noend);
            qtd = calc_listafrase2(i+1,lfrase,noend->no,qtd);
            (inf->tamlista)++;
         }
         else{
            (noend->no.qtd)++;
         }
         free(bloco);bloco=NULL;//---
      }
      ib.endbloco = endbloco;
      diskWriteBase(x,i,&ib);
      add_doc2(vet,treetok,*lfrase,arq);
      // Limpa os dados
      freeT(doc);
      clear_tm(&treetok,NULL,free);
      clear(lfrase,free);
      clearTokenize(toks);
   }
   printf("\r %d",(i));
	tfim = clock(); // Tempo final
   vet->tindex = (tfim-tini)/(double)CLOCKS_PER_SEC;
   // Mostra e grava os resultados
   vet->qtdtok = getSize_vet(*vet);
   printf("\n\n Num. Tokens = %d",vet->qtdtok);
   printf("\n Num. Registros = %d",vet->qtdcolecao);
   printf("\n Num. Blocos = %d",vet->numbloco);
   printf("\n Tempo Indexacao = %.4fs",vet->tindex);
   
   gravaVetHash(*vet,nomearqvoc);
   gravaLogIndex(*vet,nomearq);
   fclose(arq);
   fclose(f);
   fclose(x);

   free_arraylist(lfrase,free);
   clear_tm(&treetok,NULL,free);
   clear_hm(&hashtok,NULL,NULL);
   freeTokenize(toks);

   return vet;
}
