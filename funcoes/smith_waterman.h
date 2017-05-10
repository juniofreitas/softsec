#include <stdlib.h>
#include <string.h>

static char charMatchScore;
static double gapCost;

double smithwaterman_compute(PMat m,int i,int j);
double smithwaterman_score(Token s,Token t, char ptipoCharMatch, double pgapCost);
/******************************************************************************/
double smithwaterman_compute(PMat m,int i,int j) {
   if (i==0) return 0;
   if (j==0) return 0;
   return max4(0.0,
               getMat(m,i-1,j-1) + ((charMatchScore==0)?matchScore01( sAt(*m,i), tAt(*m,j) ):matchScore21( sAt(*m,i), tAt(*m,j) )),
  				   getMat(m,i-1, j) - gapCost,
					getMat(m,i, j-1) - gapCost);
}

/** */
double smithwaterman_score(Token s,Token t, char ptipoCharMatch, double pgapCost){
    charMatchScore = ptipoCharMatch;
    gapCost = pgapCost;
    PMat mat = nova_mat( s, t, smithwaterman_compute );
    unsigned register int i,j;
	 double best = -MAX_DOUBLE;
	 for(i=0; i<=getSizeT(s); i++) {
	    for(j=0; j<=getSizeT(t); j++) {
				best = max( best, getMat(mat,i,j) );
	    }
	 }
    freeMat(mat);
    return best;
}
