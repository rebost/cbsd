#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#define B_SIZE         4096

char *offsetext = ".syncoff";
char *offsetdir = "/tmp/";
char *facil;
char *myname;
char *findex=NULL;


int usage(char *myname)
{
    printf("Usage: %s [-f facil -d offsetdir - %s default) asciifile\n", myname,offsetdir);
    exit(0);
}


long int getoffset(char *offsetfile)
{
long int offset=0;
FILE *fp,*fo;
char tmp[B_SIZE+sizeof(long)];
char tmp2[B_SIZE+sizeof(long)];
int i=0,n=0;

fp = fopen(offsetfile, "r");
if(fp) {
memset(tmp,0,sizeof(tmp));
fgets(tmp,B_SIZE+sizeof(long),fp);
fclose(fp);

if(strlen(tmp)==0) return 0;

for (i=0;i<strlen(tmp),n==0;i++)
if (tmp[i]==':') { tmp[i]=' '; n=i; break;}

if (n>0) {
memset(tmp2,0,sizeof(tmp2));
sscanf(tmp,"%llu",&offset);
strncpy(tmp2,tmp+n+1,strlen(tmp)-n-1); // +-1 = : symbol

fo=fopen(findex,"r");
if (fo==NULL) return 0;
fseek(fo,offset,SEEK_SET);
memset(tmp,0,sizeof(tmp));
fgets(tmp,B_SIZE,fo);
fclose(fo);

if((strlen(tmp2)!=0)&&(!strcmp(tmp,tmp2))) {offset=offset+strlen(tmp2);}
else offset=0;
}

}
else offset=0;

return offset;
}

putoffset(char *offsetfile, char *str)
{
FILE *fp;
fp = fopen(offsetfile, "w");
if(fp) {
    fputs(str,fp);
    fclose(fp);
}
return 0;
}


char *show_myportion(long int offset)
{
FILE *fp;
char line[B_SIZE];
char *lst;
long int ipos;

fp=fopen(findex,"r");
if (fp==NULL) {
printf("No such file\n");
return NULL;
}

fseek(fp, offset, SEEK_SET);

while (!feof(fp)) {
//memset(line,0,sizeof(line));
if (fgets(line, sizeof(line), fp) != NULL) printf("%s", line);
}

ipos = ftell(fp)-strlen(line);
fclose(fp);

lst=malloc(sizeof(line)+sizeof(ipos)+5);
memset(lst,0,sizeof(lst));
sprintf(lst,"%llu:%s",ipos,line);

return lst;
}


int get_myportion()
{
char *offsetfile;
int i=0;
long int ipos;
char *lst;
offsetfile = malloc(sizeof(offsetdir) + sizeof(offsetext) + sizeof(facil));
strcpy(offsetfile,offsetdir);
strcat(offsetfile,facil);
strcat(offsetfile,offsetext);
i=getoffset(offsetfile);
lst=show_myportion(i);
if(lst) putoffset(offsetfile,lst);
free(offsetfile);
}


int
main(int argc, char **argv)
{
int i;
myname = argv[0];

findex=argv[argc-1];
facil=basename(findex);

while (1)
{
        int c;
        c = getopt(argc, argv, "d:f:");
        /* Detect the end of the options. */
        if (c == -1)
            break;
        switch (c)
          {
	  case 'd':
	    offsetdir=optarg;
	    /* if user didn't put trailing / do it for them */
//	    if(offsetdir[strlen(offsetdir)] != '/') {
//	        strcat(offsetdir, "/");
//	    }
	      break;
	  case 'f':
	    facil=optarg;
	      break;

	  }
}

if (findex==myname) usage(myname);

get_myportion();

}