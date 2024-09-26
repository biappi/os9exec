/******************************************************************* *                                                                 * *       Dieses Programm ist urheberrechtlich geschuetzt!!!        * *                                                                 * *  Copyrights (c) 1987 by recc-O-ware, Wolfgang Ocker, Puchheim   * *                                                                 * ******************************************************************* ******************************************************************* * Editon History  hdpark.c * *  #    Date      Comments                                      By * -- --------- ------------------------------------------------ --- * 00  11/08/88 Fixed for release.                               weo * */#include <stdio.h>#include <modes.h>#define TRUE  1#define FALSE 0extern int errno;/* * m a i n */main(argc, argv)  int  argc;  char *argv[];{  int  i, j;  int  devnum;  char drive[200];  int  fd;  int  ask;  char buf[5];  devnum  = 0;  ask     = TRUE;  /*   * Parsing options   */  for (i = 1; i < argc; i++)    if (argv[i][0] == '-')      for (j = 1; j < strlen(argv[i]); j++)        switch (argv[i][j]) {          case '?':            usage();            exit(1);          case 'r':            ask = FALSE;            break;          default:            usage();            exit(_errmsg(1, "unknown option '%c'\n", argv[i][j]));        }    else      devnum++;  if (devnum == 0) {      /* Devices given? */    usage();    exit(_errmsg(1, "no drives specified\n"));  }  if (ask) {    printf("Are you sure (y/n)? ");    fflush(stdout);    readln(0, buf, 2);    if (buf[0] != 'y' && buf[0] != 'Y')      exit(1);  }  /*   * Walk through arguments again   */  for (i = 1; i < argc; i++) {    if (argv[i][0] == '-')      /* Option? */      continue;    /*     * Get drive name. Add a leading slash if neccessary     */    sprintf(drive, "%s%s@", argv[i][0] == '/' ? "" : "/", argv[i]);    if ((fd = open(drive, S_IREAD)) < 0) {      _errmsg(errno, "can't open '%s'\n", argv[i]);      continue;    }    if (_ss_sqd(fd) < 0)        _errmsg(errno, "can't park '%s'\n", argv[i]);    close(fd);  }}/* * u s a g e */usage(){  fputs("Syntax: hdpark <drivename> {<opts>}\n", stderr);  fputs("Function: park hard disk\n", stderr);  fputs("Options:\n", stderr);  fputs("     -r    ready, don't ask\n", stderr);}