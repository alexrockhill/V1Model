#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "random.h"
#include "network.h"

float logistic(float x) {
	return 2.0/(1 + exp(-x)) - 1;
}

float gaussian(float x, float sigma) {
	return exp(-((pow(x, 2) / (2 * pow(sigma, 2)))));
}

float mexican_hat(float x, float sigma) {
	return (1-pow((x) / sigma, 2)) * exp(-((pow(x, 2) / (2 * pow(sigma, 2))))); //(2/(pow(3 * sigma, 0.5) * pow(M_PI, 0.25)))*
}

void update_network(nw)
	struct network nw;
{	
	struct v1n n;
	float a;
	float ori;
	float delta_ori;
	float delta_loc;
	float (*loc_f)(float, float);
	float (*lat_f)(float, float);
	if (strcmp(nw.args.loc,"gaussian") == 0) {
		loc_f = &gaussian; 
	} else if (strcmp(nw.args.loc,"mexican_hat") == 0) {
		loc_f = &mexican_hat; 
	} else {
		fprintf(stderr, "loc %s function not recognized\n", nw.args.loc);
	}
	if (strcmp(nw.args.lat,"gaussian")==0) {
		lat_f = &gaussian; 
	} else if (strcmp(nw.args.lat,"mexican_hat") == 0) {
		lat_f = &mexican_hat; 
	} else {
		fprintf(stderr, "lat %s function not recognized\n", nw.args.lat);
	}
	printf("Updating network over %i time steps: \n", nw.args.n_steps);
	for (int t_ind=1; t_ind < nw.args.n_steps; t_ind++) {
		printf("%i\n", t_ind);
		float loc_matrix[nw.args.dim][nw.args.dim][nw.args.oris];
		float lat_matrix[nw.args.dim][nw.args.dim][nw.args.oris];
		for (int i=0; i < nw.args.dim; i++) {
	  	    for (int j=0; j < nw.args.dim; j++) {
	  		    for (int k=0; k < nw.args.oris; k++) {
	  		    	loc_matrix[i][j][k] = 0;
	  		    	lat_matrix[i][j][k] = 0;
	  		    }
	  		}
	  	}
		for (int i=0; i < nw.args.dim; i++) {
	  	    for (int j=0; j < nw.args.dim; j++) {
	  		    for (int k=0; k < nw.args.oris; k++) {
	  		    	n = nw.cols[i][j].ns[k];
	  		    	for (int k1=0; k1 < nw.args.oris; k1++) {
	  		    		if (k != k1) {  // Local connections only within hypercolumn (isotropic)
			  		    	a = nw.cols[i][j].ns[k1].v[t_ind-1];
			  		    	ori = nw.cols[i][j].ns[k1].ori;
			  		    	delta_ori = fminf(fabsf(n.ori - ori), fabsf(n.ori - (ori - 180)));
	  		    			loc_matrix[i][j][k] += (*loc_f)(delta_ori, nw.args.loc_sig);
	  		    		}
	  		    		for (int i1=0; i1 < nw.args.dim; i1++) {  // Lateral connections only between at given orientation (anisotropic)
		  		    		for (int j1=0; j1 < nw.args.dim; j1++) {
		  		    			float angle = (180.0/M_PI)*atan2(i - i1, j - j1);
		  		    			if (angle < 0) {
		  		    				angle += 180;
		  		    			}
		  		    			if (((i != i1) || (j != j1)) && fabsf(angle - n.ori) < (45.0/(float)nw.args.oris)) {
		  		    				delta_loc = pow((pow(i - i1, 2) + pow(j - j1, 2)), 0.5);
		  		    				a = nw.cols[i1][j1].ns[k1].v[t_ind-1];
		  		    				lat_matrix[i][j][k] += (*lat_f)(delta_loc, nw.args.lat_sig) * logistic(a);
		  		    			}
		  		    		}
		  		    	}
		  		    }
	  		    }
	  		}
	  	}
	  	for (int i=0; i < nw.args.dim; i++){
	  	    for (int j=0; j < nw.args.dim; j++){
	  		    for (int k=0; k < nw.args.oris; k++){
	  		    	float delta_v = nw.args.lat_mu*lat_matrix[i][j][k] + nw.args.loc_mu*loc_matrix[i][j][k];
	  		    	//printf("%.2f\n", delta_v);
	  		    	if (delta_v == delta_v) {  // is not nan
	  		    		nw.cols[i][j].ns[k].v[t_ind] = (nw.cols[i][j].ns[k].v[t_ind - 1] + delta_v) * nw.args.leak;  //logistic(nw.cols[i][j].ns[k].v[t_ind] + delta_v);
	  		    	}
	  		    	//printf("%.2f\n", nw.cols[i][j].ns[k].v[t_ind]);
	  		    }
	  		}
	  	}
	}
}

struct network make_network(nw_args)
    struct network_args nw_args;
{
    struct hc **cols;
    struct network nw;
    nw.args = nw_args;
    cols = malloc(nw.args.dim * sizeof(struct hc *));
    for (int i=0;i < nw.args.dim; i++){
    	cols[i] = malloc(nw.args.dim * sizeof(struct hc));
  	    for (int j=0; j < nw.args.dim; j++){
  	    	cols[i][j].ns = malloc(nw.args.oris * sizeof(struct v1n));
  		    for (int k=0; k < nw.args.oris; k++){
  		 	    cols[i][j].ns[k].x = i;
				cols[i][j].ns[k].y = j;
				cols[i][j].ns[k].ori = (180.0/(float)nw.args.oris)*k;
				cols[i][j].ns[k].v = malloc(nw.args.n_steps * sizeof(float));
  		    	cols[i][j].ns[k].v[0] = 100*(my_rand(&nw.args.seed)-0.5);
  		    	for (int t_ind=1; t_ind < nw.args.n_steps; t_ind++) {
					cols[i][j].ns[k].v[t_ind] = 0;
  		    	}
  		    }
  	    }
    }
    nw.cols=cols;
    //nw.cols[(int)nw.args.dim/2][(int)nw.args.dim/2].ns[(int)nw.args.oris/2].v[0] = 10;
    update_network(nw);
    return nw;
}

void take_down_network(nw)
	struct network nw;
{
	for (int i=0; i < nw.args.dim; i++) {
		for (int j=0; j < nw.args.dim; j++) {
			for (int k=0; k < nw.args.oris; k++) {
				free(nw.cols[i][j].ns[k].v);
			}
			free(nw.cols[i][j].ns);
		}
	    free(nw.cols[i]);
	}
	free(nw.cols);
}