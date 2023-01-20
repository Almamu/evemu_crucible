#include "EVESecureClientHandshake.h"

#include "python/Types.h"

EVESecureClientHandshake::EVESecureClientHandshake(const PyTuple* payload) {
  if (payload->size() != 2)
      throw std::runtime_error ("EVESecureClientHandshake::EVESecureClientHandshake - invalid tuple length");

  this->clientChallenge = payload->at(0)->string();

  auto dict = payload->at(1)->as<PyDict> ();

  this->macho_version = dict->get("macho_version")->i32();
  this->boot_version = dict->get("boot_version")->decimal();
  this->boot_build = dict->get("boot_build")->i32();
  this->boot_codename = dict->get("boot_codename")->string();
  this->user_name = dict->get("user_name")->string();
  this->user_password = dict->get("user_password")->string();
  this->user_password_hash = dict->get("user_password_hash")->string();
  this->user_languageid = dict->get("user_languageid")->string();
  this->user_affiliateid = dict->get("user_affiliateid")->i32();
}