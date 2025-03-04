#include <stddef.h>

#define LEN(X)  (sizeof(X) / sizeof((X)[0]))

/**
 * Writes to stderr prefixing the data with "tstat: " and appending a new line
 * character.
 *
 * @param fmt Pointer to a null-terminated character string specifying how to
 *            interpret the data.
 * @param ... Arguments specifying data to print.
 */
void error(const char *fmt, ...);

/**
 * Loads data from the given locations, converts them to character string
 * equivalents and writes the results to the character string buffer.
 *
 * At most bfsz - 1 characters are written. The resulting character string will
 * be terminated with a null character, unless bfsz is zero. If bfsz is zero,
 * nothing is written and buffer may be a null pointer, however the return value
 * (number of bytes that would be written not including the null terminator) is
 * still calculated and returned.
 *
 * This function calls error if an error occurs or if the number of characters
 * written was truncated due to the bfsz limit.
 *
 * @param buffer Pointer to a character string to write to.
 * @param bfsz Limit on the number of characters that may be written, including
 *             the null terminator.
 * @param fmt Pointer to a null-terminated character string specifying how to
 *            interpret the data.
 * @param ... Arguments specifying data to print.
 * @return The number of characters written if successful or a negative value if
 *         an error occurred. If the resulting string gets truncated due to the
 *         bfsz limit, the function returns the total number of characters (not
 *         including the terminating null-byte) which would have been written,
 *         if the limit was not imposed.
 */
int esnprintf(char *buffer, size_t bfsz, const char *fmt, ...);

/**
 * Reads data from the file indicated by filename, interprets it according to
 * the given format string and stores the results into the given locations.
 *
 * This function calls error if an error occurs when opening the file or if read
 * failure occurs before the first receiving argument was assigned.
 *
 * @param filename File name to read from.
 * @param fmt Pointer to a null-terminated character string specifying how to
 *            read the input.
 * @param ... Receiving arguments.
 * @return Number of receiving arguments successfully assigned, or EOF if
 *   - an error occurs while opening the file, or
 *   - a read failure occurs before the first receiving argument was assigned.
 * @see error()
 */
int epscanf(const char *filename, const char *fmt, ...);
