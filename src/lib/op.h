/**
 *	op.h
 *
 *	@author	Masaki Fujimoto <fujimoto@php.net>
 *
 *	$Id$
 */
#ifndef	__OP_H__
#define	__OP_H__

#include "config.h"
#include "singleton.h"
#include "mm.h"
#include "logger.h"
#include "util.h"
#include "connection.h"
#include "thread.h"

using namespace std;
using namespace boost;

namespace gree {
namespace flare {

/**
 *	opcode base class
 */
class op {
public:
	enum									error_type {
		error_type_generic,
		error_type_client,
		error_type_server,
	};

protected:
	shared_connection			_connection;
	shared_thread					_thread;
	string								_ident;
	bool									_shutdown_request;

public:
	op(shared_connection c, string ident = "");
	virtual ~op();

	virtual int run_server();

	int set_thread(shared_thread t) { this->_thread = t; return 0; };
	string get_ident() { return this->_ident; };
	bool is_shutdown_request() { return this->_shutdown_request; };

protected:
	virtual int _parse_server_parameter();
	virtual int _run_server();

	int _send_ok();
	int _send_end();
	int _send_error(error_type t = error_type_generic, const char* m = NULL);
};

}	// namespace flare
}	// namespace gree

#endif	// __OP_H__
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
