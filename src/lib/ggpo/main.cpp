/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#include "backends/p2p.h"
#include "backends/spectator.h"
#include "backends/synctest.h"
#include "ggponet.h"
#include "types.h"

void ggpo_log(GGPOSession *ggpo, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ggpo_logv(ggpo, fmt, args);
	va_end(args);
}

void ggpo_logv(GGPOSession *ggpo, const char *fmt, va_list args)
{
	if (ggpo) {
		ggpo->Logv(fmt, args);
	}
}

GGPOErrorCode ggpo_start_session(GGPOSession **session,
    GGPOSessionCallbacks *cb,
    GGPOConnectionCallbacks *connection,
    const char *game,
    int num_players,
    int input_size,
    void *user)
{
	*session = (GGPOSession *)new Peer2PeerBackend(
	    cb, connection, game, num_players, input_size, user);
	return GGPO_OK;
}

GGPOErrorCode ggpo_add_player(
    GGPOSession *ggpo, GGPOPlayer *player, GGPOPlayerHandle *handle)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->AddPlayer(player, handle);
}

GGPOErrorCode ggpo_start_synctest(GGPOSession **ggpo,
    GGPOSessionCallbacks *cb,
    char *game,
    int num_players,
    int input_size,
    int frames,
    void *user)
{
	*ggpo =
	    (GGPOSession *)new SyncTestBackend(cb, game, frames, num_players, user);
	return GGPO_OK;
}

GGPOErrorCode ggpo_set_frame_delay(
    GGPOSession *ggpo, GGPOPlayerHandle player, int frame_delay)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->SetFrameDelay(player, frame_delay);
}

GGPOErrorCode ggpo_idle(GGPOSession *ggpo, int timeout)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->DoPoll(timeout);
}

GGPOErrorCode ggpo_add_local_input(
    GGPOSession *ggpo, GGPOPlayerHandle player, void *values, int size)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->AddLocalInput(player, values, size);
}

GGPOErrorCode ggpo_synchronize_input(
    GGPOSession *ggpo, void *values, int size, int *disconnect_flags)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->SyncInput(values, size, disconnect_flags);
}

GGPOErrorCode ggpo_disconnect_player(GGPOSession *ggpo, GGPOPlayerHandle player)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->DisconnectPlayer(player);
}

GGPOErrorCode ggpo_advance_frame(GGPOSession *ggpo)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->IncrementFrame();
}

GGPOErrorCode ggpo_client_chat(GGPOSession *ggpo, char *text)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->Chat(text);
}

GGPOErrorCode ggpo_get_network_stats(
    GGPOSession *ggpo, GGPOPlayerHandle player, GGPONetworkStats *stats)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->GetNetworkStats(stats, player);
}

GGPOErrorCode ggpo_close_session(GGPOSession *ggpo)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	delete ggpo;
	return GGPO_OK;
}

GGPOErrorCode ggpo_set_disconnect_timeout(GGPOSession *ggpo, int timeout)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->SetDisconnectTimeout(timeout);
}

GGPOErrorCode ggpo_set_disconnect_notify_start(GGPOSession *ggpo, int timeout)
{
	if (!ggpo) {
		return GGPO_ERRORCODE_INVALID_SESSION;
	}
	return ggpo->SetDisconnectNotifyStart(timeout);
}

GGPOErrorCode ggpo_start_spectating(GGPOSession **session,
    GGPOSessionCallbacks *cb,
    GGPOConnectionCallbacks *connection,
    const char *game,
    int num_players,
    int input_size,
    GGPOConnectionPlayerID player_id,
    void *user_data)
{
	*session = (GGPOSession *)new SpectatorBackend(
	    cb, connection, game, num_players, input_size, player_id, user_data);
	return GGPO_OK;
}
