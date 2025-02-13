#if !defined(READ_H_)
#define READ_H_

#include "dados.h"

void setup_ils(Dados *, char *);

void process_line_ils(Dados *, char *);

void write_db_mediun(Dados * d, const char *);
void write_gap_mediun(Dados * d, const char *);
void write_time_mediun(Dados * d, const char *);
void write_db_large(Dados * d, const char *);
void write_gap_large(Dados * d, const char *);
void write_dbranch_complete(Dados * d, const char *);

void write_time_large(Dados * d, const char *);
void write_geral(Dados * d, const char *);
void write_geral_exato(Dados * d, const char *);



#endif // READ_H_
