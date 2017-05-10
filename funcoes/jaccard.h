/* CLASSE QUE IMPLEMENTA A SIMILARIDADE DE JACCARD
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

PBag jaccard_prepare(char* s, Tokenizer(*ptokenize)(Token p1, unsigned short p2,unsigned short p3),short ip, short ic);
double jaccard_score(Bag sBag,Bag tBag);

/******************************************************************************/

/** Preprocess a string by finding tokens. */
PBag jaccard_prepare(char* s, Tokenizer(*ptokenize)(Token p1, unsigned short p2,unsigned short p3),short ip, short ic) {
   Tokenizer tokconsulta = ptokenize(*novo_token(s),ip,ic);
	return novo_bag(-1,tokconsulta);
}

/** */
double jaccard_score(Bag sBag,Bag tBag) {
	double numCommon = 0.0;

   PItemtm tm = firstEntry(sBag.wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
      if (bcontains(tBag,tok)) {
			numCommon++;
		}
      tm = successor(tm);
   }

	//System.out.println("common="+numCommon+" |s| = "+sBag.size()+" |t| = "+tBag.size());
	return  numCommon / (size_bag(sBag) + size_bag(tBag) - numCommon);
}
