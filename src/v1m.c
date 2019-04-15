// Alex Rockhill 3.10.2019

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plot.h" 
#include "network.h"
#include "random.h"

#define SLEN 100

struct args {
	int seed, dim, oris, n_steps;
	float loc_sig,lat_sig;
	char loc[SLEN], lat[SLEN];
};

struct args parse_args() {
	struct args this_args;
	FILE *fp = fopen("./bin/args.txt","rb");
	char args[SLEN];
	while (fgets(args, SLEN, fp) != NULL) {
		char *flag = strtok(args, " ");
		char *ptr = strtok(NULL, " \n");
		if (strcmp(flag, "--seed") == 0) {
			this_args.seed = atoi(ptr);
		} else if (strcmp(flag, "--dim") == 0){
			this_args.dim = atoi(ptr);
		} else if (strcmp(flag, "--oris") == 0) {
			this_args.oris = atoi(ptr);} 
		else if (strcmp(flag, "--n_steps") == 0) {
			this_args.n_steps = atoi(ptr);
		} else if (strcmp(flag, "--loc_sig") == 0) {
			this_args.loc_sig = strtof(ptr, NULL);
		} else if (strcmp(flag, "--lat_sig") == 0) {
			this_args.lat_sig = strtof(ptr, NULL);
		} else if (strcmp(flag, "--loc") == 0) {
			strcpy(this_args.loc, ptr);
		} else if (strcmp(flag, "--lat") == 0) {
			strcpy(this_args.lat, ptr);
		}
	}
	return this_args;
}
int main()
{
	struct args this_args = parse_args();
	struct network nw = make_network(this_args.dim, this_args.oris, 
		                             this_args.n_steps, this_args.seed);
	plot_network(nw);
	take_down_network(nw);
}