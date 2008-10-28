/**
 *	cluster.h
 *
 *	@author	Masaki Fujimoto <fujimoto@php.net>
 *
 *	$Id$
 */
#ifndef	__CLUSTER_H__
#define	__CLUSTER_H__

#include <map>
#include <vector>
#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "connection.h"
#include "thread_pool.h"

using namespace std;
using namespace boost;

namespace gree {
namespace flare {

typedef class op_set op_set;

/**
 *	cluster class
 */
class cluster {
public:
	enum							type {
		type_index,
		type_node,
	};

	enum							role {
		role_master,
		role_slave,
		role_proxy,
	};

	enum							state {
		state_active,
		state_prepare,
		state_down,
	};

	enum							proxy_request {
		proxy_request_error_partition = -1,
		proxy_request_continue = 1,
		proxy_request_complete,
	};

	typedef struct		_node {
		string					node_server_name;
		int							node_server_port;
		role						node_role;
		state						node_state;
		int							node_partition;
		int							node_balance;
		int							node_thread_type;

		int parse(const char* p);

	private:
		friend class serialization::access;
		template<class T> void serialize(T& ar, uint32_t version) {
			ar & node_server_name;
			ar & node_server_port;
			ar & node_role;
			ar & node_state;
			ar & node_partition;
			ar & node_balance;
			ar & node_thread_type;
		};
	} node;

	typedef struct		_partition_node {
		string					node_key;
		int							node_balance;
	} partition_node;

	typedef struct		_partition {
		partition_node					master;
		vector<partition_node>	slave;
	} partition;
	
	typedef map<string, node>		node_map;
	typedef map<int, partition>	node_partition_map;

	static const int	default_thread_type = 16;

protected:
	thread_pool*					_thread_pool;
	type									_type;
	string								_data_dir;
	pthread_mutex_t				_mutex_serialization;

	node_map							_node_map;
	node_partition_map		_node_partition_map;
	node_partition_map		_node_partition_prepare_map;
	pthread_rwlock_t			_mutex_node_map;
	pthread_rwlock_t			_mutex_node_partition_map;

	string								_node_key;
	string								_server_name;
	int										_server_port;

	// [index]
	int										_monitor_threshold;
	int										_monitor_interval;
	int										_thread_type;

	// [node]
	string								_index_server_name;
	int										_index_server_port;

public:
	cluster(thread_pool* tp, string data_dir, string server_name, int server_port);
	virtual ~cluster();

	int startup_index();
	int startup_node(string index_server_name, int index_server_port);

	int add_node(string node_server_name, int node_server_port);
	int down_node(string node_server_name, int node_server_port);
	int up_node(string node_server_name, int node_server_port);
	int remove_node(string node_server_name, int node_server_port);
	int set_node_role(string node_server_name, int node_server_port, role node_role, int node_balance, int node_partition);
	int set_node_state(string node_server_name, int node_server_port, state node_state);
	int reconstruct_node(vector<node> v);

	proxy_request pre_proxy_write(op_set* op);

	inline node get_node(string node_key) {
		node n;
		pthread_rwlock_rdlock(&this->_mutex_node_map);
		if (this->_node_map.count(node_key) > 0) {
			n = this->_node_map[node_key];
		} else {
			n.node_server_name = "";
			n.node_server_port = 0;
		}
		pthread_rwlock_unlock(&this->_mutex_node_map);

		return n;
	};
	inline node get_node(string node_server_name, int node_server_port) {
		return this->get_node(this->to_node_key(node_server_name, node_server_port));
	}
	vector<node> get_node();

	int set_monitor_threshold(int monitor_threshold);
	int set_monitor_interval(int monitor_interval);
	string get_server_name() { return this->_server_name; };
	int get_server_port() { return this->_server_port; };
	string get_index_server_name() { return this->_index_server_name; };
	int get_index_server_port() { return this->_index_server_port; };

	inline string to_node_key(string server_name, int server_port) {
		string node_key = server_name + ":" + lexical_cast<string>(server_port);
		return node_key;
	};

	inline int from_node_key(string node_key, string& server_name, int& server_port) {
		uint32_t n = node_key.find(":", 0);
		if (n == string::npos) {
			return -1;
		}
		server_name = node_key.substr(0, n);
		try {
			server_port = lexical_cast<int>(node_key.substr(n+1));
		} catch (bad_lexical_cast e) {
			return -1;
		}
		return 0;
	}

	static inline int role_cast(string s, role& r) {
		if (s == "master") {
			r = role_master;
		} else if (s == "slave") {
			r = role_slave;
		} else if (s == "proxy") {
			r = role_proxy;
		} else {
			return -1;
		}
		return 0;
	}

	static inline string role_cast(role r) {
		switch (r) {
		case role_master:
			return "master";
		case role_slave:
			return "slave";
		case role_proxy:
			return "proxy";
		}
		return "";
	}

	static inline int state_cast(string s, state& t) {
		if (s == "active") {
			t = state_active;
		} else if (s == "prepare") {
			t = state_prepare;
		} else if (s == "down") {
			t = state_down;
		} else {
			return -1;
		}
		return 0;
	}

	static inline string state_cast(state t) {
		switch (t) {
		case state_active:
			return "active";
		case state_prepare:
			return "prepare";
		case state_down:
			return "down";
		}
		return "";
	}

protected:
	int _broadcast(shared_thread_queue q, bool sync = false);
	int _save();
	int _load();
	int _reconstruct_node_partition(bool lock = true);
	int _check_node_balance(string node_key, int node_balance);
	int _check_node_partition(int node_partition, bool& preparing);
	int _check_node_partition_for_new(int node_partition, bool& preparing);
	int _determine_partition(string key, partition& p);
	string _get_partition_key(string key);
	inline int _get_partition_hash(string key) { const char* p = key.c_str(); int n = 0; while (*p) { n += static_cast<int>(*p); p++; } return n; };
};

}	// namespace flare
}	// namespace gree

#endif	// __CLUSTER_H__
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
