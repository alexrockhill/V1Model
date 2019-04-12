struct v1n{
    int x;
    int y;
    float ori;
    float v;
};
struct hc{
	struct v1n *ns;
};
struct network{
	int dim;
	int oris;
	struct hc **cols;
};
extern struct network make_network();
extern void take_down_network();