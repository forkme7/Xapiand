/*
 * Copyright (C) 2015,2016 deipi.com LLC and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "xapiand.h"


#ifdef XAPIAND_CLUSTERING

#include "raft.h"
#include "server_base.h"


// Raft Server
class RaftServer : public BaseServer {
	friend Raft;

	std::shared_ptr<Raft> raft;

	Node unserialise_remote_node(const char **p, const char *p_end);

	void raft_server(Raft::Message type, const std::string& message);

	void heartbeat_leader(const std::string& message);
	void request_vote(const std::string& message);
	void response_vote(const std::string& message);
	void leader(const std::string& message);
	void leadership(const std::string& message);
	void reset(const std::string& message);

public:
	std::string __repr__() const override {
		return Worker::__repr__("RaftServer");
	}

	RaftServer(const std::shared_ptr<XapiandServer>& server_, ev::loop_ref* ev_loop_, unsigned int ev_flags_, const std::shared_ptr<Raft>& raft_);
	~RaftServer();

	void io_accept_cb(ev::io& watcher, int revents) override;
};

#endif
