/**
 *	op_show_node.h
 *
 *	@author	Takanori Sejima <takanori.sejima@gmail.com>
 *
 *	$Id$
 */
#ifndef OP_SHOW_NODE_H
#define OP_SHOW_NODE_H

#include <sstream>

#include "op_show.h"

namespace gree {
namespace flare {

/**
 *	opcode class (show)
 */
class op_show_node : public op_show {
protected:

public:
	op_show_node(shared_connection c);
	virtual ~op_show_node();

protected:
	virtual int _parse_text_server_parameters();
	virtual int _run_server();

	virtual int _send_show_variables();
};

}	// namespace flare
}	// namespace gree

#endif // OP_SHOW_NODE_H
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
