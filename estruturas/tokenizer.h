/* ESTA CLASSE IMPLEMENTA UM TOKENIZADOR = PROGRAMA QUE DESMEMBRA UMA STRING EM PALAVRAS(TOKENS).
	TRABALHA COM REGISTROS LIVRES OU DE MULTIPLOS ATRIBUTOS.
	A CLASSE TOKEN ESTÁ DIVIDIDA EM DUAS PARTE:
		1 - TOKENIZERS DE BASES LIVRES: POR ESPAÇO, POR TAB OU SEPARADOR ESPECÍFICO;
		2 - TOKENIZERS DE BASES CORRIGIDAS: PADRAO ESPECÍFICO;
	(*)PARTE 1:
		OS TOKENS DOS REGISTROS SAO DIVIDIDOS POR PADRAO POR ESPAÇO OU POR TABS. HÁ TAMBÉM
	   A OPÇÃO ESPECÍFICA DE SEPARADOR. EM TODOS ESSES CASOS EH POSSÍVEL DUAS OPÇÕES QUE SÃO
	   TRANSFORMAR PRA MINÚSCULO O TOKEN E SEPARAR DADOS NÚMERICOS DE ALFABÉTICOS.
		NÃO HÁ UM PADRÃO PARA OS REGISTROS, NESSE CASO OS CAMPOS PODEM VARIAR DE UM REGISTRO
	   PARA O OUTRO.
		OBS.: USA-SE SOMENTE PARA BASES LIVRES PARA UNICO ATRIBUTO.
	(*)PARTE 2:
		SEGUE O PADRAO DE REGISTRO ESPECÍFICO E OBRIGATÓRIO: [ID][TAB][DADOS].
		ONDE [ID] É O IDENTIFICADOR DO REGISTRO, OBRIGATORIAMENTE UM NÚMERO INTEIRO E SEQUENCIAL.
		ONDE [TAB] É O SEPARADOR TAB OBRIGATÓRIO QUE SEPARA O ID DOS DADOS.
		ONDE [DADOS] EH: [ATR1][SEP][ATR2][SEP]...[SEP][ATRN].
		[ATRi] EH UM ATRIBUTO, QUE PODE SER UM OU UM CONJUNTO DE TOKENS.
		E [SEP] É UM SEPARADOR ESPECÍFICO.
		OBS.: EH OBRIGATÓRIO PASSAR UM VALOR ESPECÍFICO QUE INDIQUE QUE UM ATRIBUTO ESTÁ VAZIO.
				TODOS OS REGISTROS DEVEM TER MESMO NÚMERO DE ATRIBUTO.
				CASO NAO SE PASSE NENHUM SEPARADOR OS DADOS SAO CONSIDERADOS COMO UM ATRIBUTO SÓ.
	
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define TOKENIZER "TOKENIZER"
#define TAM_MAT_ERRO_TKN 8
#define DEF_SEP '\t'
#define DEF_PADRAO "[ID][TAB][DADOS]"

unsigned short ignorePunctuation = 0; // Constante para verificar pontuacao
unsigned short ignoreCase        = 0; // Constante para verificar maiusculo/minusculo

/* Estrutura que representa um tokenizador*/
typedef struct reg_tokenize Tokenizer;
typedef struct reg_tokenize* PTokenizer;
struct reg_tokenize{
   PToken* tokens; // Vetor de tokens
   unsigned int size; // Tamanho do tokenizer
};

/*----------------------------------------------------------------------------------------------------------------*/
char* erro_tokenizer(int coderro); // Funcao de tipos de erros em tokenizer
PTokenizer aloca_tokenize();
void inic_tokenize(PTokenizer toks);
PTokenizer novo_tokenize();
void add_token(PTokenizer toks, PToken t);
//---------------------------------------------------------------------//
void tokenize(PTokenizer toks,Token input, unsigned short ip, unsigned short ic);
Tokenizer tokenize_sep(PTokenizer toks, Token input,char sep, unsigned short ip, unsigned short ic); // Tokenizacao de acordo com separador especifico
Tokenizer tokenize2(Token input, unsigned short ip, unsigned short ic); // Função que realiza a tokenizacao padrao
//---------------------------------------------------------------------//
void tokenize_corr(PTokenizer toks, Token input, char sep, char* nulo, unsigned short ic); // Para base corrida de acordo com padrao

