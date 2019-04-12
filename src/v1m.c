// Alex Rockhill 3.10.2019

#include <stdio.h>
#include <stdlib.h>
#include "plot.h" 
#include "network.h"
#include "random.h"

int main()
{
	int seed = 11;
	int dim = 2;
	int oris = 4;
	struct network nw = make_network(dim,oris);
	for(int i=0;i<nw.dim;i++){
		for(int j=0;j<nw.dim;j++){
			for(int k=0;k<nw.oris;k++){
				printf("%i\n", nw.cols[i][j].ns[k].x);
				printf("%i\n", nw.cols[i][j].ns[k].y);
				printf("%f\n", nw.cols[i][j].ns[k].ori);
				printf("%f\n", nw.cols[i][j].ns[k].v);
			}
		}
	}
	take_down_network(nw);
}