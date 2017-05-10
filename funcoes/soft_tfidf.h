#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

const float DEF_LIMIAR = 0.9; 

double def_token_distance(Token a, Token b);
double soft_score(Bag sBag, Bag tBag, double(*func)(Token a, Token b), double limiar);
double soft_score2(Bag sBag, Bag tBag, double(*func)(Token a, Token b));
/******************************************************************************/

/** */
double def_token_distance(Token a, Token b){
   return jaro_winkler(a,b);   
}

/*
 * */
double soft_score(Bag sBag, Bag tBag, double(*func)(Token a, Token b), double limiar){
	func = (!func)?def_token_distance:func;
	limiar = (limiar<=0 || limiar>1)?DEF_LIMIAR:limiar;
	
   double sim = 0.0;

   PItemtm tm = firstEntry(sBag.wmap);
	while(tm != NULL){
     PToken tok = getBagKey(tm);
     if (bcontains(tBag,tok)) {
			sim += getFloat(*getPeso(sBag,tok)) * getFloat(*getPeso(tBag,tok));
	  }
     else{
			/* find best matching token */
			double matchScore = limiar;
			PToken matchTok = NULL;

			PItemtm tm2 = firstEntry(tBag.wmap);
			while(tm2 != NULL){
            //PItemtm tm2 = (PItemtm)getData(j,*al2);
            PToken tokj = getBagKey(tm2);
				double distItoJ = func( *tok, *tokj);
				if (distItoJ>=matchScore) {
					matchTok = tokj;
					matchScore = distItoJ;
				}
				tm2 = successor(tm2);
			}
			if (matchTok!=NULL) {
            double pesos = getFloat(*getPeso(sBag,tok));
            double pesot = getFloat(*getPeso(tBag,matchTok));
            sim +=  (pesos) * (pesot)* matchScore;
            //sim += getFloat(*getPeso(sBag,tok)) * getFloat(*getPeso(tBag,matchTok))* matchScore;
			}
	  }
     tm = successor(tm);
	}	
	return (sim>1?1:sim);
}

/** */
double soft_score2(Bag sBag, Bag tBag, double(*func)(Token a, Token b)){
	func = (!func)?def_token_distance:func;
	double limiar = DEF_LIMIAR;//(limiar<=0 || limiar>1)?DEF_LIMIAR:limiar;

   double sim = 0.0;

   PItemtm tm = firstEntry(sBag.wmap);
	while(tm != NULL){
     PToken tok = getBagKey(tm);
     if (bcontains(tBag,tok)) {
			sim += getPesoN(sBag,tok) * getPesoN(tBag,tok);
	  }
     else{
			/* find best matching token */
			double matchScore = limiar;
			PToken matchTok = NULL;

			PItemtm tm2 = firstEntry(tBag.wmap);
			while(tm2 != NULL){
            //PItemtm tm2 = (PItemtm)getData(j,*al2);
            PToken tokj = getBagKey(tm2);
				double distItoJ = func( *tok, *tokj);
				if (distItoJ>=matchScore) {
					matchTok = tokj;
					matchScore = distItoJ;
				}
				tm2 = successor(tm2);
			}
			if (matchTok!=NULL) {
            double pesos = getPesoN(sBag,tok);
            double pesot = getPesoN(tBag,matchTok);
            sim +=  (pesos) * (pesot)* matchScore;
            //sim += getFloat(*getPeso(sBag,tok)) * getFloat(*getPeso(tBag,matchTok))* matchScore;
			}
	  }
     tm = successor(tm);
	}
	return (sim>1?1:sim);
}