//---------------------------------------------------------------------//
PToken untokenize(Tokenizer tks);
void clearTokenize(PTokenizer tks);
void clearTokenize_full(PTokenizer tks);
void freeTokenize(PTokenizer tks);
void freeTokenize_full(PTokenizer tks);
/*----------------------------------------------------------------------------------------------------------------*/
/** */
char* erro_tokenizer(int coderro){
	const char* mat_erro[TAM_MAT_ERRO_TKN] = {
	  "SEM MEMORIA PARA ALOCACAO",
	  "NAO ALOCADO NA MEMORIA",
	  "NAO INICIALIZADO",
	  "ERRO DE RANGE",
	  "PARAMETROS ERRADOS",
	  "REGISTRO VAZIO",
	  "FORA DO PADRAO",
	  "ERRO GENERICO"
	};
	return ((coderro>=0 && coderro<=TAM_MAT_ERRO_TKN)?(char*)mat_erro[coderro]:(char*)mat_erro[TAM_MAT_ERRO_TKN-1]);
}

/** */
PTokenizer aloca_tokenize(){
   return malloc(sizeof(Tokenizer));
}

/** */
void inic_tokenize(PTokenizer toks){
	if( !toks ){
      printf("\n***%s: %s.\n",TOKENIZER,erro_tokenizer(1));
      exit(0);
   }
   toks->size = 0;
   toks->tokens = NULL;
}

/** */
PTokenizer novo_tokenize(){
   PTokenizer tks = aloca_tokenize();
	if( !tks ){
	   printf("\n***%s: %s.\n",TOKENIZER,erro_tokenizer(0));
	   exit(0);
   }
   inic_tokenize(tks);
   return tks;
}

/** */
void add_token(PTokenizer toks, PToken t){
	if( !toks ){
	   printf("\n***%s: %s.\n",TOKENIZER,erro_tokenizer(0));
	   exit(0);
   }
   if(toks->size==0)
   	toks->tokens = malloc(sizeof(PToken)*1);
   else
		toks->tokens = realloc(toks->tokens,(toks->size+1)*sizeof(PToken));
   toks->tokens[(toks->size)++] = t;
}

/******************************************************************************************************************/
/** Essa fucação recebe como entrada uma string, um flag para pontuacao e um
    flag para maiusculo/minusculo. Em resumo, essa funcao varre a string caracter
    a caracter procurando por espacos ate formar palavras.*/
/** */
void tokenize(PTokenizer toks, Token input, unsigned short ip, unsigned short ic){
	PArrayList tokens;
 	TreeMap tokMap;
   unsigned int cursor = 0;
   unsigned int i,cursor_ini,qtdt=0;

   //init_arraylist(&tokens,(void*)equals_tokens,NULL);
   tokens = create_arraylist(NULL,NULL);
   init_treemap(&tokMap,(void*)equals_tokens,NULL);
   if(ip!=0 && ip!=1) ip = ignorePunctuation;
	if(ic!=0 && ic!=1) ic = ignoreCase;

	while (cursor<getSizeT(input)) {
	    char ch = getValorT(input)[cursor];
	    if(isspace(ch)){
				cursor++;
	    }else
		 if(isalnum(ch)) {
	    		cursor_ini = cursor;qtdt=0;
	         while(cursor<getSizeT(input) && isalnum(getValorT(input)[cursor])){
					cursor++;qtdt++;
				}
				char* aux = malloc(sizeof(char)*(qtdt+1)); // ***Tamanho do token
				if(!aux){
					printf("\nSEM MEMORIA\n");exit(0);
				}
				for(i=0;i<qtdt;i++)aux[i] = getValorT(input)[i+cursor_ini];
				aux[i] = '\0';
				PToken buf = novo_token2(aux); // ***
	  		   if(ic) toLower(buf); // Minusculo...
	         PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	         if(tok==NULL){
		         tok = buf;
	            put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	         }
	         else	if(buf)freeT(buf); // ***
	         add_data(tok,tokens); // ***
	    }else {
				if (ip) {
					char* sch = toChar(ch);
					PToken buf = novo_token2(sch);
					if(ic) toLower(buf);
	            PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	   		   if (tok==NULL) {
	   	          tok = buf;
		             put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	   		   }
	   		   else{
						freeT(buf);
					}
	   			add_data(tok,tokens);
	         }
				cursor++;
	    }
	}
   for(cursor=0;cursor<size(*tokens);cursor++){
		PToken t = getData(cursor,*tokens);
		add_token(toks,t); // ***
	}
   free_arraylist(tokens,NULL);
	clear_tm(&tokMap,NULL,NULL);
}

