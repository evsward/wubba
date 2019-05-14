/**
 * Created by ujInfo on 19/5/3.
 */
const baseTr = require('../../../libs/EOS/baseTr');

function importToData(){
    const import12DataDS={
        tableId:'', 
        tableStatus:'', 
        cardBoot:'', 
        dealer:'', 
        trusteeship:'', 
        isPrivate:'', 
        dealerBalance:'', 
        oneRoundMaxTotalBet_BP:'', 
        minPerBet_BP:'', 
        oneRoundMaxTotalBet_Tie:'', 
        minPerBet_Tie:'', 
        oneRoundMaxTotalBet_Push:'', 
        minPerBet_Push:'', 
        oneRoundDealerMaxPay:'', 
        minTableDeposit:'', 
        commission_rate_agent:'', 
        commission_rate_player:'', 
        upgradingFlag:'', 
        amountSymbol:'', 
        validCardVec:''
    };
    return import12DataDS
};


function commbacImport12Data(){
    const _commbacImport12Data=new baseTr();
    return _commbacImport12Data;
}

module.exports = {commbacImport12Data,importToData}