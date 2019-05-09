#include "diff.h"

// ========================================================================== //

pa* pa_make(char* base[], int size, int start, int stop);
pa* pa_first(char* base[], int size);
pa* pa_next(pa* p);
size_t pa_filesize(pa* p);
size_t pa_size(pa* p);
char** pa_base(pa* p);
char* pa_info(pa* p);
int   pa_equal(pa* p, pa* q);
void pa_print(pa* p, void (*fp)(const char*));
void printboth(const char* left_right);
void printleft(const char* left);
void printright(const char* right);
FILE* openfile(const char* filename, const char* openflags);
//char* yesorno(int condition);

void prereq(const char* argv[]);
void end(FILE *fin1, FILE *fin2);

void version();
void todo();

void quiet(const char* argv[]);
void loud(const char* argv[]);
int sideside(const char* argv[]);
int normal(const char* argv[]);

// ========================================================================== //

int main(int argc, const char* argv[]) {

  if (argc != 3 && (*argv)[0] == '-') { fprintf(stderr, "Usage: ./diff file1 file2\n"); exit(ARGC_ERROR); }
  if (argc < 3)                       { fprintf(stderr, "Usage: ./diff file1 file2\n"); exit(ARGC_ERROR); }
  if (argc == 3)                      { prereq(argv); normal(argv); return 0; }

  while (*++argv)
    switch ((*argv)[1]) {
      default:  break;
      case 'v': version(); return 0;         // 100%  implemented (unless I want to enhance)
      case 't': todo(); return 0;            // Only for me. To be deprecated later.
      case 'q': prereq(argv); quiet(argv); break;       // 100% implemented
      case 'y': prereq(argv); sideside(argv); break;    // 50%   implemented
      case 'i': printf("i done\n"); break;
      case 'c': printf("c done\n"); break;
      case 'u': printf("u done\n"); break;
      case 's': prereq(argv); loud(argv); break;
    }

  return 0;
}

// ====================================================== //

void prereq(const char* argv[]) {
  fin1 = openfile(argv[1], "r");
  fin2 = openfile(argv[2], "r");

  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }

  p = pa_first(strings1, count1);
  q = pa_first(strings2, count2);
}
void end(FILE* fin1, FILE* fin2) {
  fclose(fin1);
  fclose(fin2);
}

void version() {
  printf("\n     _ _  __  __                            \n");
  printf("  __| (_)/ _|/ _|      _ __ ___ _ __ ___    \n");
  printf(" / _` | | |_| |_ _____| '__/ __| '_ ` _ \\   \n");
  printf("| (_| | |  _|  _|_____| | | (__| | | | | |  \n");
  printf(" \\__,_|_|_| |_|       |_|  \\___|_| |_| |_|  \n\n");
  printf("------------------------------------------\n\n");
  printf("v0.4 alpha | Quoth the anti-Raven. Alwaysmore.\n\n");
  printf("Copyright (C) 2019 | All Rights Reserved.\n");
  printf("Any unauthorized use or re-distribution (before May 19, 2019) of this code is STRICTLY prohibited.\n\n");
  printf("Written by: Chris Nutter; William McCarthy; Rasputin\n\n");
}
void todo() {
  printf("\nTODO: check line by line in a pagraph, using '|' for differences");
  printf("\nTODO: this starter code does not yet handle printing all of fin1's pagraphs.");
  printf("\nTODO: handle the rest of diff's options");
  printf("\nTODO: fix standard printing with no parameters");
  printf("\nTODO: implement multiple types of parameters\n");
}

