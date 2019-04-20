#define SLEN 100
struct network_args {
	int seed, dim, oris, n_steps;
	float loc_mu, lat_mu, loc_sig,lat_sig, leak;
	char loc[SLEN], lat[SLEN];
};
struct v1n{
    int x;
    int y;
    float ori;
    float *v;
};
struct hc{
	struct v1n *ns;
};
struct network{
	struct network_args args;
	struct hc **cols;
};
extern struct network make_network();
extern void take_down_network();