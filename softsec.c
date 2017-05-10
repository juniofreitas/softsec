#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "estruturas/pares.h"
#include "estruturas/token.h"
#include "estruturas/arraylist.h"
#include "estruturas/treemap.h"
#include "estruturas/hashmap.h"  
#include "estruturas/tokenizer.h"
#include "estruturas/memomatrix.h"
#include "estruturas/bagoftokens.h"
#include "dados/dados.h"
#include "dados/indexador.h"
#include "dados/multindexador.h"
#include "funcoes/jaro_winkler.h"
#include "funcoes/levenstein.h"
#include "funcoes/monge_elkan.h"
#include "funcoes/smith_waterman.h"
#include "funcoes/jaccard.h"
#include "funcoes/felligi_sunter.h"
#include "funcoes/tfidf.h"
#include "funcoes/soft_tfidf.h"
//#include "dados/matchdata.h"
//#include "dados/avaliador.h"

#define QTDPADRAO 20
#define INDEX 1
#define BUSCA 2
#define EXPERIMENTO 3
#define TESTE 4
/* Especifica o tipo de algoritmo de blocagem a ser usado*/
#define PARAPAR 1
#define TOKENCOMUUM 2

/* Arquivo geral de resultado de busca OK*/
#define RESULT "res_"
/* Arquivo de experimento de referencia para avaliacao OK*/
#define REFERENCIA "xref_"

