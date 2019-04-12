#include <stdio.h>
#include <stdlib.h>
#include "random.h"
#include "network.h"

struct network make_network(dim,oris,seed)
    int dim,oris,seed;
{
    struct hc **cols;
    struct network nw;
    nw.dim = dim;
    nw.oris = oris;
    cols = malloc(dim * sizeof(struct hc *));
    for(int i=0;i<dim;i++){
    	cols[i] = malloc(dim * sizeof(struct hc));
  	    for(int j=0;j<dim;j++){
  	    	cols[i][j].ns = malloc(oris * sizeof(struct v1n));
  		    for(int k=0;k<oris;k++){
  		 	    cols[i][j].ns[k].x = i;
				cols[i][j].ns[k].y = j;
				cols[i][j].ns[k].ori = (float) (360/oris)*k;
				cols[i][j].ns[k].v = my_rand(&seed) - 0.5;
  		    }
  	    }
    }
    nw.cols=cols;
    return nw;
}

void take_down_network(nw)
	struct network nw;
{
	for(int i=0;i<nw.dim;i++) {
		for(int j=0;j<nw.dim;j++) {
			free(nw.cols[i][j].ns);
		}
	    free(nw.cols[i]);
	}
	free(nw.cols);
}