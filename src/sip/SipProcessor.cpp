/**
 * This file is part of callx. The application callx performs the call
 * extraction as well as the signaling-based analysis in the VIAT system.
 *
 * http://viat.fh-koeln.de
 *
 * Copyright (C) 2013 Bernhard Mainka (mail@bmainka.de),
 * Cologne University of Applied Sciences
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * SipProcessor.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: mainka
 */

#include "SipProcessor.hpp"
#include "Call.hpp"
#include "config/CallxConfig.hpp"
#include "container/SipPacketQueue.hpp"
#include "container/RtpSinkMap.hpp"
#include "container/CallMap.hpp"
#include "container/CallDecodeQueue.hpp"
#include "container/SbaIncidentMap.hpp"
#include "Transaction.hpp"

using namespace std;

namespace callx {

/*
 * Constructor of the SipProcessor.
 */
SipProcessor::SipProcessor() {
	L_t<< "C'tor";
	classname = "SipProcessor";
	m_sipPacketQueue = SipPacketQueue::getInstance();
	m_callMap = CallMap::getInstance();
	m_rtpSinkMap = RtpSinkMap::getInstance();
	m_callDecodeQueue = CallDecodeQueue::getInstance();
	m_sbaIncidentMap = SbaIncidentMap::getInstance();
}

SipProcessor::~SipProcessor() {
	L_t<< "D'tor";
}

/**
 * Deactivates the SipPacketQueue object and stops the worker.
 * The worker is not able to stop if the queue was not deactivated before.The
 * overridden stop method from the parent CallxThread is called after that.
 */
bool SipProcessor::stop() {
	L_t<< "Overridden stop() has been called, deactivating SipPacketQueue.";
	m_sipPacketQueue->deactivate();
	return CallxThread::stop();
}

/**
 * Implements the worker method of the parent CallxThread.
 */
void SipProcessor::worker() {

	while (!m_stopRequested) {

		m_currCall.reset();
		m_currCallLock.reset();
		m_currTransaction.reset();
		m_currDialog.reset();
		m_currCallRtpSinkMap.reset();

		// get next SipPacket
		if (!m_sipPacketQueue->waitAndPop(m_currPacket)) {
			// We are here, because the SipPacketQueue has been deactivated.
			L_t<< "wait_and_pop() results false -> SipPacketQueue has been "
			<< "deactivated.";
			L_t
			<< "m_stopRequested: "
			<< m_stopRequested;
			continue;
		}

		// parsing SIP packet
		if (!m_currPacket->parse()) {
			L_i<< "Parsing SIP packet failed, discarding packet.";
			continue;
		}

		// ------------------------------------------------
		// CALL HANDLING                                -->
		// ------------------------------------------------

		// Searching Call object in CallMap
		if (m_callMap->find(m_currPacket->getCallId(), m_currCall)) {

			// Call exists.
			L_t<< "Found call in CallMap.";
			m_currCallLock = m_currCall->getUniqueLock();
			m_currCall->updateActivityTs();
			m_currDialog = m_currCall->getDialog();

		} else {

			// Call does not exist.
			L_t
			<< "Call not in CallMap.";

			if (m_currPacket->getMessageType() == mt_REQUEST
					&& (m_currPacket->getRequestMethod() == rm_INVITE
							|| m_currPacket->getRequestMethod() == rm_OPTIONS)) {
				L_t
				<< "Request is "
				<< requestMethodEnumToString(m_currPacket
						->getRequestMethod());

				// create new call
				L_i
				<< "Creating and inserting Call object into CallMap.";
				L_i
				<< "Call ID: "
				<< m_currPacket->getCallId();
				m_currCall = make_shared<Call>(m_currPacket->getCallId());
				if (m_callxConfig->record_if_incident_only
						&& !callerHasIncident(m_currPacket->getFrom().address)) {
					L_t
					<< "record_if_incident_only but the caller has no incident "
					<< "---> m_currCall->setFlagNoRtp()";
					m_currCall->setFlagNoRtp();
				}
				m_currDialog = m_currCall->getDialog();
				m_currCallLock = m_currCall->getUniqueLock();
				m_callMap->insert(make_pair(m_currPacket->getCallId(),
								m_currCall));
			} else {
				L_i
				<< "Discarding SIP Message. Call ID: "
				<< m_currPacket->getCallId();
				continue;
			}
		}

		// We are sure having a Call object. Get a reference to the local
		// RtpSinkMap object.
		m_currCallRtpSinkMap = m_currCall->getLocalRtpSinkMap();

		// ------------------------------------------------
		// TRANSACTION HANDLING                         -->
		// ------------------------------------------------

		// What about the transaction?
		// Matches if Branch and cSeqMethod are equal and also matches
		// if we have an ACK on NON-SUCCESS
		if (m_currCall->findTransaction( { m_currPacket->getBranch(),
				m_currPacket->getCseqMethod() }, m_currTransaction)
				|| (m_currPacket->getCseqMethod() == rm_ACK
						&& m_currCall->findTransaction(
								{ m_currPacket->getBranch(), rm_INVITE },
								m_currTransaction))) {

			L_t<< "Found transaction.";
			L_t
			<< "Current state is (client / server): "
			<< transactionState[m_currTransaction->getState().first]
			<< " / "
			<< transactionState[m_currTransaction->getState().second];

			// There is a Transaction matching the SIP packet.
			m_currTransaction->updateActivityClockstamp();

			if (m_currPacket->getMessageType() == mt_REQUEST) {
				handleRequestWithinTransaction();
				continue;
			} else if (m_currPacket->getMessageType() == mt_RESPONSE) {
				handleResponse();
				continue;
			}

		} else { // No matching transaction.

			L_t
			<< "Not matching an ongoing transaction.";

			// Transaction does not exist.
			if (m_currPacket->getMessageType() == mt_REQUEST) {

				// Handle requests that are initiating a transaction.
				L_t
				<< "Initial request.";
				handleInitialRequest();
				continue;

			} else if (m_currPacket->getMessageType() == mt_RESPONSE) {
				L_i
				<< "Discarding SIP Message. Call ID: "
				<< m_currPacket->getCallId();
				continue;
			}
		}
	}
	L_t
	<< "Stopped the worker because stop request is true.";
	m_stopped = true;
}

void SipProcessor::handleInitialRequest() {

	// Create Transaction object.
	m_currTransaction = make_shared<Transaction>(m_currCall, m_currPacket);

	// These requests initiate a Transaction.
	switch (m_currPacket->getRequestMethod()) {
	case rm_INVITE:     // handle INVITE
		handleInvite();
		break;
	case rm_CANCEL:     // handle CANCEL
		handleCancel();
		break;
	case rm_ACK:       // handle ACK (belongs to 2xx-response on INVITE request)
		handleAckOnSuccess();
		break;
	case rm_BYE:        // handle BYE
		handleBye();
		break;
	case rm_OPTIONS:    // handle OPTIONS
		handleOptions();
		break;
	default:            // other requests are not handled here
		L_t"Nothing to do.";
		break;
	}
}

void SipProcessor::handleRequestWithinTransaction() {

	// There requests are terminating a Transaction.
	switch (m_currPacket->getRequestMethod()) {
	case rm_ACK:        // handle ACK after non-2xx response
		handleAckOnNonSuccess();
		break;
	default:            // other requests are not handled here
		break;
	}
}

void SipProcessor::handleResponse() {

	// Dispatch categories.
	switch (m_currPacket->getResponseCategory()) {
	case rc_PROVISIONAL:    // handle 1xx response
		handleProvisional();
		break;
	case rc_SUCCESS:        // handle 2xx response
		handleSuccess();
		break;
	default:                // other responses
		handleNonSuccess();
		break;
	}
}

void SipProcessor::handleInvite() {

	L_t<< "Handling INVITE request.";

	// return if dialog state is TERMINATING
	if (m_currDialog->state == ds_TERMINATING) {
		L_i
		<< "Error: INVITE request within dead call: dialog state is "
		<< "TERMINATING";
		return;
	}

	// Break if it is a nested INVITE request (other INVITE transaction pending).
	if (m_currDialog->state == ds_INIT || m_currDialog->state == ds_EARLY
			|| m_currCall->getReinviteFlag()) {
		L_i
		<< "Error: Nested INVITE request found. Discarding SIP packet. "
		"Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// We have a re-INVITE request, if the dialog state is confirmed. It is a
	// new call otherwise.
	if (m_currDialog->state == ds_CONFIRMED) {
		m_currCall->setReinviteFlag();
	} else {

		// set dialog state
		m_currDialog->state = ds_INIT;

		// fill in caller & callee information in Dialog
		m_currDialog->caller = m_currPacket->getFrom();
		m_currDialog->callee = m_currPacket->getTo();
	}

	// set transaction state
	m_currTransaction->setState( {ts_CALLING, ts_PROCEEDING});

	// insert transaction into TransactionMap object
	m_currCall->insertTransaction(m_currTransaction);
	m_currDialog->state = ds_INIT;

	// Handle session description (create RTP sinks).
	processSdp();

	// Create SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();
}

void SipProcessor::handleBye() {

	// set initial state of transaction
	m_currTransaction->setState( { ts_TRYING, ts_TRYING });

	// insert transaction into TransactionMap object
	m_currCall->insertTransaction(m_currTransaction);

	// Create SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();
}

void SipProcessor::handleOptions() {

	// set initial state of transaction
	m_currTransaction->setState( { ts_TRYING, ts_TRYING });

	// insert Transaction into TransactionMap object
	m_currCall->insertTransaction(m_currTransaction);

	// Create SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();
}

void SipProcessor::handleCancel() {

	// Searching INVITE transaction to be canceled.

	// Generate the TransactionId of the INVITE transaction that is requested
	// to be canceled (same branch + request method INVITE)
	TransactionId inviteTransactionId = { m_currTransaction->getId().first,
			rm_INVITE };

	// search the INVITE transaction
	shared_ptr<Transaction> inviteTransaction;
	if (!m_currCall->findTransaction(inviteTransactionId, inviteTransaction)) {

		// CANCEL request does not match an INVITE transaction
		L_t<< "Transaction error: CANCEL request does not match an "
		"INVITE transaction. Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// found INVITE transaction
	L_t<< "Found the INVITE transaction to be canceled.";

	// check SentBy
	if (m_currTransaction->getSentBy() != inviteTransaction->getSentBy()) {
		// SentBy does not match
		L_t<< "SentBy value of the CANCEL request does not match the one in "
		<< "the initial INVITE request. Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// check CSeq number
	if (inviteTransaction->getCseqNum() != m_currTransaction->getCseqNum()) {
		L_t<< "CSeq Number in CANCEL transaction does not match the "
		<< "INVITE transaction. Call ID: "
		<< m_currPacket->getCallId();
	}

	// setting initial state of the CANCEL transaction
	m_currTransaction->setState( { ts_TRYING, ts_TRYING });

	// insert CANCEL transaction into TransactionMap object
	m_currCall->insertTransaction(m_currTransaction);

	// Create CANCEL event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// inform the INVITE Transaction
	inviteTransaction->setCancelFlag();

	// Create SBA event (of the INVITE Transaction) and push it into
	// the caller specific list in the SbaEventMap.
	inviteTransaction->createAndPushEvent();
}

void SipProcessor::handleAckOnNonSuccess() {

	// We had a non-2xx response on an INVITE request before. So the ACK
	// is sent within the INVITE transaction.
	if (!matchState( { { ts_TERMINATED, ts_COMPLETED } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "TERMINATED / COMPLETED";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update transaction (that's the INVITE Transaction object)
	m_currTransaction->setState( {ts_TERMINATED, ts_TERMINATED});
	m_currTransaction->setAckFlag();

	// Create SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// Delete Call from CallMap if we have no reINVITE.
	if (!m_currCall->getReinviteFlag()) {

		m_currDialog->state = ds_TERMINATING;

		// delete Call from CallMap
		deleteCallFromCallMap(m_currPacket->getCallId());
	}

	// delete RTP sinks
	annulRtpSinks();

	// delete the transaction (the INVITE transaction)
	m_currCall->delTransaction(m_currTransaction->getId());
}

void SipProcessor::handleAckOnSuccess() {

	// ACK after 2xx response on an INVITE request. We have a new (fleeting)
	// transaction. Searching INVITE transaction to be acked.
	shared_ptr<Transaction> inviteTransaction;
	if (!m_currCall->findInviteTransaction(inviteTransaction)) {

		// ACK request does not match an INVITE transaction
		L_t<< "Transaction error: ACK request does not match an INVITE "
		<< "transaction. Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// check SentBy (part of Via header)
	if (m_currTransaction->getSentBy() != inviteTransaction->getSentBy()) {
		// SentBy does not match
		L_t<< "SentBy of the ACK request does not match the one in "
		<< "the initial INVITE request. Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// check CSeq number
	if (inviteTransaction->getCseqNum() != m_currTransaction->getCseqNum()) {
		L_t<< "CSeq Number in ACK transaction does not match the "
		<< "INVITE transaction. Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// found INVITE transaction
	L_t<< "Found the INVITE transaction to this ACK request.";

	// inform the INVITE Transaction
	inviteTransaction->setAckFlag();

	// Create SBA event of the INVITE Transaction and push it into
	// the caller specific list in the SbaEventMap.
	inviteTransaction->createAndPushEvent();

	// update Transaction - that's the ACK Transaction object itself, not the
	// INVITE Transaction object
	m_currTransaction->setState( { ts_TERMINATED, ts_TERMINATED });

	// Handle session description (create RTP sinks).
	processSdp();

	// 3-way handshake is over, confirm RtpSink objects
	confirmRtpSinks();

	// delete the INVITE transaction
	m_currCall->delTransaction(inviteTransaction->getId());

	// delete the ACK transaction
	m_currCall->delTransaction(m_currTransaction->getId());
}

void SipProcessor::handleProvisional() {

	switch (m_currTransaction->getCseqMethod()) {
	case rm_INVITE:
		handleProvisionalOnInvite();
		break;
	default:
		L_t"Nothing to do.";
		break;
	}
}

void SipProcessor::handleProvisionalOnInvite() {

	// match transaction state
	if (!matchState( { { ts_CALLING, ts_PROCEEDING }, { ts_PROCEEDING,
			ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "CALLING / PROCEEDING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update Transaction
	m_currTransaction->setState( {ts_PROCEEDING, ts_PROCEEDING});
	if (m_currPacket->getResponseCode() > 100) {

		// Add callee information to the Dialog.
		m_currDialog->callee.tag = m_currPacket->getTo().tag;
		m_currDialog->callee.displayname = m_currPacket->getTo().displayname;

		// Early dialog if tag in the to-field is not empty.
		if (m_currDialog->callee.tag != "") {
			m_currDialog->state = ds_EARLY;
		}
	}
}

void SipProcessor::handleProvisionalOnNonInvite() {

	if (!matchState( { { ts_TRYING, ts_TRYING },
			{ ts_PROCEEDING, ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "TRYING / TRYING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}
	m_currTransaction->setState( {ts_PROCEEDING, ts_PROCEEDING});
}

void SipProcessor::handleSuccess() {

	switch (m_currTransaction->getCseqMethod()) {
	case rm_INVITE:
		handleSuccessOnInvite();
		break;
	case rm_BYE:
		handleResponseOnBye();
		break;
	case rm_OPTIONS:
		handleResponseOnOptions();
		break;
	case rm_CANCEL:
		handleResponseOnCancel();
		break;
	default:
		L_t"Nothing to do.";
		break;
	}
}

void SipProcessor::handleNonSuccess() {

	switch (m_currTransaction->getCseqMethod()) {
	case rm_INVITE:
		handleNonSuccessOnInvite();
		break;
	case rm_BYE:
		handleResponseOnBye();
		break;
	case rm_OPTIONS:
		handleResponseOnOptions();
		break;
	case rm_CANCEL:
		handleResponseOnCancel();
		break;
	default:
		L_t"Nothing to do.";
		break;
	}
}

void SipProcessor::handleSuccessOnInvite() {

	// match transaction state
	if (!matchState( { { ts_CALLING, ts_PROCEEDING }, { ts_PROCEEDING,
			ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "CALLING / PROCEEDING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// set Transaction state
	m_currTransaction->setState( {ts_TERMINATED, ts_TERMINATED});

	// set Dialog state
	m_currDialog->state = ds_CONFIRMED;

	// update Transaction final response
	m_currTransaction->setFinalResponseCode(m_currPacket->getResponseCode());
	m_currTransaction->setFinalResponseReason(m_currPacket->getResponseReason());

	// Add callee information in Dialog, that is not available in initial
	// INVITE request.
	m_currDialog->callee.tag = m_currPacket->getTo().tag;
	m_currDialog->callee.displayname = m_currPacket->getTo().displayname;

	// Create INVITE SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// Handle session description (create RtpSink objects).
	processSdp();

	// Reset reinvite flag. This has no impact if it hasen't been
	// a re-INVITE transaction.
	m_currCall->unsetReinviteFlag();

	// The transaction will be deleted after handling the ACK request!
	// (success on invite)
}

void SipProcessor::handleNonSuccessOnInvite() {

	// match transaction state
	if (!matchState( { { ts_CALLING, ts_PROCEEDING }, { ts_PROCEEDING,
			ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "CALLING / PROCEEDING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update Transaction
	m_currTransaction->setState( {ts_TERMINATED, ts_COMPLETED});
	m_currTransaction->setFinalResponseCode(m_currPacket->getResponseCode());
	m_currTransaction->setFinalResponseReason(m_currPacket->getResponseReason());

	// Create INVITE SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// revert creation of RtpSink objects
	annulRtpSinks();

	// A possible re-INVITE flag should not be deleted here. The Information
	// about re-INVITE is essential in handleAckOnNonSuccess().
}

void SipProcessor::handleResponseOnBye() {

	if (!matchState( { { ts_TRYING, ts_TRYING },
			{ ts_PROCEEDING, ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "TRYING / TRYING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update Transaction
	m_currTransaction->setState( {ts_TERMINATED, ts_TERMINATED});
	m_currTransaction->setFinalResponseCode(m_currPacket->getResponseCode());
	m_currTransaction->setFinalResponseReason(m_currPacket->getResponseReason());

	// Create BYE SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// delete the transaction
	m_currCall->delTransaction(m_currTransaction->getId());

	// decode
	prepareCallToDecode(m_currPacket->getCallId());
}

void SipProcessor::handleResponseOnOptions() {

	if (!matchState( { { ts_TRYING, ts_TRYING },
			{ ts_PROCEEDING, ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "TRYING / TRYING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update Transaction
	m_currTransaction->setState( {ts_TERMINATED, ts_TERMINATED});
	m_currTransaction->setFinalResponseCode(m_currPacket->getResponseCode());
	m_currTransaction->setFinalResponseReason(m_currPacket->getResponseReason());

	// Create OPTIONS SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// delete the transaction
	m_currCall->delTransaction(m_currTransaction->getId());
}

void SipProcessor::handleResponseOnCancel() {

	if (!matchState( { { ts_TRYING, ts_TRYING },
			{ ts_PROCEEDING, ts_PROCEEDING } })) {
		L_t<< "Transaction error. State is not (client / server): "
		<< "TRYING / TRYING or PROCEEDING / PROCEEDING.";
		L_t
		<< "Call ID: "
		<< m_currPacket->getCallId();
		return;
	}

	// update Transaction
	m_currTransaction->setState( {ts_TERMINATED, ts_TERMINATED});
	m_currTransaction->setFinalResponseCode(m_currPacket->getResponseCode());
	m_currTransaction->setFinalResponseReason(m_currPacket->getResponseReason());

	// Create CANCEL SBA event and push it into the caller specific list
	// in the SbaEventMap.
	m_currTransaction->createAndPushEvent();

	// delete the transaction
	m_currCall->delTransaction(m_currTransaction->getId());
}

void SipProcessor::processSdp() {

	// Return if nothing to do.
	if (!m_currPacket->hasSdpPayload()) {
		return;
	}

	L_t<< "Processing Session Description.";

	// temp RTP sink
	shared_ptr<RtpSink> rtpSink;

	SocketAddress rtpSocket = m_currPacket->getSdpMediaSocket();

	// is re-INVITE? test if RTP sink already exists
	if (m_currCall->getReinviteFlag()) {
		if (m_rtpSinkMap->find(rtpSocket, rtpSink)) {
			if (rtpSink->getStatus() == rs_CONFIRMED) {
				rtpSink->setStatus(rs_CONFIRMED_INIT);
				L_t<< "RtpSink does exist. Changing state from CONFIRMED to CONFIRMED_INIT.";
			} else {
				L_i
				<< "Error while creating RtpSink object in re-INVITE "
				<< "transaction. RtpSink exists with status not equal "
				<< "to CONFIRMED. Status is "
				<< rtpSinkStatusEnumToString(rtpSink->getStatus())
				<< ", Call ID: "
				<< m_currPacket->getCallId();
			}

			// RtpSink object already exists (no change in socket address).
			return;
		}
	}

	// Create and insert RtpSink.

	// RTP socket in SIP response is caller audio
	// Reinvite can be different, see below.
	bool isCallerSink;
	if (m_currPacket->getMessageType() == mt_RESPONSE)
		isCallerSink = true;
	else
		isCallerSink = false;

	// reinvite of the callee toggles isCallerSink variable
	if (m_currCall->getDialog()->caller.tag == m_currPacket->getTo().tag)
		isCallerSink = !isCallerSink;

	rtpSink = make_shared<RtpSink>(isCallerSink);

	// If the call has exceeded max_recording_time or (!)
	// if record_if_incident_only is true (config) and there is no incident
	// for this user, the new rtpSink has to be deactivated.
	if (!m_currCall->rtpSinksAreActivated()) {
		L_t<< "Deactivate RtpSink because Call object flag"
		<< "'rtpSinksAreActivated' is false.";
		rtpSink->deactivate();
	}

	// Insert sink into local and into global RtpSinkMap.
	m_currCall->getLocalRtpSinkMap()->insert( { rtpSocket, rtpSink });
	L_t<< "Inserting RtpSink Object into local and into global RtpSinkMap.";
	m_rtpSinkMap->insert( { rtpSocket, rtpSink });
	L_t<< "Size of local RtpSinkMap is now: "
	<< m_currCall->getLocalRtpSinkMap()->size();
}

void SipProcessor::confirmRtpSinks() {
	for (auto iter = m_currCallRtpSinkMap->begin();
			iter != m_currCallRtpSinkMap->end(); iter++) {

		if (iter->second->getStatus() == rs_CONFIRMED) {

			// Set state TERMINATED and delete RtpSink reference from global
			// RtpSinkMap if state is CONFIRMED.
			iter->second->setStatus(rs_TERMINATED);
			m_rtpSinkMap->erase(iter->first);

		} else if (iter->second->getStatus() == rs_INIT
				|| iter->second->getStatus() == rs_CONFIRMED_INIT) {

			// Set state to CONFIRMED if state is INIT or CONFIRMED_INIT.
			iter->second->setStatus(rs_CONFIRMED);
		}
	}
}

void SipProcessor::annulRtpSinks() {
	for (auto iter = m_currCallRtpSinkMap->begin();
			iter != m_currCallRtpSinkMap->end(); iter++) {

		if (iter->second->getStatus() == rs_INIT) {

			// Set state TERMINATED and delete RtpSink reference from global
			// RtpSinkMap if state is INIT.
			iter->second->setStatus(rs_TERMINATED);
			m_rtpSinkMap->erase(iter->first);

		} else if (iter->second->getStatus() == rs_CONFIRMED_INIT) {

			// Set state to CONFIRMED_INIT if state is CONFIRMED_INIT.
			iter->second->setStatus(rs_CONFIRMED);
		}
	}
}

bool SipProcessor::matchState(std::initializer_list<TransactionState> states) {
	for (auto iter = states.begin(); iter != states.end(); iter++) {
		if (*iter == m_currTransaction->getState())
			return true;
	}
	return false;
}

void SipProcessor::deleteCallFromCallMap(const string& callId) {

	// delete the call
	size_t count = m_callMap->erase(callId);
	if (count == 0) {
		L_e<< "Deleting Call reference from CallMap failed. "
		<< "Call not found. Call ID: "
		<< callId;
	} else {
		L_i
		<< "Call reference deleted from CallMap.";
		L_i
		<< "Call ID: "
		<< callId;
	}
}

void SipProcessor::prepareCallToDecode(const string& callId) {
	m_currCall->unregisterRtpSinks();
	deleteCallFromCallMap(callId);
	m_callDecodeQueue->push(move(m_currCall));
}

bool SipProcessor::callerHasIncident(const std::string &caller) {
	std::shared_ptr<SbaIncidentSet> sbaIncidentSet;

	if (m_sbaIncidentMap->find(caller, sbaIncidentSet)) {
		return sbaIncidentSet->size();
	}
	return false;
}

} /* namespace callx */

