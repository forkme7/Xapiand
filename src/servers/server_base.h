/*
 * Copyright (C) 2015 deipi.com LLC and contributors. All rights reserved.
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

#include <time.h>     // for time_t
#include <memory>     // for shared_ptr

#include "ev/ev++.h"  // for io, loop_ref (ptr only)
#include "worker.h"   // for Worker

class XapiandServer;


// This class lets make different types of servers.
class BaseServer : public Worker {
	friend Worker;
	friend XapiandServer;

protected:
	ev::io io;

	BaseServer(const std::shared_ptr<XapiandServer>& server_, ev::loop_ref* ev_loop_, unsigned int ev_flags_);

	void destroyer();

	void destroy_impl() override;
	void shutdown_impl(time_t asap, time_t now) override;

public:
	virtual ~BaseServer();

	virtual void io_accept_cb(ev::io& watcher, int revents) = 0;
};
