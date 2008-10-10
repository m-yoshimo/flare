/**
 *	ini_option.h
 *
 *	@author	Masaki Fujimoto <fujimoto@php.net>
 *
 *	$Id$
 */
#ifndef __INI_OPTION_H__
#define __INI_OPTION_H__

#include "ini.h"

namespace gree {
namespace flare {

#define	ini_option_object()			singleton<ini_option>::instance()

/**
 *	flared global configuration class
 */
class ini_option : public ini {
private:
	int					_argc;
	char**			_argv;

	string			_config_path;
	bool				_daemonize;
	string			_data_dir;
	string			_log_facility;
	uint32_t		_max_connection;
	string			_server_name;
	int					_server_port;
	int					_thread_pool_size;
	
public:
	static const uint32_t default_max_connection = 128;
	static const int default_server_port = 12121;
	static const int default_thread_pool_size = 5;

	ini_option();
	virtual ~ini_option();

	int load();
	int reload();

	int set_args(int argc, char** argv) { this->_argc = argc; this->_argv = argv; return 0; };

	string get_config_path() { return this->_config_path; };
	bool is_daemonize() { return this->_daemonize; };
	string get_data_dir() { return this->_data_dir; };
	string get_log_facility() { return this->_log_facility; };
	uint32_t get_max_connection() { return this->_max_connection; };
	string get_server_name() { return this->_server_name; };
	int get_server_port() { return this->_server_port; };
	int get_thread_pool_size() { return this->_thread_pool_size; };

private:
	int _setup_cli_option(program_options::options_description& option);
	int _setup_config_option(program_options:: options_description& option);
};

}	// namespace flare
}	// namespace gree

#endif // __INI_OPTION_H__
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