/** Para separador especifico */
Tokenizer tokenize_sep(PTokenizer toks, Token input,char sep, unsigned short ip, unsigned short ic){
	PArrayList tokens;
 	TreeMap tokMap;
   unsigned int cursor = 0;
   unsigned int i,cursor_ini,qtdt=0;

   //init_arraylist(&tokens,(void*)equals_tokens,NULL);
   tokens = create_arraylist(NULL,NULL);
   init_treemap(&tokMap,(void*)equals_tokens,NULL);
   if(ip!=0 && ip!=1) ip = ignorePunctuation;
	if(ic!=0 && ic!=1) ic = ignoreCase;
	if (sep <= 0 || sep == '\0' ) sep = DEF_SEP;

	while (cursor<getSizeT(input)) {
	    char ch = getValorT(input)[cursor];
	    if (ch!=sep) {
	    		cursor_ini = cursor;qtdt=0;
	         while(cursor<getSizeT(input) && getValorT(input)[cursor]!=sep){
					cursor++;qtdt++;
				}
				char* aux = malloc(sizeof(char)*(cursor+1)); // ***Tamanho do token
				if(!aux){
					printf("\nSEM MEMORIA\n");exit(0);
				}
				for(i=0;i<qtdt;i++)aux[i] = getValorT(input)[i+cursor_ini];
				aux[i] = '\0';
				PToken buf = novo_token2(aux);
	  		   if(ic) toLower(buf); // Minusculo...
	         PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	         if(tok==NULL){
		         tok = buf;
	            put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	         }
	         else{
					if(buf)freeT(buf); // ***
				}
	         add_data(tok,tokens); // ***
	    }else {
				if(ip){
					char* sch = toChar(ch);
					PToken buf = novo_token2(sch);
					if(ic) toLower(buf);
	            PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	   		   if (tok==NULL) {
	   	          tok = buf;
		             put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	   		   }
	   		   else{
						freeT(buf);
					}
	   			add_data(tok,tokens);
	         }
				cursor++;
	    }
	}
   for(cursor=0;cursor<size(*tokens);cursor++){
		PToken t = getData(cursor,*tokens);
		add_token(toks,t); // ***
	}
   free_arraylist(tokens,NULL);
	free_tm(&tokMap,NULL,NULL);
}

/** */
Tokenizer tokenize2(Token input, unsigned short ip, unsigned short ic){
   Tokenizer toks;
	ArrayList tokens;
 	TreeMap tokMap;
 	PToken* pt = NULL;
   unsigned int cursor = 0;

   init_arraylist(&tokens,(void*)equals_tokens,NULL);
   init_treemap(&tokMap,(void*)equals_tokens,NULL);
   if(ip!=0 && ip!=1) ip = ignorePunctuation;
	if(ic!=0 && ic!=1) ic = ignoreCase;

	while (cursor<getSizeT(input)) {
	    char ch = getValorT(input)[cursor];
	    if(isspace(ch)){
				cursor++;
	    }else if (isalnum(ch) ) {
	    		PToken buf = novo_token(NULL);
	         while (cursor<getSizeT(input) && isalnum(getValorT(input)[cursor])) {
	            char* sch = toChar(getValorT(input)[cursor]);
	            PToken aux = novo_token(sch);
	            free(sch);
					concat(buf,aux);
					cursor++;
				}
	  		   if(ic) toLower(buf);
	         PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	         if (tok==NULL) {
		         tok = novo_token2(getValorT(*buf));//---
	            put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	         }
	         add_data(tok,&tokens);
	    }else {
				if (ip) {
					PToken buf = novo_token(NULL);
					char* sch = toChar(ch);
					PToken aux = novo_token(sch);
					free(sch);
					concat( buf,aux );
					if(ic) toLower(buf);
	            PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	   		   if (tok==NULL) {
	   	          tok = novo_token2(getValorT(*buf));
		             put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	   		   }
	   			add_data(tok,&tokens);
	         }
				cursor++;
	    }
	}	

   inic_tokenize(&toks);
   for(cursor=0;cursor<size(tokens);cursor++){
		PToken t = getData(cursor,tokens);
		add_token(&toks,t);
	}
   clear(&tokens,NULL);
   clear_tm(&tokMap,NULL,NULL);
	return toks;
}

