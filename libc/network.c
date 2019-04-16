#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "random.h"
#include "network.h"


float gaussian1D(float a, float x0, float x1, float sigma) {
	return a*exp(-((pow(x1 - x0, 2) / (2 * pow(sigma, 2)))));
}

float mexican_hat1D(float a, float x0, float x1, float sigma) {
	return a*(1-pow((x1 - x0) / sigma, 2)) * exp(-((pow(x1 - x0, 2) / (2 * pow(sigma, 2))))); //(2/(pow(3 * sigma, 0.5) * pow(M_PI, 0.25)))*
}

float gaussian2D(float a, float x0, float x1, float y0, float y1, float sigma) {
	return a*exp(-(((pow(x1 - x0, 2) / (2 * pow(sigma, 2)) + (pow(y1 - y0, 2) / (2 * pow(sigma, 2)))))));
}

float mexican_hat2D(float a, float x0, float x1, float y0, float y1, float sigma) {
	return a*(1-(((pow(x1 - x0, 2) + pow(y1 - y0, 2)) / pow(sigma, 2))/2))*
		      exp(-((pow(x1 - x0, 2) + pow(y1 - y0, 2)) / (2*pow(sigma, 2)))); //(1/(M_PI*pow(sigma,2)))
}

void update_network(nw)
	struct network nw;
{	
	float (*loc_f)(float, float, float, float);
	float (*lat_f)(float, float, float, float, float, float);
	if (strcmp(nw.loc,"gaussian") == 0) {
		loc_f = &gaussian1D; 
	} else if (strcmp(nw.loc,"mexican_hat") == 0) {
		loc_f = &mexican_hat1D; 
	} else {
		fprintf(stderr, "%s function not recognized\n", nw.loc);
	}
	if (strcmp(nw.lat,"gaussian")==0) {
		lat_f = &gaussian2D; 
	} else if (strcmp(nw.loc,"mexican_hat") == 0) {
		lat_f = &mexican_hat2D; 
	} else {
		fprintf(stderr, "%s function not recognized\n", nw.lat);
	}
	//printf("Updating network over %i time steps: \n", nw.n_steps);
	for (int t_ind=1; t_ind < nw.n_steps; t_ind++) {
		//printf("%i\n", t_ind);
		float update_matrix[nw.dim][nw.dim][nw.oris];
		for (int i=0; i < nw.dim; i++) {
	  	    for (int j=0; j < nw.dim; j++) {
	  		    for (int k=0; k < nw.oris; k++) {
	  		    	float ori = nw.cols[i][j].ns[k].ori;
	  		    	for (int k1=0; k1 < nw.oris; k1++) {
	  		    		if (k == k1) {  // Lateral excitements only between shared orientations
		  		    		for (int i1=0; i1 < nw.dim; i1++){
			  		    		for (int j1=0; j1 < nw.dim; j1++){
			  		    			if (! ((i == i1) && (j == j1))){
			  		    				float v = nw.cols[i1][j1].ns[k].v[t_ind-1];
			  		    				update_matrix[i][j][k] += (*lat_f)(v, i, i1, j, j1, nw.lat_sig);
			  		    			}
			  		    		}
			  		    	}
			  		    } else {
			  		    	float v = nw.cols[i][j].ns[k1].v[t_ind-1];
			  		    	float ori1 = nw.cols[i][j].ns[k1].ori;
	  		    			update_matrix[i][j][k] += (*loc_f)(v, ori, ori1, nw.loc_sig);
	  		    		}
	  		    	}
	  		    }
	  		}
	  	}
	  	for (int i=0; i < nw.dim; i++){
	  	    for (int j=0; j < nw.dim; j++){
	  		    for (int k=0; k < nw.oris; k++){
	  		    	float delta_v = update_matrix[i][j][k]; // / (nw.dim * nw.dim * nw.oris));
	  		    	/*if (delta_v > 10 || delta_v < -10) {
	  		    		printf("%f %i %i %i\n", delta_v, i, j, k);
	  		    	}*/
	  		    	if (delta_v == delta_v) {  // is not nan
	  		    		nw.cols[i][j].ns[k].v[t_ind] += delta_v;
	  		    	}
	  		    	/*if (nw.cols[i][j].ns[k].v[t_ind] < -1) {
	  		    		nw.cols[i][j].ns[k].v[t_ind] = -1;
	  		    	} else if (nw.cols[i][j].ns[k].v[t_ind] > 1) {
	  		    		nw.cols[i][j].ns[k].v[t_ind] = 1;
	  				}*/
	  				//printf("%.2f\n", nw.cols[i][j].ns[k].v[t_ind]);
	  		    }
	  		}
	  	}
	}
}

struct network make_network(dim, oris, n_steps, loc_sig, lat_sig, loc, lat, seed)
    int dim, oris, n_steps, seed;
    float loc_sig, lat_sig;
    char *loc, *lat;
{
    struct hc **cols;
    struct network nw;
    nw.dim = dim;
    nw.oris = oris;
    nw.n_steps = n_steps;
    nw.loc_sig = loc_sig;
    nw.lat_sig = lat_sig;
    nw.loc = loc; 
    nw.lat = lat;
    nw.seed = seed;
    cols = malloc(dim * sizeof(struct hc *));
    for (int i=0;i < dim; i++){
    	cols[i] = malloc(dim * sizeof(struct hc));
  	    for (int j=0; j < dim; j++){
  	    	cols[i][j].ns = malloc(oris * sizeof(struct v1n));
  		    for (int k=0; k < oris; k++){
  		 	    cols[i][j].ns[k].x = i;
				cols[i][j].ns[k].y = j;
				cols[i][j].ns[k].ori = (float) (360/oris)*k;
				cols[i][j].ns[k].v = malloc(n_steps * sizeof(float));
  		    	cols[i][j].ns[k].v[0] = 2*(my_rand(&nw.seed)-0.5);
  		    }
  	    }
    }
    nw.cols=cols;
    update_network(nw);
    return nw;
}

void take_down_network(nw)
	struct network nw;
{
	for (int i=0; i < nw.dim; i++) {
		for (int j=0; j < nw.dim; j++) {
			for (int k=0; k < nw.oris; k++) {
				free(nw.cols[i][j].ns[k].v);
			}
			free(nw.cols[i][j].ns);
		}
	    free(nw.cols[i]);
	}
	free(nw.cols);
}