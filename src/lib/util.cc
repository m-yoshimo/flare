/**
 *	util.cc
 *	
 *	implementation of gree::flare::util
 *
 *	@author	Masaki Fujimoto <fujimoto@php.net>
 *	
 *	$Id$
 */
#include "util.h"

namespace gree {
namespace flare {

const char* const line_delimiter = "\r\n";

// {{{ ctor/dtor
// }}}

// {{{ operator overloads
// }}}

// {{{ public methods
/**
 *	strerror (thread safe)
 */
const char* util::strerror(int e) {
	static map<pthread_t, string> msg_map;
	char buf[BUFSIZ];

	// FIXME: glibc dependent
	char* p = strerror_r(e, buf, sizeof(buf));
	msg_map[pthread_self()] = p;

	return msg_map[pthread_self()].c_str();
}

/**
 *	thread safe inet_ntoa()
 */
int util::inet_ntoa(struct in_addr in, char* dst) {
	static pthread_mutex_t m;
	static bool is_mutex_initialized = false;

	if (is_mutex_initialized == false) {
		pthread_mutex_init(&m, NULL);
		is_mutex_initialized = true;
	}

	char* p;
	pthread_mutex_lock(&m);
	p = ::inet_ntoa(in);
	strcpy(dst, p);
	pthread_mutex_unlock(&m);

	return 0;
}

/**
 *	get my own fqdn
 */
int util::get_fqdn(string& fqdn) {
	char buf_hostname[BUFSIZ];

	if (gethostname(buf_hostname, sizeof(buf_hostname)) != 0) {
		log_warning("gethostname() failed: %s (%d)", util::strerror(errno), errno);
		return -1;
	}

	struct hostent he;
	struct hostent* hp;
	int e;
	char buf_hostent[BUFSIZ];
	if (gethostbyname_r(buf_hostname, &he, buf_hostent, sizeof(buf_hostent), &hp, &e) != 0) {
		log_warning("gethostbyname_r() failed: %s (%d)", util::strerror(e), e);
		return -1;
	}

	fqdn = he.h_name;

	return 0;
}

/**
 *	get next word from buffer
 */
uint32_t util::next_word(const char* src, char* dst, uint32_t dst_len) {
	const char *p = src;
	char *q = dst;

	// sync w/ memcached behavior (cannot use isspace() here because memcached does not recognize '\t' and other space chars as ws)
	while (*p && *p == ' ') {
		p++;
	}
	while (*p && *p != ' ' && *p != '\n' && static_cast<uint32_t>(p-src) < dst_len) {
		*q++ = *p++;
	}
	*q = '\0';

	return p-src;
}

/**
 *	get next digit(s) from buffer
 */
uint32_t util::next_digit(const char* src, char* dst, uint32_t dst_len) {
	const char *p = src;
	char *q = dst;

	// sync w/ memcached behavior (cannot use isspace() here because memcached does not recognize '\t' and other space chars as ws)
	while (*p && *p == ' ') {
		p++;
	}
	while (*p && isdigit(*p) && *p != '\n' && static_cast<uint32_t>(p-src) < dst_len) {
		*q++ = *p++;
	}
	*q = '\0';

	return p-src;
}

/**
 *	join vector<T>
 */
template<class T> string util::vector_join(vector<T> list, string glue) {
	ostringstream sout;
	typename vector<T>::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		if (sout.tellp() > 0) {
			sout << glue;
		}
		sout << *it;
	}

	return sout.str();
}

/**
 *	split string
 */
template<class T> vector<T> util::vector_split(string s, string sep) {
	vector<T> r;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> separator(sep.c_str());
	tokenizer token_list(s, separator);
	for (tokenizer::iterator it = token_list.begin(); it != token_list.end(); it++) {
		r.push_back(lexical_cast<T>(*it));
	}

	return r;
}
// }}}

// {{{ protected methods
// }}}

// {{{ private methods
// }}}

}	// namespace flare
}	// namespace gree
// vim: foldmethod=marker tabstop=2 shiftwidth=2 autoindent