void argerro(){
    printf("\nArgumentos invalidos! \n");
    printf(" Para indexar use: \"softsec -i [base][arqvoc][arqlista][indbase]\"\n");
    printf(" Para buscar  use: \"softsec -b [base][arqvoc][arqlista][indbase][consulta]\"\n");
    printf(" Para experimentos use: \"softsec -e [base][arqvoc][arqlista][indbase]\"\n");
    exit(0);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
int main(int argc, char *argv[]){
   register int i,j,tam;
   clock_t tini=0, tfim=0;
   FILE* arq; // Base
   FILE* arqx;// Indice da base
   FILE* arql; // Lista invertida
   FILE* arqv; // Vocabulario
   char* nomearq; // Nome do arquivo da base
   char* nomearqx; // Nome do arquivo de indice da base
   char* arqvoc; // Nome do arquivo de vocabulario
   char* arqlista; // Nome do arquivo de listas invertidas
   char* consulta; // Descrição da consulta
   short tipo; // Tipo do processamento (1 para indexação e 2 para busca)
   VetHash vet;
   HashMap hashtok;
   PTreeMap treerelev;
   PPar parf;
   /* Entrada: - softsec [indexa][base][arqvoc][arqlista][indbase]
               - softsec [busca][base][arqvoc][arqlista][indbase][consulta]
               - softsec [experimento][base][arqvoc][arqlista][indbase]
      Onde:
        [indexa] = -i (indica ao programa que a base sera indexada)
        [busca]  = -b (indica ao programa que sera realizado uma consulta)
        [experimento] = -e (indica ao programa que sera realizado um experimento)
        [base]   = nome do arquivo que contem a base a ser indexada
        [arqvoc] = nome do arquivo de vocabulario gerado pela indexacao e usado para busca
        [arqlista] = nome do arquivo de listas invertidas gerado pela indexacao e usado para busca
        [consulta] = descrição da consulta a ser realizada durante a busca
        [indbase] = nome do arquivo de indice da base
   */
   if(argc != 6 && argc != 7){
      argerro();
   }
   else{
       if((strcmp((char*)argv[1],"-i")!=0)&&(strcmp((char*)argv[1],"-b")!=0)&&
          (strcmp((char*)argv[1],"-e")!=0)&&(strcmp((char*)argv[1],"-t")))
          argerro();
       if(strcmp((char*)argv[1],"-i")==0 && argc==6){
          tipo = INDEX;
       }
       else
       if(strcmp((char*)argv[1],"-b")==0 && argc==7){
          tipo = BUSCA;
          consulta = (char*)argv[6];
       }
       else
       if(strcmp((char*)argv[1],"-e")==0 && argc==6){
          tipo = EXPERIMENTO;
          //consulta = (char*)argv[6];
       }
       else
       if(strcmp((char*)argv[1],"-t")==0 && argc==6){
          tipo = TESTE;
       }
       else{
          argerro();
       }
       nomearq  = (char*)argv[2];
       arqvoc   = (char*)argv[3];
       arqlista = (char*)argv[4];
       nomearqx = (char*)argv[5];
    }

    if(tipo == INDEX){
       printf("\n*** SOFT-TFIDF - INDEXACAO ***\n");
    }
    else
    if(tipo == BUSCA){
       printf("\n*** SOFT-TFIDF - BUSCA***\n");
       printf("\n->Consulta: %s",consulta);
    }
    else
    if(tipo == EXPERIMENTO){
      printf("\n*** SOFT-TFIDF - EXPERIMENTO***\n");
    }
    else{
      printf("\n*** SOFT-TFIDF - TESTE***\n");
    }
    
    printf("\n->Base de dados: %s",nomearq);
    printf("\n->Vocabulario: %s",arqvoc);
    printf("\n->Lista Invertida: %s",arqlista);
    printf("\n->Indice da Base: %s",nomearqx);

    //------INDEXACAO-----------------------------------------------------------
    if(tipo == INDEX){
       printf("\n\n->INDEXANDO... aguarde\n");
       indexador_online(nomearq,nomearqx,arqvoc,arqlista,0); // Separador padrao
       printf("\n\nProcesso de Indexacao terminado com sucesso!!!\n");
       return 0;
    }

    //------BUSCA---------------------------------------------------------------
    else
    if(tipo == BUSCA){
       char nomebusca[strlen(RESULT)+strlen(nomearq)+2];
       strcpy(nomebusca,"");strcat(nomebusca,RESULT);//strncat(nomebusca,consulta,20);
       strcat(nomebusca,".txt");//strcat(nomebusca,nomearq);

       printf("\n\n->BUSCANDO... aguarde\n");
       tini = clock(); // Tempo inicial
       arql = abre_arquivo_bin(arqlista,"rb");// Abre arquivo de listas invertidas
       inicia_vethash(&vet); // Inicia o voc
       carregaVetHash(&vet, arqvoc); // Carrega o voc
       hashtok = calcHashTok(vet);  // Calculando hash de toks
       printf("\n->Tamanho do Vocabulario: %d",hsize(hashtok));

       printf("\n->Calculando a Similaridade...\n");
       parf = novo_par((void*)soft_score2,(void*)jaro_winkler);
		 treerelev = busca_vetorial(*novo_token(consulta), vet,hashtok,arql,*parf);
		 fclose(arql);
		 if(!treerelev)return 0;
		 else printf("\nQtd. Respostas: %d",size_tm(*treerelev));
       tfim = clock(); // Tempo final
       printf("\nTempo busca   : %.4fs\n",((tfim-tini)/(double)CLOCKS_PER_SEC));

       // Abre base
       if(!(arq = abre_arquivo_texto(nomearq,"r")))return 0;
       if(!(arqx = abre_arquivo_bin(nomearqx,"rb")))return 0;
       // Mostra e salva os resultados
       IndBase ib;
       FILE* arb = abre_arquivo_texto(nomebusca,"w+");
       fprintf(arb,"***BUSCA***\n");
       fprintf(arb,"\nBase: %s",nomearq);
       fprintf(arb,"\nConsulta: \"%s\"",consulta);
       fprintf(arb,"\nQtd. Respostas: %d",size_tm(*treerelev));
       fprintf(arb,"\nTempo busca   : %.4fs",((tfim-tini)/(double)CLOCKS_PER_SEC));
       fprintf(arb,"\n\nResultados:\n");
		 PItemtm inf;
       for(inf = lastEntry(*treerelev),j=0;inf!=NULL;j++,inf=antecessor(inf)){
          PLong coddoc = (PLong)inf->value;
          PFloat sim = (PFloat)inf->key;
          PToken linha = retLinha(arq,arqx,getLong(*coddoc)-1,&ib);
          if(j<QTDPADRAO){
             printf("\n%d.(%d)-\"%s\" \nSIM.: \"%f\"\n",(j+1),getLong(*coddoc),getValorT(*linha),getFloat(*sim));
          }
          fprintf(arb,"\n%d.(%d)-\"%s\" \nSIM.: \"%f\"\n",(j+1),getLong(*coddoc),getValorT(*linha),getFloat(*sim));
          //fprintf(arb,"\n%s\tSIM.:\"%f\"",getValorT(*linha),getFloat(*sim) );
       }
       if(j+1>QTDPADRAO)printf("\n...");
       printf("\n\n->Resultados salvos com sucesso!!!\n");
       fclose(arb);
       fclose(arq);
       fclose(arqx);
       free_tm(treerelev,free,free);
    }
    else
    //******************************************************************************************************
    if(tipo == EXPERIMENTO){
       printf("\n\nEm desenvolvimento!!!\n");
    }
    else
    //******************************************************************************************************
    if(tipo == TESTE){ // Usado internamente
       char ascval[26] = {'a','b','c','d','e','f','g','h','i','j','k','l',
                          'm','n','o','p','q','r','s','t','u','v','w','x','y','z'};
       
		 printf("\n\nTeste GERAL!!!\n");
		 printf("\nTa=\"%s\"",arqvoc);
		 printf("\nTb=\"%s\"",arqlista);
		 Token a,b;
		 setValorT(&a,arqvoc);
		 setValorT(&b,arqlista);
		 printf("\nDistancia Levenstain entre \"%s\" e \"%s\" eh: %f",a.valor,b.valor,leven_score(a,b,0,1) );
		 printf("\nDistancia LevensNorm entre \"%s\" e \"%s\" eh: %f",a.valor,b.valor,leven_score_norm(a,b,0,1) );
       //PTreeMap tmx = create_treemap((void*)def_equals_long_tm,NULL);
       //PHashMap hmx = create_hashmap((void*)equal_par,(void*)hash_par_ex);
       //PArrayList al = create_arraylist(NULL,NULL);
 		 //PBag b;
		 //PPar p;
		 //PLong lx;

		 /*PTokenizer tks = novo_tokenize();
		 PToken t = novo_token("  Simposio Brasileiro De  banco  de DadoS  ");
		 printf("\n\nT=\"%s\"",t->valor);
		 tokenize_sep(tks,*t,' ',0,0);
		 for(i=0;i<tks->size;i++){
			 //if((i+1)%1000==0)printf("\r (%d)\t",i+1);
			 //add_data(p,al);
			 PToken ti = tks->tokens[i];
			 printf("\n(%d)=\"%s\"",(i+1),ti->valor);
			 //add_token(&tks,novo_token("teste"));
			 //put_tm(tmx,novo_long(i),novo_token("teste tree"));
			 //hput(hmx,novo_long(i),novo_token2("teste"));
			 //add_data(novo_token(teste),al);
		 }*/
		 //PToken t = untokenize(tks);printf("\nT=\"%s\"",t->valor);
		 //printf("\n<ENTER> para limpar o par...");getchar();
		 //freeBag(b);
		 //free_hm(hmx,free,free);
       //free_arraylist(al,(void*)free);
       //freeTokenize(tks);
		 //	printf("\n<ENTER> para sair...");getchar();
    }

    printf("\n\nFIM\n");
    return 0;
}
