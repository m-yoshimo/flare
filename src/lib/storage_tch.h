/**
 *	storage_tch.h
 *
 *	@author	Masaki Fujimoto <fujimoto@php.net>
 *
 *	$Id$
 */
#ifndef	__STORAGE_TCH_H__
#define	__STORAGE_TCH_H__

#include <boost/shared_ptr.hpp>

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif // HAVE_STDLIB_H

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif // HAVE_STDINT_H

#include "tcutil.h"
#include "tchdb.h"
#include "storage.h"

using namespace std;
using namespace boost;

namespace gree {
namespace flare {

/**
 *	storage class
 */
class storage_tch : public storage {
protected:
	static const type	_type = storage::type_tch;

	string						_data_path;
	TCHDB*						_db;

public:
	storage_tch(string data_dir);
	virtual ~storage_tch();

	virtual int open();
	virtual int close();

	virtual type get_type() { return this->_type; };
};

}	// namespace flare
}	// namespace gree

#endif	// __STORAGE_H__
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
