class RBFile
{
public:
	RBFile(char *name,int flag);
	RBFile(void);
	~RBFile();


	void open(char *name,int flag);
	void close(void);

	int read(void *,int);
	int write(void *,int);
	void seek(int pos,int from);
	int tell(void);
	int length;
	int isopen;

protected:
	char filename[256];
	int pos;
	FILE *fic;
};

#define RBTEXT 1
#define RBBINARY 0

#define RBREAD 2
#define RBWRITE 4
