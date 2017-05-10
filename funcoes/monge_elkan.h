#include <stdlib.h>
#include <string.h>

static char scaling = 1;
static char charMatchScore = 1;
static double openGapScore = 2;
static double extendGapScore = 1;
static double lowerBound = -MAX_DOUBLE;

static PMat is;
static PMat it;

static const int CHAR_EXACT_MATCH_SCORE = +5;
static const int CHAR_APPROX_MATCH_SCORE = +3;
static const int CHAR_MISMATCH_MATCH_SCORE = -3;

static ArrayList approx;

void montaApprox();
void setScaling(char flag);
double instmatrix_compute(PMat m,int i,int j);
double inssmatrix_compute(PMat m,int i,int j);
double matrixtrio_compute(PMat m, int i,int j);
PMat inicia_matrixtrio(Token s, Token t);
double affinegap_score(Token s,Token t,char pcharMatchScore,double popenGapScore, double pextendGapScore, double plowerBound);
double me_matchScore(char c, char d);
double mongeelkan_score(Token s,Token t,char pcharMatchScore,double popenGapScore, double pextendGapScore, double plowerBound);
/******************************************************************************/
/** */
void montaApprox() {
   init_arraylist(&approx,NULL,NULL);
   PTreeMap tmp;
   
   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("d"),novo_token("d"));put_tm(tmp,novo_token("t"),novo_token("t"));
   add_data( tmp,&approx);

   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("g"),novo_token("g"));put_tm(tmp,novo_token("j"),novo_token("j"));
   add_data( tmp,&approx);

   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("l"),novo_token("l"));put_tm(tmp,novo_token("r"),novo_token("r"));
   add_data( tmp,&approx);
   
   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("m"),novo_token("m"));put_tm(tmp,novo_token("n"),novo_token("n"));
   add_data( tmp,&approx);

   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("b"),novo_token("b"));put_tm(tmp,novo_token("p"),novo_token("p"));put_tm(tmp,novo_token("v"),novo_token("v"));
   add_data( tmp,&approx);

   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token("a"),novo_token("a"));put_tm(tmp,novo_token("e"),novo_token("e"));put_tm(tmp,novo_token("i"),novo_token("i"));
   put_tm(tmp,novo_token("o"),novo_token("o"));put_tm(tmp,novo_token("u"),novo_token("u"));
   add_data( tmp,&approx);
   
   tmp = create_treemap((void*)equals_tokens_bag, NULL);
   put_tm(tmp,novo_token(","),novo_token(","));put_tm(tmp,novo_token("."),novo_token("."));
   add_data( tmp,&approx);
}

/** If scaling is true, then distances are scaled to 0-1 */
void setScaling(char flag){
   scaling=flag;
}

/** */
double instmatrix_compute(PMat m,int i,int j){
	if (i==0 || j==0) return 0;
	return max3( lowerBound, getMat(m,i,j-1) + openGapScore, getMat(it,i,j-1) + extendGapScore );
}

/** */
double inssmatrix_compute(PMat m,int i,int j){
	if (i==0 || j==0) return 0;
	return max3( lowerBound, getMat(m,i-1,j) + openGapScore,	 getMat(is,i-1,j) + extendGapScore );
}

/** */
double matrixtrio_compute(PMat m, int i,int j) {
	if (i==0 || j==0) return 0;
   double matchScore;
   if(charMatchScore==0)
      matchScore = matchScore01( sAt(*m,i), tAt(*m,j) );
   else
   if(charMatchScore==1)
      matchScore = matchScore21( sAt(*m,i), tAt(*m,j) );
   else
   if(charMatchScore==2)
	   matchScore = me_matchScore(sAt(*m,i), tAt(*m,j));
   return max4( lowerBound, getMat(m,i-1,j-1) + matchScore,	getMat(is,i-1,j-1) + matchScore,getMat(it,i-1,j-1) + matchScore );
}

/** */
PMat inicia_matrixtrio(Token s, Token t){
   is = nova_mat(s,t,inssmatrix_compute);
   it = nova_mat(s,t,instmatrix_compute);
   return nova_mat(s,t,matrixtrio_compute);
}

/** */
double affinegap_score(Token s,Token t,char pcharMatchScore,double popenGapScore, double pextendGapScore, double plowerBound){
   charMatchScore = (pcharMatchScore!=0)?pcharMatchScore:charMatchScore;
   openGapScore = (popenGapScore!=0)?popenGapScore:openGapScore;
   extendGapScore = (pextendGapScore!=0)?pextendGapScore:extendGapScore;
   lowerBound = (plowerBound!=0)?plowerBound:lowerBound;

   PMat mat = inicia_matrixtrio(s,t);
   unsigned register int i,j;
	double best = -MAX_DOUBLE;
	for (i=0; i<=getSizeT(s); i++) {
      for (j=0; j<=getSizeT(t); j++) {
			best = max( best, getMat(mat,i,j) );
      }
	}
   freeMat(mat);
   freeMat(is);
   freeMat(it);
	return best;
}

/** tipo 2*/
double me_matchScore(char c, char d){
	c = tolower(c);
	d = tolower(d);
	if (c==d) return CHAR_EXACT_MATCH_SCORE;
	else {
      unsigned int i;
		PToken objC = novo_token(toChar(c));
		PToken objD = novo_token(toChar(d));
		for (i=0; i<size(approx); i++) {
         PTreeMap ia = getData(i,approx);
			if (get_tm(*ia,objC)!=NULL && get_tm(*ia,objD)!=NULL){
				return CHAR_APPROX_MATCH_SCORE;
         }
		}
		return CHAR_MISMATCH_MATCH_SCORE;
	}
}

/** */
double mongeelkan_score(Token s,Token t,char pcharMatchScore,double popenGapScore, double pextendGapScore, double plowerBound) {
      charMatchScore = (pcharMatchScore!=0)?pcharMatchScore:2;
      openGapScore = (popenGapScore!=0)?popenGapScore:-5;
      extendGapScore = (pextendGapScore!=0)?pextendGapScore:-1;
      lowerBound = (plowerBound!=0)?plowerBound:0;

      montaApprox();
      setScaling(1);
		if (scaling) {
			int minLen = min( getSizeT(s), getSizeT(t) );
 			return affinegap_score(s,t,charMatchScore,openGapScore,extendGapScore,lowerBound) /
                (minLen * CHAR_EXACT_MATCH_SCORE);
		} else {
			return affinegap_score(s,t,charMatchScore,openGapScore,extendGapScore,lowerBound);
		}
	}
