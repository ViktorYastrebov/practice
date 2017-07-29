#include <stdio.h>
#include <stdlib.h>

#include "unicode/ustdio.h"
#include "unicode/uchar.h"

/**
* Writes the supplied UChar array to a UTF-8 file.
*
* @param filename the path to the file to be written
* @param str pointer to the UChar array to be written
*/

UChar* read_utf8_file(const char* filename, long* size) {
  /* open a UTF-8 file for reading */
  UFILE* f = u_fopen(filename, "r", NULL, "UTF-8");

  /* get the file size */
  long fsize;
  fseek(u_fgetfile(f), 0, SEEK_END);
  fsize = ftell(u_fgetfile(f));
  u_frewind(f);

  /* allocate enough memory to store the whole string plus termination */
  UChar* str = (UChar*)malloc(sizeof(UChar) * (fsize + 1));

  /* read the string into the allocated space */
  for ((*size) = 0; !u_feof(f); ++(*size)) {
    str[*size] = u_fgetc(f);
  }

  /* add NULL termination */
  str[*size] = 0;

  /* close the file resource */
  u_fclose(f);

  return str;
}

void write_utf8_file(const char* filename, const UChar* str) {
  /* open a UTF-8 file for writing */
  UFILE* f = u_fopen(filename, "w", NULL, "UTF-8");

  /* write Unicode string to file */
  u_fputs(str, f);

  /* close the file resource */
  u_fclose(f);
}

int main() {
  /* read the string and its size */
  long size;
  UChar* str = read_utf8_file("utf8_test.txt", &size);

  /* write the string to a separate file */
  write_utf8_file("utf8_test_out.txt", str);

  /* free the allocated string */
  free(str);

  return 0;
}