/******************************************************************************************************************/

/** Para campos separados por TAB*/
void tokenize_corr(PTokenizer toks, Token input, char sep, char* nulo, unsigned short ic){
	PArrayList tokens;
 	TreeMap tokMap;
 	PToken* pt = NULL,buf;
 	short qtdtab=0; // Numero de tabs, pelo menos 1
   unsigned int cursor = 0;
   unsigned int i,cursor_ini,qtdt=0;

	if(ic!=0 && ic!=1) ic = ignoreCase;
   tokens = create_arraylist(NULL,NULL);
   init_treemap(&tokMap,(void*)equals_tokens,NULL);

	while (cursor<getSizeT(input)){
	    char ch = getValorT(input)[cursor];
	    if(ch == '\t' || ch == sep){
				cursor++;
	    }else{
				if(qtdtab==1){
		    		cursor_ini = cursor;qtdt=0;
		         while(cursor<getSizeT(input) && getValorT(input)[cursor] != sep){
						cursor++;qtdt++;
					}
					char* aux = malloc(sizeof(char)*(qtdt+1)); // ***Tamanho do token
					if(!aux){
						printf("\nSEM MEMORIA\n");exit(0);
					}
					for(i=0;i<qtdt;i++)aux[i] = getValorT(input)[i+cursor_ini];
					aux[i] = '\0';
					buf = novo_token2(aux); // ***
				}
				else{
               cursor_ini = cursor;qtdt=0;
		         while(cursor<getSizeT(input) && getValorT(input)[cursor] != '\t'){
						cursor++;
						qtdt++;
					}
					char* aux = malloc(sizeof(char)*(qtdt+1)); // ***Tamanho do token
					if(!aux){
						printf("\nSEM MEMORIA\n");exit(0);
					}
					for(i=0;i<qtdt;i++){
                  if(!isdigit(getValorT(input)[i+cursor_ini])){
							if(size(*tokens)<=1){
								printf("\n***%s: %s. O ID DEVE SER NUMERICO\n",TOKENIZER,erro_tokenizer(6),DEF_PADRAO);
								exit(0);
							}
						}
						aux[i] = getValorT(input)[i+cursor_ini];
					}
					aux[i] = '\0';
					buf = novo_token2(aux); // ***
					qtdtab++;
			   }
			   // ---
	  		   if(ic) toLower(buf);
	         PToken tok = (PToken)get_tm(tokMap,(void*)getValorT(*buf));
	         if(tok==NULL){
		         tok = buf;
	            put_tm(&tokMap,(void*)getValorT(*buf),(void*)tok);
	         }
	         else if(buf)freeT(buf); // ***
	         if(strcmp(tok->valor,nulo)==0)free(setToken(tok,""));
	         add_data(tok,tokens); // ***
	    }
	}
	if(size(*tokens)<=1){
		printf("\n***%s: %s. CORRETO \"%s\"\n",TOKENIZER,erro_tokenizer(6),DEF_PADRAO);
		exit(0);
	}

   for(cursor=0;cursor<size(*tokens);cursor++){
		PToken t = getData(cursor,*tokens);
		add_token(toks,t);
	}
   free_arraylist(tokens,NULL);
   free_tm(&tokMap,NULL,NULL);
}

/******************************************************************************************************************/
/** */
PToken untokenize(Tokenizer tks){
	unsigned register int i;
	PToken nt = novo_token(NULL);
	for(i=0;i<tks.size;i++){
		PToken t = tks.tokens[i];
		concat(nt,t);
		concatChar(nt," ");
	}
	return nt;
}

/** */
void clearTokenize(PTokenizer tks){
   tks->size = 0;
   free(tks->tokens);
   tks->tokens = NULL;
}

/** */
void clearTokenize_full(PTokenizer tks){
   unsigned register int i;
	if(!tks)return;
   for(i=0;i<tks->size;i++){
      PToken tk = tks->tokens[i];
      freeT(tk);tk=NULL;
   }
   tks->size = 0;
   if(tks->tokens)free(tks->tokens);
   tks->tokens = NULL;
}

/** */
void freeTokenize(PTokenizer tks){
	clearTokenize(tks);
	if(tks)free(tks);
	tks = NULL;
}

/** */
void freeTokenize_full(PTokenizer tks){
	clearTokenize_full(tks);
	if(tks)free(tks);
	tks = NULL;
}
//---FIM---
