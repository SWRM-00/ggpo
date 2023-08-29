/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#ifndef _UDP_PROTO_H_
#define _UDP_PROTO_H_

#include "game_input.h"
#include "ggponet.h"
#include "network.h"
#include "network_msg.h"
#include "poll.h"
#include "ring_buffer.h"
#include "timesync.h"

class NetworkProtocol : public IPollSink {
public:
	struct Stats {
		int ping;
		int remote_frame_advantage;
		int local_frame_advantage;
		int send_queue_len;
		Network::Stats network;
	};

	struct Event {
		enum Type {
			Unknown = -1,
			Connected,
			Synchronizing,
			Synchronzied,
			Input,
			Disconnected,
			NetworkInterrupted,
			NetworkResumed,
		};

		Type type;
		union {
			struct {
				GameInput input;
			} input;
			struct {
				int total;
				int count;
			} synchronizing;
			struct {
				int disconnect_timeout;
			} network_interrupted;
		} u;

		Event(Type t = Unknown) : type(t) {}
	};

public:
	virtual bool OnLoopPoll(void *cookie);

public:
	NetworkProtocol();
	virtual ~NetworkProtocol();

	void Init(Network *network,
	    Poll &p,
	    int queue,
	    GGPOConnectionPlayerID player_id,
	    NetworkMsg::connect_status *status);

	void Synchronize();
	bool GetPeerConnectStatus(int id, int *frame);
	bool IsInitialized() { return _network != NULL; }
	bool IsSynchronized() { return _current_state == Running; }
	bool IsRunning() { return _current_state == Running; }
	void SendInput(GameInput &input);
	void SendInputAck();
	bool HandlesMsg(GGPOConnectionPlayerID from, NetworkMsg *msg);
	void OnMsg(NetworkMsg *msg, int len);
	void Disconnect();

	void GetNetworkStats(struct GGPONetworkStats *stats);
	bool GetEvent(NetworkProtocol::Event &e);
	void GGPONetworkStats(Stats *stats);
	void SetLocalFrameNumber(int num);
	int RecommendFrameDelay();

	void SetDisconnectTimeout(int timeout);
	void SetDisconnectNotifyStart(int timeout);

protected:
	enum State { Syncing, Synchronzied, Running, Disconnected };

	struct QueueEntry {
		int queue_time;
		GGPOConnectionPlayerID dest_addr;
		NetworkMsg *msg;

		QueueEntry() {}
		QueueEntry(int time, GGPOConnectionPlayerID dst, NetworkMsg *m)
		    : queue_time(time), dest_addr(dst), msg(m)
		{
		}
	};

	bool CreateSocket(int retries);
	void UpdateNetworkStats(void);
	void QueueEvent(const NetworkProtocol::Event &evt);
	void ClearSendQueue(void);
	void Log(const char *fmt, ...);
	void LogMsg(const char *prefix, NetworkMsg *msg);
	void LogEvent(const char *prefix, const NetworkProtocol::Event &evt);
	void SendSyncRequest();
	void SendMsg(NetworkMsg *msg);
	void PumpSendQueue();
	void DispatchMsg(uint8 *buffer, int len);
	void SendPendingOutput();

	bool OnInvalid(NetworkMsg *msg, int len);
	bool OnSyncRequest(NetworkMsg *msg, int len);
	bool OnSyncReply(NetworkMsg *msg, int len);
	bool OnInput(NetworkMsg *msg, int len);
	bool OnInputAck(NetworkMsg *msg, int len);
	bool OnQualityReport(NetworkMsg *msg, int len);
	bool OnQualityReply(NetworkMsg *msg, int len);
	bool OnKeepAlive(NetworkMsg *msg, int len);

protected:
	/*
	 * Network transmission information
	 */
	Network *_network;
	GGPOConnectionPlayerID _peer_addr;
	uint16 _magic_number;
	int _queue;
	uint16 _remote_magic_number;
	bool _connected;
	int _send_latency;
	int _oop_percent;
	struct {
		NetworkMsg *msg;
		GGPOConnectionPlayerID dest_addr;
		int send_time;
	} _oo_packet;
	RingBuffer<QueueEntry, 64> _send_queue;

	/*
	 * Stats
	 */
	int _round_trip_time;
	int _packets_sent;
	int _bytes_sent;
	int _kbps_sent;
	int _stats_start_time;

	/*
	 * The state machine
	 */
	NetworkMsg::connect_status *_local_connect_status;
	NetworkMsg::connect_status _peer_connect_status[UDP_MSG_MAX_PLAYERS];

	State _current_state;
	union {
		struct {
			uint32 roundtrips_remaining;
			uint32 random;
		} sync;
		struct {
			uint32 last_quality_report_time;
			uint32 last_network_stats_interval;
			uint32 last_input_packet_recv_time;
		} running;
	} _state;

	/*
	 * Fairness.
	 */
	int _local_frame_advantage;
	int _remote_frame_advantage;

	/*
	 * Packet loss...
	 */
	RingBuffer<GameInput, 64> _pending_output;
	GameInput _last_received_input;
	GameInput _last_sent_input;
	GameInput _last_acked_input;
	unsigned int _last_send_time;
	unsigned int _last_recv_time;
	unsigned int _shutdown_timeout;
	unsigned int _disconnect_event_sent;
	unsigned int _disconnect_timeout;
	unsigned int _disconnect_notify_start;
	bool _disconnect_notify_sent;

	uint16 _next_send_seq;
	uint16 _next_recv_seq;

	/*
	 * Rift synchronization.
	 */
	TimeSync _timesync;

	/*
	 * Event queue
	 */
	RingBuffer<NetworkProtocol::Event, 64> _event_queue;
};

#endif
