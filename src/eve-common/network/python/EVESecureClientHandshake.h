#pragma once

#include <string>

class PyTuple;

class EVESecureClientHandshake {
public:
  EVESecureClientHandshake(const PyTuple* payload);

  std::string clientChallenge;
  int macho_version;
  float boot_version;
  int boot_build;
  std::string boot_codename;
  std::string boot_region;
  std::string user_name;
  std::string user_password;
  std::string user_password_hash;
  std::string user_languageid;
  int user_affiliateid;
};
