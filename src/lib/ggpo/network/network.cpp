/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#include "network.h"
#include "types.h"

Network::Network() : _connection_callbacks(), _callbacks(NULL)
{
}

Network::~Network(void)
{
}

void Network::Init(Poll *poll,
    Callbacks *callbacks,
    GGPOConnectionCallbacks *connection_callbacks)
{
	_callbacks = callbacks;
	_connection_callbacks = *connection_callbacks;

	_poll = poll;
	_poll->RegisterLoop(this);
}

void Network::SendTo(
    char *buffer, int len, int flags, GGPOConnectionPlayerID dst, int destlen)
{
	_connection_callbacks.send_message(
	    buffer, len, flags, dst, _connection_callbacks.user_data);
	Log("sent packet length %d to %lu.\n", len, dst);
}

bool Network::OnLoopPoll(void *cookie)
{
	uint8 recv_buf[MAX_UDP_PACKET_SIZE];

	for (;;) {
		GGPOConnectionPlayerID player_id = -1;
		int len = _connection_callbacks.poll_message((char *)recv_buf,
		    MAX_UDP_PACKET_SIZE,
		    0,
		    &player_id,
		    _connection_callbacks.user_data);

		// TODO: handle len == 0... indicates a disconnect.

		if (len == -1) {
			Log("Unable to poll for events.\n");
			break;
		}
		else if (len > 0) {
			Log("recvfrom returned (len:%d  from:%lld).\n", len, player_id);
			NetworkMsg *msg = (NetworkMsg *)recv_buf;
			_callbacks->OnMsg(player_id, msg, len);
		}
	}
	return true;
}

void Network::Log(const char *fmt, ...)
{
	char buf[1024];
	size_t offset;
	va_list args;

	strcpy(buf, "Network | ");
	offset = strlen(buf);
	va_start(args, fmt);
	vsnprintf(buf + offset, ARRAY_SIZE(buf) - offset - 1, fmt, args);
	buf[ARRAY_SIZE(buf) - 1] = '\0';
	::Log(buf);
	va_end(args);
}
