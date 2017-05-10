/* CLASSE QUE TRATA DOS DADOS DE ENTRADA PROVINDOS DE ARQUIVOS E AINDA TRAZ OUTRAS FUNCOES
	AUXILIARES
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DADOS "DADOS"

FILE* abre_arquivo_texto(char *arql,char* tipo);
FILE* abre_arquivo_bin(char *arql,char* tipo);
char ver_arquivo(char *nomearq,char tipo);
short ler_linha(FILE *f, PToken regs);
void carrega(PArrayList dados, char* nomearq);
void escreve_hashtoken(char *arq, ArrayList dados);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/** Abre arquivo texto */
FILE* abre_arquivo_texto(char *arql,char* tipo){
   FILE *arq;
   if(strcmp(tipo,"r")!=0 && strcmp(tipo,"w")!=0 &&
      strcmp(tipo,"w+")!=0 && strcmp(tipo,"r+")!=0)
      strcpy(tipo,"r");
      
   if(!(arq=fopen(arql,tipo))){
      printf("\n***%s: ARQUIVO TEXTO %s NÃO PODE SER ABERTO\n",DADOS,arql);
      return NULL;
   }
   return arq;
}

/** Abre arquivo binario */
FILE* abre_arquivo_bin(char *arql,char* tipo){
   FILE *arq;
   if(strcmp(tipo,"rb")!=0 && strcmp(tipo,"rb+")!=0 &&
      strcmp(tipo,"wb")!=0 && strcmp(tipo,"wb+")!=0)
      strcpy(tipo,"rb");

   if(!(arq=fopen(arql,tipo))){
      if(strcmp(tipo,"rb")==0){
         arq = fopen(arql,"wb");
         fclose(arq);
         if(!(arq = fopen(arql,"rb"))){
            printf("\nARQUIVO BINARIO %s NÃO PODE SER ABERTO\n",arql);
            return NULL;
         }
      }
      else{
         printf("\nARQUIVO BINARIO %s NÃO PODE SER ABERTO\n",arql);
         return NULL;
      }
   }
   return arq;
}

/** Verifica se o arquivo existe e se o mesmo está correto */
char ver_arquivo(char *nomearq, char tipo){
   // Tipo 't' == text; 'b' == binario;
   FILE *arq;
   if(tipo != 't' && tipo != 'b'){
      printf("\nPARAMETRO ERRADO\n");exit(1);
   }
   if(tipo == 't')
      arq=fopen(nomearq,"r");
   else
      arq=fopen(nomearq,"rb");
   char res = (arq != NULL);

   if(res)fclose(arq);
   
   return res;
}

/** Le uma linha do arquivo e a retorna -> MODO NAO INDEXADO */
short ler_linha(FILE *f, PToken regs){
    unsigned register long tamlinha;
    char* linha = malloc(sizeof(char)*10);
    char ch;

    tamlinha = 0;
    ch = fgetc(f);    
    while((ch !='\n') && (ch !=EOF)/* && (ch != '\r')  && (ch != '\0')  && (ch != '\f')*/){       
       linha[tamlinha] = ch;
       tamlinha++;
       if(tamlinha>sizeof(linha))
          linha = realloc(linha,sizeof(char)*(tamlinha+10));
       ch = fgetc(f);   
    }
    linha = realloc(linha,sizeof(char)*(tamlinha+1));
    linha[tamlinha] = '\0';

    if(tamlinha==0)
       return 0;
    setValorT(regs,linha);
    return 1;
}

/** Carrega todas as linhas do arquivo -> MODO NAO INDEXADO */
void carrega(PArrayList dados, char* nomearq){
   FILE* f;
   f = abre_arquivo_texto(nomearq,"r");

   if(dados == NULL || f == NULL) return;
   register unsigned long i=0;
   PToken dado;
   do{
       if(!feof(f)){
          dado = novo_token(NULL);
          if(ler_linha(f,dado)>0);
             add(create_element(dado,i),dados);
          i++;
       }
       else{
          fclose(f);
          return;
       }
   }while(1);
}

/******************************************************************************************************************/
/** Funcao auxiliar ...*/
void escreve_hashtoken(char *arq, ArrayList dados){
    if(size(dados)==0)return;

    FILE *f;
    register unsigned long i;

    if(!(f = fopen(arq,"w+"))){
        printf("\nERRO DE ABERTURA\n");
        exit(1);
    }

    for(i=0;i<size(dados);i++){
       PToken item = getData(i,dados);
       fprintf(f,"%s = %ld\n",getValorT(*item),(hashCodeT01(*item)));
    }
    fclose(f);
}
