/*
 * Copyright (C) 2015,2016,2017 deipi.com LLC and contributors. All rights reserved.
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

#include "test_threadpool.h"

#include "utils.h"


using namespace queue;


int test_pool() {
	INIT_LOG
	std::string results;
	ThreadPool<> pool("W%zu", 4);
	pool.enqueue(std::make_shared<TestTask>("1", 0.08, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("2", 0.02, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("3", 0.04, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("4", 0.01, results));
	pool.end();
	pool.join();

	if (results != "<1<2<3<44>2>3>1>") {
		L_ERR(nullptr, "ThreadPool::enqueue is not working correctly. Result: %s  Expected: <1<2<3<44>2>3>1>", results.c_str());
		RETURN(1);
	}

	RETURN(0);
}


int test_pool_limit() {
	INIT_LOG
	std::string results;
	ThreadPool<> pool("W%zu", 3);
	pool.enqueue(std::make_shared<TestTask>("1", 0.08, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("2", 0.02, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("3", 0.04, results));
	std::this_thread::sleep_for(std::chrono::duration<double>(0.001));
	pool.enqueue(std::make_shared<TestTask>("4", 0.01, results));
	pool.end();
	pool.join();
	if (results != "<1<2<32><44>3>1>") {
		L_ERR(nullptr, "ThreadPool::enqueue is not working correctly. Result: %s  Expected: <1<2<32><44>3>1>", results.c_str());
		RETURN(1);
	}

	RETURN(0);
}


int test_pool_func() {
	INIT_LOG
	ThreadPool<> pool("W%zu", 4);
	std::vector<std::shared_future<int>> results;
	test_pool_class_t obj;

	int i = 1;

	// Using lambda without parameters
	results.emplace_back(pool.enqueue([i = i]() {
		return i * i;
	}));
	++i;

	// Using lambda with parameters
	results.emplace_back(pool.enqueue([](int i) {
		return i * i;
	}, i));
	++i;

	// Using regular function
	results.emplace_back(pool.enqueue(_test_pool_func_func, i));
	++i;

	// Using member function
	results.emplace_back(pool.enqueue([&obj](int i) {
		return obj.func(i);
	}, i));
	++i;

	int total = 0;
	for (auto& result: results) {
		total += result.get();
	}

	if (total != 30) {
		L_ERR(nullptr, "ThreadPool::enqueue functions with int is not working correctly. Result: %d Expect: 30", total);
		RETURN(1);
	}

	pool.end();
	pool.join();

	RETURN(0);
}


int test_pool_func_shared() {
	INIT_LOG
	ThreadPool<> pool("W%zu", 4);
	std::vector<std::shared_future<int>> results;
	test_pool_class_t obj;

	int i = 1;

	// shared_ptr
	// Using lambda without parameters
	results.emplace_back(pool.enqueue([i = std::make_shared<int>(i)]() {
		return *i * *i;
	}));
	++i;

	// Using lambda with parameters
	results.emplace_back(pool.enqueue([](std::shared_ptr<int> i) {
		return *i * *i;
	}, std::make_shared<int>(i)));
	++i;

	// Using regular function
	results.emplace_back(pool.enqueue(_test_pool_func_func_shared, std::make_shared<int>(i)));
	++i;

	// Using member function
	results.emplace_back(pool.enqueue([&obj](std::shared_ptr<int> i) {
		return obj.func_shared(i);
	}, std::make_shared<int>(i)));
	++i;

	int total = 0;
	for (auto& result: results) {
		total += result.get();
	}

	if (total != 30) {
		L_ERR(nullptr, "ThreadPool::enqueue functions with std::shared_ptr is not working correctly. Result: %d Expect: 30", total);
		RETURN(1);
	}

	pool.end();
	pool.join();

	RETURN(0);
}


int test_pool_func_unique() {
	INIT_LOG
	ThreadPool<> pool("W%zu", 4);
	std::vector<std::shared_future<int>> results;
	test_pool_class_t obj;

	int i = 1;

	// unique_ptr
	// Using lambda without parameters
	results.emplace_back(pool.enqueue([i = std::make_unique<int>(i)]() {
		return *i * *i;
	}));
	++i;

	// Using lambda with parameters
	results.emplace_back(pool.enqueue([](std::unique_ptr<int> i) {
		return *i * *i;
	}, std::make_unique<int>(i)));
	++i;

	// Using regular function
	results.emplace_back(pool.enqueue(_test_pool_func_func_unique, std::make_unique<int>(i)));
	++i;

	// Using member function
	results.emplace_back(pool.enqueue([&obj](std::unique_ptr<int> i) {
		return obj.func_unique(std::move(i));
	}, std::make_unique<int>(i)));
	++i;

	int total = 0;
	for (auto& result: results) {
		total += result.get();
	}

	if (total != 30)  {
		L_ERR(nullptr, "ThreadPool::enqueue functions with std::unique_ptr is not working correctly. Result: %d Expect: 30", total);
		RETURN(1);
	}

	pool.end();
	pool.join();

	RETURN(0);
}
