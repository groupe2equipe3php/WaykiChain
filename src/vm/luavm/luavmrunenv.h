// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2017-2019 The WaykiChain Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef LUA_VM_RUNENV_H
#define LUA_VM_RUNENV_H

#include "luavm.h"
#include "appaccount.h"
#include "commons/serialize.h"
#include "entities/account.h"
#include "persistence/leveldbwrapper.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"
#include "json/json_spirit_writer_template.h"

#include <memory>

using namespace std;
class CVmOperate;

class CLuaVMRunEnv {
private:
	/**
	 * Run the script object
	 */
	std::shared_ptr<CLuaVM> pLua;
	/**
	 * vm before the account state
	 */
	vector<std::shared_ptr<CAccount> > rawAccount;
	/**
	 * vm operate the account state
	 */
	vector<std::shared_ptr<CAccount> > newAccount;
	/**
	 * current run the tx
	 */
	std::shared_ptr<CBaseTx> pBaseTx;
	/**
	 * the block height
	 */
	uint32_t runtimeHeight;
	/**
	 * vm before the app account state
	 */
	vector<std::shared_ptr<CAppUserAccount>> rawAppUserAccount;
	/**
	 * vm operate the app account state
	 */
	vector<std::shared_ptr<CAppUserAccount>> newAppUserAccount;

	CAccountDBCache *pAccountCache;
	CContractDBCache *pContractCache;
    CTxReceiptDBCache *pTxReceiptCache;

	vector<CVmOperate> vmOperateOutput;   //保存操作结果
    bool isCheckAccount;  //校验账户平衡开关

    map<vector<uint8_t>, vector<CAppFundOperate>> mapAppFundOperate;  // vector<unsigned char > 存的是accountId
private:
    /**
     * @brief The initialization function
     * @param Tx: run the tx's contact
     * @param accountCache: Cache holds account
     * @param contractCache: Cache holds contract
     * @param height: run the Environment the block's height
     * @return : check the the tx and account is Legal true is legal false is illegal
     */
    bool Initialize(std::shared_ptr<CBaseTx>& tx, CAccountDBCache& accountCache, CContractDBCache& contractCache,
                    CTxReceiptDBCache& txReceiptCache, int32_t height);
    /**
     * @brief check action
     * @param operates: run the script return the code,check the code
     * @return : true check success
     */
    bool CheckOperate(const vector<CVmOperate>& operates);
    /**
     *
     * @param operates: through the vm return code, The accounts add/minus money
     * @param accountCache:
     * @param contractCache
     * @return true operate account success
     */
    bool OperateAccount(const vector<CVmOperate>& operates);
    /**
     * @brief find the vOldAccount from newAccount if find success remove it from newAccount
     * @param vOldAccount: the argument
     * @return:Return the object
     */
    std::shared_ptr<CAccount> GetNewAccount(std::shared_ptr<CAccount>& vOldAccount);
    /**
     * @brief find the account from newAccount
     * @param account: argument
     * @return:Return the object
     */
    std::shared_ptr<CAccount> GetAccount(std::shared_ptr<CAccount>& account);
    /**
     * @brief get the account id
     * @param value: argument
     * @return:Return account id
     */
    UnsignedCharArray GetAccountID(CVmOperate value);
    //	bool IsSignatureAccount(CRegID account);
    bool OperateAppAccount(const map<vector<uint8_t>, vector<CAppFundOperate>> opMap);

    std::shared_ptr<CAppUserAccount> GetAppAccount(std::shared_ptr<CAppUserAccount>& appAccount);

public:
    /**
     * A constructor.
     */
    CLuaVMRunEnv();
    virtual ~CLuaVMRunEnv();

    /**
     * @brief get be operate the account
     * @return the variable rawAccount
     */
    vector<std::shared_ptr<CAccount>>& GetRawAccont();
    /**
     * @brief get after operate the account
     * @return :the variable newAccount
     */
    vector<std::shared_ptr<CAccount>>& GetNewAccount();
    vector<std::shared_ptr<CAppUserAccount>>& GetRawAppUserAccount();
    vector<std::shared_ptr<CAppUserAccount>>& GetNewAppUserAccount();

    /**
     * @brief  start to run the script
     * @param Tx: run the tx
     * @param accountCache: the second argument
     * @param height: block height
     * @param nBurnFactor: Executing a step script to spending
     * @return: tuple<bool,uint64_t,string>  bool represent the script run success
     * uint64_t if the script run success Run the script calls the money ,string represent run the
     * failed's  Reason
     */
    std::tuple<bool, uint64_t, string> ExecuteContract(std::shared_ptr<CBaseTx>& tx, int32_t height, CCacheWrapper &cw,
                                                  uint64_t nBurnFactor, uint64_t& uRunStep);

    /**
     * @brief just for test
     * @return:
     */
    //	shared_ptr<vector<CVmOperate> > GetOperate() const;
    const CRegID& GetScriptRegID();
    const CRegID& GetTxAccount();
    uint64_t GetValue() const;
    const string& GetTxContract();
    CContractDBCache* GetScriptDB();
    CAccountDBCache* GetCatchView();
    int32_t GetConfirmHeight();
    // Get burn version for fuel burning
    int32_t GetBurnVersion();
    uint256 GetCurTxHash();
    bool InsertOutputData(const vector<CVmOperate>& source);
    void InsertOutAPPOperte(const vector<uint8_t>& userId, const CAppFundOperate& source);

    bool GetAppUserAccount(const vector<uint8_t>& id, std::shared_ptr<CAppUserAccount>& pAppUserAccount);
    bool CheckAppAcctOperate(CLuaContractInvokeTx* tx);
    void SetCheckAccount(bool bCheckAccount);
};

#endif  // LUA_VM_RUNENV_H
