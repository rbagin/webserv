#ifndef CGIRESULT_HPP
# define CGIRESULT_HPP

# include <string>

/*
** Shared contract object: Ivan's CGIProcess produces it, Ravi's
** HttpResponse::fromCGI() consumes it. Ivan never interprets the
** bytes in `output`, Ravi never runs fork/exec/waitpid.
*/
struct CGIResult {
	bool		success;	// process exited cleanly
	std::string	output;		// raw stdout bytes (CGI headers + body)
	bool		timedOut;	// true if SIGKILL was sent
	int			exitCode;	// process exit status

	CGIResult() : success(false), timedOut(false), exitCode(0) {}
};

#endif
