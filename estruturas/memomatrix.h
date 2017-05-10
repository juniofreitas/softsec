/* IMPLEMENTA UMA MATRIZ DINAMICA
   QUE CONTEM OS DADOS DE UMA COMPUTACAO DE UMA FUNCAO DE SIMILARIDADE
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* Estrutura que representa uma matriz dinâmica de computacao de funcao */
typedef struct tmat Mat;
typedef struct tmat* PMat;
struct tmat{
   double **value;
   char **computed;
   Token s;
   Token t;
   char printNegativeValues;
   double (*compute)(PMat mat, int i,int j);
};

PMat aloca_mat();
void inicia_mat(PMat m, Token s, Token t, double (*compute)(PMat mat, int i,int j));
PMat nova_mat(Token s, Token t, double (*compute)(PMat mat, int i,int j));
double default_compute(PMat m,int i, int j);
void setCompute(PMat m, double (*compute)(PMat mat, int i,int j));
double getMat(PMat m,int i,int j);
char sAt(Mat m,int i);
char tAt(Mat m,int i);
void verLinha(Mat m,int i);
void verColuna(Mat m,int j);
void setPrintNegativeValues(PMat m,char flag);
char* toStringMat(Mat m, char flag);
void freeMat(PMat m);
double matchScore01(char c,char d);
double matchScore21(char c,char d);
static double max3(double x,double y,double z);
static double max4(double w,double x,double y,double z);
/*----------------------------------------------------------------------------*/
/** */
PMat aloca_mat(){
   return malloc(sizeof(Mat));
}

/** */
void inicia_mat(PMat m, Token s, Token t, double (*compute)(PMat mat, int i,int j)){
   if(!m)return;
   register int l;
   inicia_token(&m->s,getValorT(s));
   inicia_token(&m->t,getValorT(t));
	m->value    = calloc(getSizeT(m->s)+1,sizeof(double*));
   m->computed = calloc(getSizeT(m->s)+1,sizeof(char*));
   for(l=0; l<getSizeT(m->s)+1;l++){
      m->value[l]    = calloc(getSizeT(m->t)+1,sizeof(double));
      memset(m->value[l],0,sizeof(m->value[l]));
      m->computed[l] = calloc(getSizeT(m->t)+1,sizeof(char));
      memset(m->computed[l],0.0,sizeof(m->computed[l]));
   }
   m->printNegativeValues = 0;
   m->compute = (compute)?compute:default_compute;
}

/** */
PMat nova_mat(Token s, Token t, double (*compute)(PMat mat, int i,int j)){
   PMat novamat = aloca_mat();
   if(novamat) inicia_mat(novamat,s,t,compute);
   else{
      printf("\nSEM MEMORIA\n");
      exit(0);
   }
   return novamat;
}

/*----------------------------------------------------------------------------*/
/** */
double default_compute(PMat m,int i, int j){
   return sAt(*m,i)/95.0 * sAt(*m,j)/95.0;
}

/** */
void setCompute(PMat m, double (*compute)(PMat mat, int i,int j)){
   if(!m)return;
   m->compute = (compute)?compute:default_compute;
}

/** */
double getMat(PMat m,int i,int j){
   verLinha(*m,i);
   verColuna(*m,j);
	if (!m->computed[i][j]) {
       m->value[i][j] = m->compute(m,i,j);
       m->computed[i][j] = 1;
	}
	return m->value[i][j];
}

/** */
char sAt(Mat m,int i){
   verLinha(m,i);
	return getValorT(m.s)[i-1];
}

/** */
char tAt(Mat m,int i){
   verColuna(m,i);
	return getValorT(m.t)[i-1];
}

/** */
void verLinha(Mat m,int i){
   if(i<0 && i>getSizeT(m.s)){
      printf("\n Linha fora da faixa -> %d (Tam:%d)\n",i,getSizeT(m.s));
      exit(1);
   }
}

/** */
void verColuna(Mat m,int j){
   if(j<0 && j>getSizeT(m.t)){
      printf("\n Coluna fora da faixa -> %d (Tam:%d)\n",j,getSizeT(m.t));
      exit(1);
   }
}

/** */
void setPrintNegativeValues(PMat m,char flag) {
	m->printNegativeValues = flag;
}

/** Print the matrix, for debugging and/or explanation. */
char* toStringMat(Mat m, char flag){
	Token buf;
	unsigned register int i,j;

   setPrintNegativeValues(&m,flag);
   
	// line 1
	inicia_token(&buf,"   ");

	for (i=1; i<=getSizeT(m.s); i++){
   	concatChar(&buf," ");
	   concatChar(&buf,toChar(sAt(m,i)));
	   concatChar(&buf," ");
   }
   
	concat(&buf,novo_token("\n"));

	// line 2
	concat(&buf,novo_token("   "));
	for (i=1; i<=getSizeT(m.s); i++) concatChar(&buf,"---");
	concatChar(&buf,"\n");
	// remaining lines
	for (j=1; j<=getSizeT(m.t); j++) {
    concatChar(&buf," ");
    concatChar(&buf,toChar(tAt(m,j)));
    concatChar(&buf,"|");
    //concat(&buf,novo_token(strcat(strcat(" ",toChar(tAt(m,j))),"|")));
    for (i=1; i<=getSizeT(m.s); i++) {
			double v = m.printNegativeValues ? -getMat(&m,i,j) : getMat(&m,i,j);
			char* sv = malloc(sizeof(char)*10);
			sprintf(sv," %.1f ",v);
			concatChar(&buf,sv);
    }
    concatChar(&buf,"\n");
	}

	return getValorT(buf);
}

/** */
void freeMat(PMat m){
   unsigned register i;
   for(i=0;i<getSizeT(m->s)+1;i++){
      free(m->value[i]);
      free(m->computed[i]);
   }
   free(m->value);
   free(m->computed);
   free(m);
}

//------------------------------------------------------------------------------
/** tipo 0*/
double matchScore01(char c,char d) {
	return tolower(c)==tolower(d) ? 0 : -1;
}

/** tipo 1*/
double matchScore21(char c,char d) {
	return tolower(c)==tolower(d) ? 2 : -1;
}

/** Return max of three numbers. */
static double max3(double x,double y,double z) {
	return max(x, max(y,z) );
}

/** Return max of four numbers. */
static double max4(double w,double x,double y,double z) {
	return max(max(w,x), max(y,z) );
}
