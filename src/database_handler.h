/*
 * Copyright (C) 2016 deipi.com LLC and contributors. All rights reserved.
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

#include "database.h"
#include "manager.h"
#include "multivalue/matchspy.h"
#include "multivalue/keymaker.h"
#include "schema.h"


extern const std::regex find_types_re;


using endpoints_error_list = std::unordered_map<std::string, std::vector<std::string>>;


class AggregationMatchSpy;


class DatabaseHandler {
	Endpoints endpoints;
	int flags;
	HttpMethod method;
	std::shared_ptr<Schema> schema;
	std::shared_ptr<Database> database;

	Xapian::Document _get_document(const std::string& term_id);
	MsgPack run_script(const MsgPack& data, const std::string& prefix_term_id);

	MsgPack _index(Xapian::Document& doc, const MsgPack& obj, std::string& term_id, const std::string& _document_id, const std::string& ct_type, const std::string& ct_length);

	void get_similar(Xapian::Enquire& enquire, Xapian::Query& query, const similar_field_t& similar, bool is_fuzzy=false);
	Xapian::Enquire get_enquire(Xapian::Query& query, const Xapian::valueno& collapse_key, const query_field_t* e, Multi_MultiValueKeyMaker* sorter, AggregationMatchSpy* aggs);

public:
	DatabaseHandler();
	DatabaseHandler(const Endpoints &endpoints_, int flags_=0);
	~DatabaseHandler();

	std::shared_ptr<Database> get_database() const noexcept {
		return database;
	}

	std::shared_ptr<Schema> get_schema() const {
		return std::make_shared<Schema>(XapiandManager::manager->database_pool.get_schema(endpoints[0], flags));
	}

	std::shared_ptr<Schema> get_fvschema() const {
		std::shared_ptr<const MsgPack> fvs, fvs_aux;
		for (const auto& e : endpoints) {
			fvs_aux = XapiandManager::manager->database_pool.get_schema(e, flags);	/* Get the first valid schema */
			if (fvs_aux->is_null()) {
				continue;
			}
			if (fvs == nullptr) {
				fvs = fvs_aux;
			} else if (*fvs != *fvs_aux) {
				throw MSG_ClientError("Cannot index in several indexes with different schemas");
			}
		}
		return std::make_shared<Schema>(fvs ? fvs : fvs_aux);
	}

	void checkout() {
		if (!database && !XapiandManager::manager->database_pool.checkout(database, endpoints, flags)) {
			throw MSG_CheckoutError("Cannot checkout database: %s", endpoints.as_string().c_str());
		}
	}

	void checkin() {
		if (database) {
			XapiandManager::manager->database_pool.checkin(database);
			database.reset();
		}
	}

	void reset(const Endpoints& endpoints_, int flags_, HttpMethod method_);

	Xapian::docid index(const std::string& body, const std::string& _document_id, bool commit_, const std::string& ct_type, const std::string& ct_length, endpoints_error_list* err_list=nullptr);
	Xapian::docid index(const MsgPack& obj, const std::string& _document_id, bool commit_, const std::string& ct_type, const std::string& ct_length);
	Xapian::docid patch(const std::string& patches, const std::string& _document_id, bool commit_, const std::string& ct_type, const std::string& ct_length);
	void write_schema(const std::string& body);

	void get_mset(const query_field_t& e, Xapian::MSet& mset, AggregationMatchSpy* aggs, std::vector<std::string>& suggestions, int offset=0);

	Xapian::Document get_document(const Xapian::docid& did) {
		L_CALL(this, "DatabaseHandler::get_document(1)");

		checkout();
		auto doc = database->get_document(did);
		checkin();
		return doc;
	}

	void update_schema() const {
		auto mod_schema = schema->get_modified_schema();
		if (mod_schema) {
			XapiandManager::manager->database_pool.set_schema(endpoints[0], flags, mod_schema);
		}
	}

	void update_schemas() const {
		auto mod_schema = schema->get_modified_schema();
		if (mod_schema) {
			for (const auto& e: endpoints) {
				XapiandManager::manager->database_pool.set_schema(e, flags, mod_schema);
			}
		}
	}

	Xapian::Document get_document(const std::string& doc_id);
	Xapian::docid get_docid(const std::string& doc_id);

	void delete_document(const std::string& doc_id, bool commit_=false, bool wal_=true);
	endpoints_error_list multi_db_delete_document(const std::string& doc_id, bool commit_=false, bool wal_=true);

	MsgPack get_value(const Xapian::Document& document, const std::string& slot_name);
	void get_document_info(MsgPack& info, const std::string& document_id);
	void get_database_info(MsgPack& info);
};
