#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include <string>


void load_blocked_domains(const std::string& filename);


bool is_blocked_domain(const std::string& host);

#endif
