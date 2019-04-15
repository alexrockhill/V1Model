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
	int dim, oris, n_steps;
	float loc_sig, lat_sig, seed;
	char *loc, *lat;
	struct hc **cols;
};
extern struct network make_network();
extern void take_down_network();