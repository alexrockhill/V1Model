// Alex Rockhill 3.10.2019

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plot.h" 
#include "network.h"
#include "random.h"

struct network_args parse_args() {
	struct network_args nw_args;
	FILE *fp = fopen("./bin/args.txt","rb");
	char args[SLEN];
	while (fgets(args, SLEN, fp) != NULL) {
		char *flag = strtok(args, " ");
		char *ptr = strtok(NULL, " \n");
		if (strcmp(flag, "--seed") == 0) {
			nw_args.seed = atoi(ptr);
		} else if (strcmp(flag, "--dim") == 0){
			nw_args.dim = atoi(ptr);
		} else if (strcmp(flag, "--oris") == 0) {
			nw_args.oris = atoi(ptr);} 
		else if (strcmp(flag, "--n_steps") == 0) {
			nw_args.n_steps = atoi(ptr);
		} else if (strcmp(flag, "--loc_mu") == 0) {
			nw_args.loc_mu = strtof(ptr, NULL);
		} else if (strcmp(flag, "--lat_mu") == 0) {
			nw_args.lat_mu = strtof(ptr, NULL);
		} else if (strcmp(flag, "--loc_sig") == 0) {
			nw_args.loc_sig = strtof(ptr, NULL);
		} else if (strcmp(flag, "--lat_sig") == 0) {
			nw_args.lat_sig = strtof(ptr, NULL);
		} else if (strcmp(flag, "--leak") == 0) {
			nw_args.leak = strtof(ptr, NULL);
		} else if (strcmp(flag, "--loc") == 0) {
			strcpy(nw_args.loc, ptr);
		} else if (strcmp(flag, "--lat") == 0) {
			strcpy(nw_args.lat, ptr);
		}
	}
	return nw_args;
}
int main()
{
	struct network_args nw_args = parse_args();
	struct network nw = make_network(nw_args);
	plot_network(nw);
	take_down_network(nw);
}