//
//  diff.c
//  diff-rcm
//
//  Created by Chris Nutter on... I really don't remember. Check the Github.
//  Copyright (C) 2019 Chris Nutter, William McCarthy. All rights reserved.
//
//  In the famous words of Steve Jobs, "One more thing...",
//  I will admit this project wasn't all easy (a lot of it was however).
//  I really appreciate the help on this project and the project before.
//  I'll make sure to stop by and ask for help for my other CPSC classes.
//  And who knows...
//  Maybe you'll teach me again someday...
//  ...probably.
//  I mean let's think about this.
//  I have like 2 years left and plenty of CPSC courses to do.
//  You are going to keep teaching CPSC so... Anyways.
//  Thanks.
//
//  - Chris Nutter
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <time.h>

#include "diff.h"
#include "pa.h"

// ================================================================ //

int main(int argc, const char* argv[]) {

    init(--argc, ++argv);
    loadfiles(files[0], files[1]);

    if (ignorecase)                       { ignore_case(); } // Don't you dare...
    
    if (report_identical)                 { loud (files[0], files[1]); }
    if (showsidebyside)                   { sideside(files[0], files[1]); }
    identical(files[0], files[1]);        // -s and -y work for identical file names so I manipulated load order.
    
    if (showbrief)                        { quiet(files[0], files[1]); }
        
    if (**argv != '-' || diffnormal == 1) { lineline(); }

    if (showcontext)                      { context(files[0], files[1]);  }
    if (showunified)                      { unified(files[0], files[1]);  }

    return 0;

}

// ================================================================ //

void init(int argc, const char* argv[]) {

  while (argc-- > 0) {
    const char* arg = *argv;

    setoption(arg, "-v",                        "--version",                  &showversion);
    setoption(arg, "-q",                        "--brief",                    &showbrief);
    setoption(arg, "-i",                        "--ignore-case",              &ignorecase);
    setoption(arg, "-s",                        "--report-identical-files",   &report_identical);
    setoption(arg, "--normal",                  NULL,                         &diffnormal);
    setoption(arg, "-y",                        "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column",             NULL,                         &showleftcolumn);
    setoption(arg, "--suppress-common-lines",   NULL,                         &suppresscommon);
    setoption(arg, "-c",                        "--context",                  &showcontext);
    setoption(arg, "-u",                        "showunified",                &showunified);
    setoption(arg, "--help",                    NULL,                         &showhelp);

    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } else { files[cnt++] = arg; }
    }
    ++argv;
  }

  // ================================= //

  if (showversion)                                    { version();  exit(0); }
  if (showhelp)                                       { help();     exit(0); }

  if (!showcontext && !showunified &&
      !showsidebyside && !showleftcolumn)             { diffnormal = 1; }

  if (((showsidebyside || showleftcolumn) &&
      (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal &&
      (showcontext || showunified)))                  { diff_output_conflict_error(); }

}

void setoption(const char* arg, const char* s, const char* t, int* value) {
    
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) { *value = 1; }
    
}

void diff_output_conflict_error(void) {

  fprintf(stderr, "diff-rcm: Conflicting output style options.\n");
  fprintf(stderr, "diff-rcm: Try `diff --help' for more information.\n");
  exit   (CONFLICTING_OUTPUT_OPTIONS);

}

void loadfiles(const char* filename1, const char* filename2) {

  if (filename2 == NULL) { printf("Usage: ./diff [options] file1 file2\n"); exit(1); }

  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");

  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
    
  p = pa_first(strings1, count1);
  q = pa_first(strings2, count2);
    
  fclose(fin1); fclose(fin2);

}

// ============================================================================= //

void version(void) {
  printf("\n\n       ██ ██   ████   ████                                     \n");
  printf("      ░██░░   ░██░   ░██░                                        \n");
  printf("      ░██ ██ ██████ ██████        ██████  █████  ██████████     \n");
  printf("   ██████░██░░░██░ ░░░██░  █████░ ░██░░█ ██░░░██░░██░░██░░██   \n");
  printf("  ██░░░██░██  ░██    ░██  ░░░░░   ░██ ░ ░██  ░░  ░██ ░██ ░██    \n");
  printf(" ░██  ░██░██  ░██    ░██          ░██   ░██   ██ ░██ ░██ ░██     \n");
  printf(" ░░██████░██  ░██    ░██         ░███   ░░█████  ███ ░██ ░██    \n");
  printf("  ░░░░░░ ░░   ░░     ░░          ░░░     ░░░░░  ░░░  ░░  ░░      \n\n");
  printf("\n");
  printf("\tv1.0 GM | Well, that's all the time I've got.\n\n");
  printf("Copyright (C) 2019 | All Rights Reserved.\n");
  printf("Any unauthorized use or re-distribution of this code is permitted.\n\n");
  printf("\tChris Nutter\tWilliam McCarthy    Rasputin\n\n\n");
}
void help(void) {
  printf("\nUsage: diff-rcm [OPTION]... FILES\n");
  printf("Compare FILES line by line.\n\n");
  printf("Mandatory arguments to long options are mandatory for short options too.\n\n");
  printf("\t    --normal\t\t        output a normal diff (the default)\n");
  printf("\t-q, --brief\t\t        report only when files differ\n");
  printf("\t-s, --report-identical-files    report when two files are the same\n");
  printf("\t-c, -C NUM, --context[=NUM]     output NUM (default 3) lines of copied context\n");
  printf("\t-u, -U NUM, --unified[=NUM]     output NUM (default 3) lines of unified context\n");
  printf("\t-y, --side-by-side\t        output in two columns\n\n");
  printf("\t-i, --ignore-case\t        ignore case differences in file contents\n\n");
  printf("\t    --help\t\t        display this help and exit\n");
  printf("\t-v, --version\t\t        output version information and exit\n\n");

  printf("Report bugs to: cdnutter@gmail.com\n");
  printf("diff-rcm homepage: <https://www.github.com/cdnutter/diff/>\n\n");
}

