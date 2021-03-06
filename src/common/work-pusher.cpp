/*
 * Copyright (c) 2016, Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>
#include <string>

#include "work-pusher.hpp"

WorkPusher::WorkPusher(
    boost::shared_ptr<zmq::context_t> ctx, std::string connection_endpoint)
      : zmq_ctx(ctx), endpoint(connection_endpoint) {
  socket.reset(new zmq::socket_t(*zmq_ctx, ZMQ_PUSH));
  socket->connect(endpoint.c_str());
}

void WorkPusher::push_work(uint cmd, std::vector<ZmqPortableBytes>& args) {
  boost::unique_lock<boost::mutex> lock(mutex);
  bool more = args.size() > 0;
  ZmqPortableBytes msg;
  msg.pack<uint>(cmd);
  send_msg(*socket, msg, more);
  if (more) {
    send_msgs(*socket, args);
  }
}

void WorkPusher::push_work(uint cmd) {
  std::vector<ZmqPortableBytes> args(0);    /* Empty args */
  push_work(cmd, args);
}

