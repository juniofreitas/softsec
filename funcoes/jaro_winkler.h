#include <stdlib.h>
#include <string.h>

const unsigned short JARO = 1;
const unsigned short WINKLER = 2;
const unsigned short PREFIX = 4;

double jaro_winkler(Token s, Token t);
double jaro_score(Token s, Token t);
double winkler_score(Token s, Token t);
int halfLengthOfShorter(Token str1, Token str2);
PToken commonChars(Token s, Token t, int halflen);
int transpositions(Token common1, Token common2);
int commonPrefixLength(int maxLength,Token common1,Token common2);

/******************************************************************************/

/*
 * */
double jaro_winkler(Token s,Token t){
	double dist = jaro_score(s,t);

	if (dist<0 || dist>1){ 
		printf("\nJaro so gera scores entre 0 e 1\n"); 
		exit(0);
   }	

	double prefLength = winkler_score(s,t);
	dist = dist + prefLength*0.1 * (1 - dist);
	return dist; 
}
 
/*
 * */
double jaro_score(Token s, Token t){
   PToken str1 = &s;
   PToken str2 = &t;
      
   int halflen = halfLengthOfShorter(*str1,*str2);
   
   PToken common1 = commonChars(*str1,*str2,halflen);
   PToken common2 = commonChars(*str2,*str1,halflen);
   
   if(getSizeT(*common1)!=getSizeT(*common2)){
      freeT(common1);
      freeT(common2);
      return 0;
   }
   if(getSizeT(*common1)==0 || getSizeT(*common2)==0){
      freeT(common1);
      freeT(common2);
      return 0;
   }

   int transpos = transpositions(*common1,*common2);
   
   unsigned int sc1 = getSizeT(*common1);
   unsigned int sc2 = getSizeT(*common2);
   unsigned int ss1 = getSizeT(*str1);
   unsigned int ss2 = getSizeT(*str2);

   freeT(common1);
   freeT(common2);

   return ( sc1/((double)ss1) +
            sc2/((double)ss2) +
            (sc1-transpos)/((double)sc1))/3.0;
}

/*
 * */
double winkler_score(Token s, Token t){
    double vcommon = commonPrefixLength(PREFIX,s,t);
    return vcommon;
}

/*
 *
 */
int halfLengthOfShorter(Token str1, Token str2){
    return (getSizeT(str1)>getSizeT(str2))? getSizeT(str1)/2 +1:getSizeT(str2)/2 +1;
}

/*
 *
 */
PToken commonChars(Token s, Token t, int halflen){
   PToken common = novo_token("");
   PToken copy = toCopy(t);
   register int i,j;
   char ch; 
   PToken aux;
   
   for(i=0; i<getSizeT(s);i++){
       ch = getValorT(s)[i];
       short foundIt = 0;
       for(j=max(0,i-halflen);!foundIt && j<min(i+halflen,getSizeT(t));j++){
            if(getValorT(*copy)[j]==ch){
                foundIt = 1;
                char* sch = toChar(ch);
                aux = novo_token(sch);
                free(sch);
                concat(common,aux);
                getValorT(*copy)[j] = '*';
                freeT(aux);
                aux = NULL;
            }
       }     
   }
   freeT(copy);
   return common;   
}

/*
 *
 */
int transpositions(Token common1, Token common2){
    int transpos = 0;
    register unsigned int i;
    
    for(i=0; i<getSizeT(common1); i++){
        if(getValorT(common1)[i]!= getValorT(common2)[i]){
           transpos++;                          
        }     
    }
    transpos /= 2;
    return transpos;
}

/*
 *
 */ 
int commonPrefixLength(int maxLength,Token common1,Token common2){
   register unsigned int i;  
	int n = min(maxLength, min(getSizeT(common1), getSizeT(common2)) );
	for (i=0; i<n; i++) {
		if (getValorT(common1)[i]!= getValorT(common2)[i]) return i;
	}
	return n; /* first n characters are the same */
}
