#ifndef METRICS_H
#define METRICS_H

#include <string>

void metrics_init();
void metrics_record_request(const std::string& host);
void metrics_print_summary();

#endif
