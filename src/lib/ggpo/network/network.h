/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#ifndef _UDP_H
#define _UDP_H

#include "ggponet.h"
#include "network_msg.h"
#include "poll.h"
#include "ring_buffer.h"

#define MAX_UDP_ENDPOINTS 16

static const int MAX_UDP_PACKET_SIZE = 4096;

class Network : public IPollSink {
public:
	struct Stats {
		int bytes_sent;
		int packets_sent;
		float kbps_sent;
	};

	struct Callbacks {
		virtual ~Callbacks() {}
		virtual void OnMsg(
		    GGPOConnectionPlayerID from, NetworkMsg *msg, int len) = 0;
	};

protected:
	void Log(const char *fmt, ...);

public:
	Network();

	void Init(Poll *p,
	    Callbacks *callbacks,
	    GGPOConnectionCallbacks *connection_callbacks);
	void SendTo(char *buffer,
	    int len,
	    int flags,
	    GGPOConnectionPlayerID dst,
	    int destlen);

	virtual bool OnLoopPoll(void *cookie);

public:
	~Network(void);

protected:
	// Remote network.
	GGPOConnectionCallbacks _connection_callbacks;

	// state management
	Callbacks *_callbacks;
	Poll *_poll;
};

#endif
