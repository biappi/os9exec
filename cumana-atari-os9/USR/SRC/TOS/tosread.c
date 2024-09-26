/* * Read (copy) a MSDOS file to OS-9 *  * Emmet P. Gray			US Army, HQ III Corps & Fort Hood * ...!ihnp4!uiucuxc!fthood!egray	Attn: AFZF-DE-ENV Directorate of * Engineering & Housing Environmental Management Office Fort Hood, TX * 76544-5057 ported to OS-9 by Michael Baehr command line option processing * modified by Wolfgang Ocker */#include <stdio.h>#include <types.h>#include <modes.h>#include "msdos.h"extern int    errno;int           path;int           fd;				  /* the file descriptor for						   * the floppy */int           dir_start;			  /* starting sector for						   * directory */int           dir_len;				  /* length of directory (in						   * sectors) */int           dir_entries;			  /* number of directory						   * entries */int           dir_chain[25];			  /* chain of sectors in						   * directory */int           clus_size;			  /* cluster size (in						   * sectors) */int           fat_len;				  /* length of FAT table (in						   * sectors) */int           num_clus;				  /* number of available						   * clusters */unsigned char *fatbuf;				  /* the File Allocation						   * Table */char          *mcwd;				  /* the Current Working						   * Directory */long          size;long          current;int           textmode = 0;int           nowarn = 0;main(argc, argv)  int  argc;  char *argv[];{  int              fat, i, j, ismatch, entry, subdir(), single, c, fargn;  char             *filename, *newfile, text[4], tname[9], *getname(), *unixname();  char             *strncpy(), *pathname, *getpath(), *target, *tmp, *malloc();  char             *strcat(), *strcpy();  void             perror(), exit();  struct directory *dir, *search();  target   = NULL;  textmode = 1;  fargn    = 0;  /* get command line options */  for (i = 1; i < argc; i++)    if (argv[i][0] == '-')      for (j = 1; j < strlen(argv[i]); j++)	switch (argv[i][j]) {	  case '?':	    usage();	    exit(1);	  case 'b':	    textmode = 0;	    break;	  case 'n':	    nowarn = 1;	    break;	  case 'w':	    target = argv[i] +	      (argv[i][2] == '=' ? 3 : 2);	    j = strlen(argv[i]);	    break;	  default:	    usage();	    exit(_errmsg(1, "unknown option '%c'\n", argv[i][j]));	}    else      fargn++;  if (init(_READ)) {    fprintf(stderr, "tosread: Cannot initialize disk\n");    exit(1);  }  if (target)    if ((path = open(target, S_IFDIR)) != -1) {      single = 0;      close(path);    }    else      exit(_errmsg(errno, "can't find '%s' directory\n",		   target));  else {    if (fargn > 2) {      usage();      exit(_errmsg(1, "Too many files\n"));    }    for (i = 1; i < argc; i++)      if (argv[i][0] != '-')	if (index(argv[i], '?') ||	    index(argv[i], '*')) {	  usage();	  exit(_errmsg(1, "Too many files\n"));	}    target = NULL;    if (fargn == 2)      for (i = argc - 1; i > 0; i--)	if (argv[i][0] != '-') {	  target = argv[i];	  break;	}    single = 1;  }  for (i = 1; i < argc; i++) {    if (argv[i][0] == '-')			  /* skip options */      continue;    filename = getname(argv[i]);    pathname = getpath(argv[i]);    if (subdir(pathname))      continue;    ismatch = 0;    for (entry = 0; entry < dir_entries; entry++) {      dir = search(entry);      /* if empty */      if (dir->name[0] == NULL)	break;      /* if erased */      if (dir->name[0] == 0xe5)	continue;      /* if dir or volume lable */      if ((dir->attr & 0x10) || (dir->attr & 0x08))	continue;      strncpy(tname, dir->name, 8);      strncpy(text, dir->ext, 3);      newfile = unixname(tname, text);      fat     = dir->start[1] * 0x100 + dir->start[0];      size    = dir->size[2] * 0x10000 + dir->size[1] * 0x100 + dir->size[0];      /* if single file */      if (single) {	if (!strcmp(newfile, filename)) {	  if (target) {	    target = getname(target);	    readit(fat, target, newfile);	  }	  else	    readit(fat, argv[i], newfile);	  ismatch = 1;	  break;	}      }      /* if multiple files */      else {	if (match(newfile, filename)) {	  printf("Copying %s\n", newfile);	  tmp = malloc(strlen(target) + 1 + strlen(newfile) + 1);	  strcpy(tmp, target);	  strcat(tmp, "/");	  strcat(tmp, newfile);	  readit(fat, tmp);	  ismatch = 1;	}      }    }    if (!ismatch) {      fprintf(stderr, "tosread: File '%s' not found\n", filename);      continue;    }    if (single)      break;  }  close(fd);  exit(0);}readit(fat, target)  int  fat;  char *target;{  char ans[10];  int  i;  void exit();  FILE *fp;  current = 0L;  if (!nowarn) {    if (!access(target, 0)) {      while (1) {	printf("File '%s' exists, overwrite (y/n) ? ", target);	gets(ans);	if (ans[0] == 'n' || ans[0] == 'N')	  return(0);	if (ans[0] == 'y' || ans[0] == 'Y')	  break;      }    }  }  for (i = 0; i < strlen(target); target[i] = tolower(target[i]), i++);  if (!(fp = fopen(target, "w"))) {    fprintf(stderr, "tosread: Can't open '%s' for write\n", target);    return(0);  }  while (1) {    getcluster(fat, fp);    /* get next cluster number */    fat = getfat(fat);    if (fat == -1) {      fprintf(stderr, "tosread: FAT problem\n");      exit(1);    }    /* end of cluster chain */    if (fat >= 0xff8)      break;  }  fclose(fp);  return(0);}getcluster(num, fp)				  /* read a cluster */  int  num;  FILE *fp;{  int  i, buflen, start;  void exit(), perror();  char buf[1024];  start = (num - 2) * clus_size + dir_start + dir_len;  move(start);  buflen = clus_size * MSECSIZ;  if (read(fd, buf, buflen) != buflen) {    perror("getcluster: read");    exit(1);  }    /* stop at size not EOF marker */  for (i = 0; i < buflen; i++) {    current++;    if (current > size)      break;    if (textmode && buf[i] == '\l')      continue;    if (textmode && buf[i] == '\x1a')		  /* EOF */      current = size;    else      putc(buf[i], fp);  }  return(0);}usage(){  fprintf(stderr, "Syntax: tosread [<opts>] <tosfile> [<os9file>]\n");  fprintf(stderr, "Function: copy TOS files to OS-9\n");  fprintf(stderr, "Options:\n");  fprintf(stderr, "     -b        binary file transfer (no cr/lf -> cr mapping)\n");  fprintf(stderr, "     -n        overwrite without warning\n");  fprintf(stderr, "     -w=<dir name> wild card copy to <dir name>\n");  fprintf(stderr, "Environment:\n");  fprintf(stderr, "     TCWD      current TOS subdirectory\n");  fprintf(stderr, "     TDRV      drive number\n");}