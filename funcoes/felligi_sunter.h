/* CLASSE QUE IMPLEMENTA A SIMILARIDADE DE FELLIGISUNTER
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const double mismatchFactor = 0.5;

PBag felligisunter_prepare(VetHash vet, HashMap hashtok, PBag bag);
double felligisunter_score(Bag sBag,Bag tBag);
/******************************************************************************/

/** Preprocess a string by finding tokens and giving them appropriate weights */
PBag felligisunter_prepare(VetHash vet, HashMap hashtok, PBag bag){

   // reweight by -log( freq/collectionSize )
	double normalizer = 0.0;
	
   PItemtm tm = firstEntry(bag->wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
		if (vet.qtdcolecao>0) {
			PLong pid = (PLong)hget(hashtok,tok);
			PInfoHash inf = getVetHash(vet,pid);
         double dfr = inf==NULL ? 1.0 : inf->tamlista;
         double w = -log( dfr/vet.qtdcolecao );
         setPeso(bag,tok,novo_float(w));
			/*Integer dfInteger = (Integer)documentFrequency.get(tok);
			double df = dfInteger==null ? 1.0 : dfInteger.intValue();
			double w = -Math.log( df/collectionSize );
			*/
		} else {
         setPeso(bag,tok,novo_float(1.0));
		}
      tm = successor(tm);
   }

	return bag;
}

/** */
double felligisunter_score(Bag sBag,Bag tBag) {
	double sim = 0.0;
	
   PItemtm tm = firstEntry(sBag.wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
      if (bcontains(tBag,tok)) {
			sim += getFloat(*getPeso(tBag,tok));
		}
		else{
         sim -= getFloat(*getPeso(sBag,tok))*mismatchFactor;
      }
      tm = successor(tm);
   }

	return sim;
}