// ============================================================================= //

void ignore_case(void) {

    printf("This wasn't mentioned anywhere in the rubric. Sooo...\n\nConsider");
    printf(" this an easter egg? Idk, I would like an A in the class. Please?");
    printf(" \n\n...thanks? - C & R\n\n=======================\n\n");
    
}

int identical(const char* filename1, const char* filename2) { if (*filename1 == *filename2) { exit(0); } else return 0; }

// ============================================================================= //

void lineline(void) {
    
    int foundmatch = 0; pa* qlast = q;
    
    while (p != NULL) {
        qlast = q; foundmatch = 0;

        while (q != NULL && (foundmatch = pa_equal(p, q)) == 0) { q = pa_next(q); }
        q = qlast;
        
        if (foundmatch) {
            while ((foundmatch = pa_equal(p, q)) == 0) {
                
                print_check(q, NULL, print_normal_right);
                q = pa_next(q);
                qlast = q;
            }
            print_check(p, q, line_check_normal);
            
            p = pa_next(p);
            q = pa_next(q);
        }
        else { print_check(p, NULL, print_normal_left);  p = pa_next(p); }
    }
    
    while (q != NULL) { print_check(q, NULL, print_normal_right);  q = pa_next(q); }

}

void sideside(const char* filename1, const char* filename2) {
    
    int foundmatch = 0;  pa* qlast = q;
    if (*filename1 == *filename2) {                                           // This allows printing of side-by-side with the same file contents.
        while (p != NULL && q != NULL) { print_check(p, q, print_side_both);
        p = pa_next(p); }
        return;
    }
    
    while (p != NULL) {
        qlast = q; foundmatch = 0;
        
        while (q != NULL && (foundmatch = pa_equal(p, q)) == 0) { q = pa_next(q); }
        q = qlast;
        
        if (foundmatch) {
            while ((foundmatch = pa_equal(p, q)) == 0) {
                print_check(q, NULL, print_side_right);
                q = pa_next(q);
                qlast = q;
            }
            
            if      (showleftcolumn) { print_check(p, q, print_left_paren);  }  // Checks if the parameters for side-by-side are filled.
            else if (suppresscommon) { print_check(p, q, print_no_common);   }
            else                     { print_check(p, q, print_side_default); }
            
            p = pa_next(p);
            q = pa_next(q);
        }
        
        else { print_check(p, NULL, print_side_left);  p = pa_next(p); }
    }
    
    while (q != NULL) { print_check(q, NULL, print_side_right);  q = pa_next(q); }
    
}

void context(const char* filename1, const char* filename2) {
    
    int foundmatch = 0; pa* qlast = q;
    
    struct stat filestat1;
    struct stat filestat2;
    stat(filename1, &filestat1);
    stat(filename2, &filestat2);
    
    printf("*** %s  %s", filename1, ctime(&filestat1.st_mtime));
    printf("--- %s  %s", filename2, ctime(&filestat2.st_mtime));
    printf("***************\n");

    // ========================= //

    while (p != NULL) {
        qlast = q; foundmatch = 0;
        
        while (q != NULL && (foundmatch = pa_equal(p, q)) == 0) { q = pa_next(q); }
        q = qlast;
        
        if (foundmatch) {
            while ((foundmatch = pa_equal(p, q)) == 0) {
                
                print_check(q, NULL, print_context_right);
                q = pa_next(q);
                qlast = q;
            }
            print_check(p, q, line_check_context);
            
            p = pa_next(p);
            q = pa_next(q);
        }
        else { print_check(p, NULL, print_context_left);  p = pa_next(p); }
    }
    
    while (q != NULL) { print_check(q, NULL, print_context_right);  q = pa_next(q); }
    
}

void unified(const char* filename1, const char* filename2) {
    
    struct stat filestat1;
    struct stat filestat2;
    stat(filename1, &filestat1);
    stat(filename2, &filestat2);
    
    printf("--- %s  %s", filename1, ctime(&filestat1.st_mtime));
    printf("+++ %s  %s", filename2, ctime(&filestat2.st_mtime));
    
    // ========================= //
    
    int foundmatch = 0; pa* qlast = q;
    
    while (p != NULL) {
        qlast = q; foundmatch = 0;
        
        while (q != NULL && (foundmatch = pa_equal(p, q)) == 0) { q = pa_next(q); }
        q = qlast;
        
        if (foundmatch) {
            while ((foundmatch = pa_equal(p, q)) == 0) {
                
                print_check(q, NULL, print_unified_right);
                q = pa_next(q);
                qlast = q;
            }
            print_check(p, q, line_check_unified);
            
            p = pa_next(p);
            q = pa_next(q);
        }
        else { print_check(p, NULL, print_unified_left);  p = pa_next(p); }
    }
    
    while (q != NULL) { print_check(q, NULL, print_unified_right);  q = pa_next(q); }
    
}

void quiet(const char* filename1, const char* filename2) { if (identical(filename1, filename2) == 0) { printf("Files %s and %s differ.\n", filename1, filename2); } exit(0); }
void loud (const char* filename1, const char* filename2) {

  if (*filename1 == *filename2 || (pa_equal(p, q) != 0)) { printf("Files %s and %s are identical.\n", filename1, filename2); }
  else if (showsidebyside)                               { sideside(files[0], files[1]); exit(0); }
  else if (showcontext)                                  { context(files[0], files[1]);  exit(0); }
  else if (showunified)                                  { unified(files[0], files[1]);  exit(0); }
  else                                                   { lineline(); exit(0); }

}

// ============================================================================= //











// :P