int normal(const char* argv[]) {

  printf("\nTHIS IS NOT NORMAL FOR NOW. THIS IS PLACEHOLDER. MMKAY.\n");
  printf("THIS IS NOT NORMAL FOR NOW. THIS IS PLACEHOLDER. MMKAY.\n\n\n");

  pa_print(p, printleft);
  pa_print(q, printright);

  printf("\nTHIS IS NOT NORMAL FOR NOW. THIS IS PLACEHOLDER. MMKAY.\n");
  printf("THIS IS NOT NORMAL FOR NOW. THIS IS PLACEHOLDER. MMKAY.\n\n\n");

  return 0;
}
int sideside(const char* argv[]) {

  if (pa_equal(p, q)) { pa_print(p, printboth); return 0; }
  pa_print(q, printright);
  while ((q = pa_next(q)) != NULL && !equal) { equal = pa_equal(p, q); pa_print(q, equal ? printboth : printright); }
  pa_print((p = pa_next(p)), printleft);

  return 0;
}
void quiet(const char* argv[]) { if (pa_equal(p, q) == 0) { printf("The files are not equal.\n"); } }
void loud(const char* argv[]) { if (pa_equal(p, q) == 1) { printf("The files are equal.\n"); } }

int hash(const char* s) {
    unsigned long code = 0;
    while (*s != '\0') { code = code *31 + *s++; }
    return code % HASHLEN;
}

// ====================================================== //

pa* pa_make(char* base[], int filesize, int start, int stop) {
  pa* p = (pa*) malloc(sizeof(pa));
  p->base = base;
  p->filesize = filesize;
  p->start = start;
  p->stop = stop;

  return p;
}
pa* pa_first(char* base[], int size) {
  pa* p = pa_make(base, size, 0, -1);
  return pa_next(p);
}
pa* pa_next(pa* p) {
  if (p->stop == p->filesize) { return NULL; }

  int i;
  pa* pnew = pa_make(p->base, p->filesize, p->stop + 1, p->stop + 1);
  for (i = pnew->start; i < p->filesize && strcmp(p->base[i], "\n") != 0; ++i) { }
  pnew->stop = i;

  return pnew;
}
void pa_print(pa* p, void (*fp)(const char*)) {
  if (p == NULL) { return; }
  for (int i = p->start; i <= p->stop && i != p->filesize; ++i) { fp(p->base[i]); }
}
void pa_destroy(pa* p) { free(p); }
void printleft(const char* left) {
  char buf[BUFLEN];

  strcpy(buf, left);
  int j = 0, len = (int)strlen(buf) - 1;
  for (j = 0; j <= 48 - len ; ++j) { buf[len + j] = ' '; }
  buf[len + j++] = '<';
  buf[len + j++] = '\0';
  printf("%s\n", buf);
}
void printright(const char* right) {
  if (right == NULL) { return; }
  printf("%50s %s", ">", right);
}
void printboth(const char* left_right) {
  char buf[BUFLEN];
  size_t len = strlen(left_right);
  if (len > 0) { strncpy(buf, left_right, len); }
  buf[len - 1] = '\0';
  printf("%-50s %s", buf, left_right);
}
size_t pa_filesize(pa* p) { return p == NULL ? 0 : p->filesize; }
size_t pa_size(pa* p) { return p == NULL || p->stop < p->start ? 0 : p->stop - p->start + 1; }
char** pa_base(pa* p) { return p->base; }
//char* yesorno(int condition) { return condition == 0 ? "no" : "YES"; }
char* pa_info(pa* p) {
  static char buf[BUFLEN];   // static for a reason
  snprintf(buf, sizeof(buf), "base: %p, filesize: %d, start: %d, stop: %d\n",
    p->base, p->filesize, p->start, p->stop);
  return buf;  // buf MUST be static
}
int pa_equal(pa* p, pa* q) {
  if (p == NULL || q == NULL) { return 0; }
  if (pa_size(p) != pa_size(q)) { return 0; }
  int i = p->start, j = q->start, equal = 0;
  while ((equal = strcmp(p->base[i], q->base[i])) == 0) { ++i; ++j; }
  return equal;
}
FILE* openfile(const char* filename, const char* openflags) {
  FILE* f;
  if ((f = fopen(filename, openflags)) == NULL) {  printf("can't open '%s'\n", filename);  exit(1); }
  return f;
}

// ============================ LEFTOVERS ============================ //

// if ((*argv)[0] == '-')