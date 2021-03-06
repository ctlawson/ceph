// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2017 Red Hat, Inc
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#pragma once

#include <seastar/core/future.hh>

#include "Fwd.h"

class AuthAuthorizer;

namespace ceph::net {

class Messenger {
  entity_name_t my_name;
  entity_addrvec_t my_addrs;
  uint32_t global_seq = 0;
  uint32_t crc_flags = 0;

 public:
  Messenger(const entity_name_t& name)
    : my_name(name)
  {}
  virtual ~Messenger() {}

  const entity_name_t& get_myname() const { return my_name; }
  const entity_addrvec_t& get_myaddrs() const { return my_addrs; }
  entity_addr_t get_myaddr() const { return my_addrs.front(); }
  virtual void set_myaddrs(const entity_addrvec_t& addrs) {
    my_addrs = addrs;
  }

  /// bind to the given address
  virtual void bind(const entity_addrvec_t& addr) = 0;

  /// start the messenger
  virtual seastar::future<> start(Dispatcher *dispatcher) = 0;

  /// either return an existing connection to the peer,
  /// or a new pending connection
  virtual ConnectionRef connect(const entity_addr_t& peer_addr,
                                const entity_type_t& peer_type) = 0;

  /// stop listenening and wait for all connections to close. safe to destruct
  /// after this future becomes available
  virtual seastar::future<> shutdown() = 0;

  uint32_t get_global_seq(uint32_t old=0) {
    if (old > global_seq) {
      global_seq = old;
    }
    return ++global_seq;
  }

  uint32_t get_crc_flags() const {
    return crc_flags;
  }
  void set_crc_data() {
    crc_flags |= MSG_CRC_DATA;
  }
  void set_crc_header() {
    crc_flags |= MSG_CRC_HEADER;
  }

  virtual void print(ostream& out) const = 0;
};

inline ostream& operator<<(ostream& out, const Messenger& msgr) {
  out << "[";
  msgr.print(out);
  out << "]";
  return out;
}

} // namespace ceph::net
