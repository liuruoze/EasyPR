#include "easypr/core/params.h"

namespace easypr {
  CParams* CParams::instance_ = nullptr;

  CParams* CParams::instance() {
    if (!instance_) {
      instance_ = new CParams;
    }
    return instance_;
  }
}/*! \namespace easypr*/