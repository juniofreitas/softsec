#include <stdlib.h>
#include <string.h>

static double gapCost = 1.0;
static char tipoCharMatch = 0;

double leven_compute(PMat m,int i,int j);
double leven_score(Token s,Token t, char ptipoCharMatch, double pgapCost);
double leven_score_norm(Token s,Token t, char ptipoCharMatch, double pgapCost);
/******************************************************************************/

/** */
double leven_compute(PMat m,int i,int j) {
   if (i==0) return -j*gapCost;
   if (j==0) return -i*gapCost;
   return max3( getMat(m,i-1,j-1) + ((tipoCharMatch==0)?matchScore01( sAt(*m,i), tAt(*m,j) ):matchScore21( sAt(*m,i), tAt(*m,j) )),
					 getMat(m,i-1, j) - gapCost,
					 getMat(m,i, j-1) - gapCost);
}

/** */
double leven_score(Token s,Token t, char ptipoCharMatch, double pgapCost){
    tipoCharMatch = ptipoCharMatch;
    gapCost = pgapCost;
    PMat mat = nova_mat( s, t, leven_compute );
    double val = getMat(mat,getSizeT(s),getSizeT(t));
    freeMat(mat);
    return abs(val);
}

/** */
double leven_score_norm(Token s,Token t, char ptipoCharMatch, double pgapCost){
    tipoCharMatch = ptipoCharMatch;
    gapCost = pgapCost;
    double maxlen = (getSizeT(s)>getSizeT(t))?getSizeT(s):getSizeT(t);
    if(maxlen == 0){
		 return 0.0;
	 }
	 else{
    	PMat mat = nova_mat( s, t, leven_compute );
    	double val = abs(getMat(mat,getSizeT(s),getSizeT(t)));
    	freeMat(mat);
    	printf("\nmaxlen=%f",maxlen);
    	return (1.0 - (val/maxlen));
	 }
}
