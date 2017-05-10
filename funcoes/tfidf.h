/* CLASSE QUE IMPLEMENTA A SIMILARIDADE DO COSSENO - COM IDF
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

PBag tfidf_prepare(VetHash vet, HashMap hashtok,PBag bag);
PBag tfidf_prepare2(VetHash vet, HashMap hashtok, PBag bag);
double tfidf_score(Bag sBag,Bag tBag);
double tfidf_score2(Bag sBag,Bag tBag);
PTreeMap busca_vetorial(Token c,VetHash vet,HashMap hashtok,FILE *flinv,Par dado_func);
/******************************************************************************/

/** Preprocess a string by finding tokens and giving them TFIDF weights */
PBag tfidf_prepare(VetHash vet, HashMap hashtok,PBag bag){
	/* reweight by tdfidf */
   double normalizer = 0.0;

   if(!bag)return;

   PItemtm tm = firstEntry(bag->wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
		if(vet.qtdcolecao>0){
			PLong pid = (PLong)hget(hashtok,tok);
			PInfoHash inf = getVetHash(vet,pid);
			// set previously unknown words to df==1, which gives them a high value
			double dfr = inf==NULL ? 1.0 : inf->tamlista;
			double w = log( getFloat(*getPeso(*bag,tok)) + 1) * log( vet.qtdcolecao/dfr );
			PFloat peso = getPeso(*bag,tok);
         setFloat(peso,w);
			//setPeso( bag,tok, novo_float(w)/*peso*/ );
			normalizer += w*w;
		} else {
			setPeso(bag, tok, novo_float(1.0) );
			normalizer += 1.0;
		}
      tm = successor(tm);
   }
	setNorma(bag,normalizer);

	return bag;
}

/** Preprocess a string by finding tokens and giving them TFIDF weights */
PBag tfidf_prepare2(VetHash vet, HashMap hashtok,PBag bag){
	/* reweight by tdfidf */
   double normalizer = 0.0;

   if(!bag)return;

   PItemtm tm = firstEntry(bag->wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
		if(vet.qtdcolecao>0){
			PLong pid = (PLong)hget(hashtok,tok);
			PInfoHash inf = getVetHash(vet,pid);
			// set previously unknown words to df==1, which gives them a high value
			double dfr = inf==NULL ? 1.0 : inf->tamlista;
			double w = log( getFloat(*getPeso(*bag,tok)) + 1) * log( vet.qtdcolecao/dfr );
			PFloat peso = getPeso(*bag,tok);
         setFloat(peso,w);
			//setPeso( bag,tok, novo_float(w)/*peso*/ );
			normalizer += w*w;
		} else {
			setPeso(bag, tok, novo_float(1.0) );
			normalizer += 1.0;
		}
      tm = successor(tm);
   }
	normalizer = sqrt(normalizer);
	setNorma(bag,normalizer);
   tm = firstEntry(bag->wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
      PFloat peso = getPeso(*bag,tok);
      setFloat(peso,getFloat(*peso)/normalizer);
		//setPeso( bag,tok, novo_float(getFloat(*getPeso(*bag,tok))/normalizer)/*peso*/ );
      tm = successor(tm);
   }
	return bag;
}

/* Retorna o valor da similaridade*/
double tfidf_score(Bag sBag,Bag tBag){
	double sim = 0.0;

   PItemtm tm = firstEntry(sBag.wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
      if (bcontains(tBag,tok)) {
			sim += getFloat(*getPeso(sBag,tok)) * getFloat(*getPeso(tBag,tok));
		}
      tm = successor(tm);
   }
	/*System.out.println("common="+numCommon+" |s| = "+sBag.size()+" |t| = "+tBag.size());*/
	return sim;
}

/* Retorna o valor da similaridade*/
double tfidf_score2(Bag sBag,Bag tBag){
	double sim = 0.0;

   PItemtm tm = firstEntry(sBag.wmap);
   while(tm != NULL){
      PToken tok = getBagKey(tm);
      if (bcontains(tBag,tok)) {
			sim += getPesoN(sBag,tok) * getPesoN(tBag,tok);
		}
      tm = successor(tm);
   }
	/*System.out.println("common="+numCommon+" |s| = "+sBag.size()+" |t| = "+tBag.size());*/
	return sim;
}

/** */
PTreeMap busca_vetorial(Token c,VetHash vet,HashMap hashtok,FILE *flinv,Par dado_func){
	PTokenizer tks = novo_tokenize();
	PHashMap hids = create_hashmap(def_equals,def_hash);
	PTreeMap tresp = create_treemap((void*)equals_resp_token,NULL);
	PTreeMap trank = create_treemap((void*)equals_resp,NULL);
	PBag q,d;
	Long lid;
	double sim;
	long j;
	double(* func_token)(Bag bq,Bag bd,double(*func)(Token a, Token b)) = getA(dado_func);
	double(* func_edit)(Token ta,Token td) = getB(dado_func);

	tokenize(tks,c,0,0);
	q = tfidf_prepare(vet,hashtok,novo_bag(-1,*tks)); // Para pesoN

	PItemtm tm = firstEntry(q->wmap);
   while(tm != NULL){
	   PLong pid = hget(hashtok,tm->key);
	   PToken tok = getBagKey(tm);
      PInfoHash inf = getVetHash(vet,pid);
      if(inf != NULL){
         PArrayList lbag = get_doc_bags(vet,inf->endbloco,flinv);
         for(j=0;j<size(*lbag);j++){
            d = getData(j,*lbag);
  			   setLong(&lid,(d->s)-1);
  			   if(hcontainsKey(*hids,&lid) || (d->s == q->s))
				   continue;
				else
				   hput(hids,&lid,NULL);
				// Similaridade...
				sim = func_token(*q,*d,func_edit);
				put_tm(tresp,novo_long(d->s),novo_float(sim));
            //sim = soft_score2(*q,*d,jaro_winkler,LIMIAR_PADRAO);
         }
	      free_arraylist(lbag,(void*)freeBag2);
			lbag = NULL;
      }
      tm = successor(tm);
   }
   freeBag(q);
   free_hm(hids,NULL,NULL);
   freeTokenize(tks);
	if(size_tm(*tresp)==0){
	   printf("\n***%s: %s.\n","TFIDF","NAO ENCONTRADO");
	   return NULL;
	}
   // Ordena os resultados...
	PItemtm inf = firstEntry(*tresp);
	while(inf != NULL){
	   put_tm(trank,inf->value,inf->key);
	   inf = successor(inf);
	}
	free_tm(tresp,NULL,NULL);
	return trank;
}

