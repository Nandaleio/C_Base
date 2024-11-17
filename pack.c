// Copyright (c) Cesanta Software Limited
// All rights reserved.

// This program is used to pack arbitrary data into a C binary. It takes
// a list of files as an input, and produces a .c data file that contains
// contents of all these files as a collection of byte arrays.
//
// Usage:
//   1. Compile this file:
//      cc -o pack pack.c
//
//   2. Convert list of files into single .c:
//      ./pack file1.data file2.data > fs.c
//
//   3. In your application code, you can access files using this function:
//      const char *mg_unpack(const char *file_name, size_t *size);
//
//   4. Build your app with fs.c:
//      cc -o my_app my_app.c fs.c

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define OUTPUT_FILE "fs.c"

static const char *code =
    "static int scmp(const char *a, const char *b) {\n"
    "  while (*a && (*a == *b)) a++, b++;\n"
    "  return *(const unsigned char *) a - *(const unsigned char *) b;\n"
    "}\n"
    "const char *mg_unlist(size_t no) {\n"
    "  return packed_files[no].name;\n"
    "}\n"
    "const char *mg_unpack(const char *name, size_t *size, time_t *mtime) {\n"
    "  const struct packed_file *p;\n"
    "  for (p = packed_files; p->name != NULL; p++) {\n"
    "    if (scmp(p->name, name) != 0) continue;\n"
    "    if (size != NULL) *size = p->size - 1;\n"
    "    if (mtime != NULL) *mtime = p->mtime;\n"
    "    return (const char *) p->data;\n"
    "  }\n"
    "  return NULL;\n"
    "}\n";

int main(int argc, char *argv[]) {
  int i, j, ch;
  const char *strip_prefix = "";

  FILE *fptr;

  fptr = fopen(OUTPUT_FILE, "w");

  fprintf(fptr, "%s", "#include <stddef.h>\n");
  fprintf(fptr, "%s", "#include <string.h>\n");
  fprintf(fptr, "%s", "#include <time.h>\n");
  fprintf(fptr, "%s", "\n");
  fprintf(fptr, "%s", "#if defined(__cplusplus)\nextern \"C\" {\n#endif\n");
  fprintf(fptr, "%s", "const char *mg_unlist(size_t no);\n");
  fprintf(fptr, "%s", "const char *mg_unpack(const char *, size_t *, time_t *);\n");
  fprintf(fptr, "%s", "#if defined(__cplusplus)\n}\n#endif\n\n");

  for (i = 1; i < argc; i++) {

    if (strcmp(argv[i], "-s") == 0) {
      strip_prefix = argv[++i];
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      fprintf(stderr, "Usage: %s[-s STRIP_PREFIX] files...\n", argv[0]);
      exit(EXIT_FAILURE);
    } else {
      char ascii[12];
      FILE *fp = fopen(argv[i], "rb");
      if (fp == NULL) {
        fprintf(stderr, "Cannot open [%s]: %s\n", argv[i], strerror(errno));
        exit(EXIT_FAILURE);
      }

      fprintf(fptr, "static const unsigned char v%d[] = {\n", i);
      for (j = 0; (ch = fgetc(fp)) != EOF; j++) {
        if (j == (int) sizeof(ascii)) {
          fprintf(fptr, "\n");
          j = 0;
        }
        ascii[j] = (char) ((ch >= ' ' && ch <= '~' && ch != '\\') ? ch : '.');
        fprintf(fptr, " %3u,", ch);
      }
      // Append zero byte at the end, to make text files appear in memory
      // as nul-terminated strings.
      fprintf(fptr, " 0 \n};\n");
      fclose(fp);
    }
  }

  fprintf(fptr, "%s", "\nstatic const struct packed_file {\n");
  fprintf(fptr, "%s", "  const char *name;\n");
  fprintf(fptr, "%s", "  const unsigned char *data;\n");
  fprintf(fptr, "%s", "  size_t size;\n");
  fprintf(fptr, "%s", "  time_t mtime;\n");
  fprintf(fptr, "%s", "} packed_files[] = {\n");

  for (i = 1; i < argc; i++) {
    struct stat st;
    const char *name = argv[i];
    size_t n = strlen(strip_prefix);
    if (strcmp(argv[i], "-s") == 0) {
      i++;
      continue;
    }
    stat(argv[i], &st);
    if (strncmp(name, strip_prefix, n) == 0) name += n;
    fprintf(fptr, "  {\"/%s\", v%d, sizeof(v%d), %lu},\n", name, i, i,
           (unsigned long) st.st_mtime);
  }
  fprintf(fptr, "%s", "  {NULL, NULL, 0, 0}\n");
  fprintf(fptr, "%s", "};\n\n");
  fprintf(fptr, "%s", code);

  fclose(fptr);

  return EXIT_SUCCESS;
}
