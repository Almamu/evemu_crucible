/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2021 The EVEmu Team
    For the latest information visit https://evemu.dev
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        Zhur
*/


#ifndef __ACCOUNTSERVICE_H_INCL__
#define __ACCOUNTSERVICE_H_INCL__

#include "services/Service.h"
#include "account/AccountDB.h"
#include "Client.h"

class AccountService : public Service <AccountService> {
public:
    AccountService();

    // this moves currency, adds journal entries, and sends blink event. handles applicable corp taxes internally.
    // will throw if fails
    static void TransferFunds(
        uint32 fromID,
        uint32 toID,
        double amount,
        std::string reason = "",
        uint8 entryTypeID = Journal::EntryType::Undefined,
        uint32 referenceID = 0,
        uint16 fromKey = Account::KeyType::Cash,
        uint16 toKey = Account::KeyType::Cash,
        Client* pClient=nullptr
    );

    // this should be the ONLY method to alter corp balances, and ONLY called from TransferFunds()
    static void HandleCorpTransaction(
        uint32 corpID,
        int8 entryTypeID,
        uint32 fromID,
        uint32 toID,
        int8 currency,
        uint16 accountKey,
        double amount,
        std::string description,
        uint32 referenceID = 0
    );

protected:
    AccountDB m_db;

    EVEResult GetKeyMap(EVECallArgs& call);
    EVEResult GetEntryTypes(EVECallArgs& call);
    EVEResult GetWalletDivisionsInfo(EVECallArgs& call);
    EVEResult GetDefaultContactCost(EVECallArgs& call);
    EVEResult SetContactCost(EVECallArgs& call, std::optional<PyInt*> cost);
    EVEResult GetCashBalance(EVECallArgs& call, std::optional<PyBool*> isCorpWallet, std::optional<PyInt*> walletKey);
    EVEResult GetCashBalance(EVECallArgs& call, std::optional<PyInt*> isCorpWallet, std::optional<PyInt*> walletKey);
    EVEResult GetJournal(EVECallArgs& call, PyInt* accountKey, PyInt* fromDate, std::optional<PyInt*> entryTypeID, PyInt* corpAccount, std::optional <PyInt*> transactionID, std::optional<PyInt*> rev);
    EVEResult GetJournal(EVECallArgs& call, PyInt* accountKey, PyInt* fromDate, std::optional<PyInt*> entryTypeID, PyBool* corpAccount, std::optional <PyInt*> transactionID, std::optional<PyInt*> rev);
    EVEResult GetJournalForAccounts(EVECallArgs& call, PyInt* accountKeys, PyInt* fromDate, std::optional<PyInt*> entryTypeID, PyBool* corpAccount, std::optional <PyInt*> transactionID, std::optional<PyInt*> rev);
    EVEResult GetJournalForAccounts(EVECallArgs& call, PyInt* accountKeys, PyInt* fromDate, std::optional<PyInt*> entryTypeID, PyInt* corpAccount, std::optional <PyInt*> transactionID, std::optional<PyInt*> rev);
    EVEResult GiveCash(EVECallArgs& call, PyInt* toID, PyInt* amount, std::optional <PyString*> reason);
    EVEResult GiveCash(EVECallArgs& call, PyInt* toID, PyFloat* amount, std::optional <PyString*> reason);
    EVEResult GiveCash(EVECallArgs&call, PyInt* toID, PyFloat* amount, std::string reason);
    EVEResult GiveCashFromCorpAccount(EVECallArgs& call, PyInt* toID, PyInt* amount, PyInt* fromAcctKey);
    EVEResult GiveCashFromCorpAccount(EVECallArgs& call, PyInt* toID, PyFloat* amount, PyInt* fromAcctKey);
};

#endif
