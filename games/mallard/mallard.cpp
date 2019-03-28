#include "mallard.hpp"

ACTION mallard::newtable(name dealer, asset deposit, bool isPrivate, name code, string sym, string commission_rate_agent, string commission_rate_player, asset oneRoundMaxTotalBet_bp, asset minPerBet_bp,
                         asset oneRoundMaxTotalBet_tie, asset minPerBet_tie,
                         asset oneRoundMaxTotalBet_push, asset minPerBet_push)
{
    require_auth(dealer);

    bool symbol_exist_flag = false; // flag if user symbol(code,sym) is including in sysconfig(symOptions).
    asset minPerBet_default_temp;
    for (auto p : mallard::symOptions)
    {
        if (p.code == code && 0 == p.symName.code().to_string().compare(sym))
        { // found, exist
            symbol_exist_flag = true;
            minPerBet_default_temp = p.minPerBet_default;
        }
    }
    extended_symbol cur_ex_sym = defaultSym;
    if (symbol_exist_flag)
    {
        cur_ex_sym = extended_symbol(symbol(symbol_code(sym), 4), code);
    }

    asset init_asset_empty = asset(0, cur_ex_sym.get_symbol());

    eosio_assert(oneRoundMaxTotalBet_bp > init_asset_empty && minPerBet_bp > minPerBet_default_temp && oneRoundMaxTotalBet_tie > init_asset_empty && minPerBet_tie > minPerBet_default_temp && oneRoundMaxTotalBet_push > init_asset_empty && minPerBet_push > minPerBet_default_temp, "max bet amount is < 0 || min bet amount < minPerBet_default_temp!");

    //auto temp_rate_platform = Round(comission_rate_platform_default, 4);
    auto temp_rate_agent = Atof(commission_rate_agent.c_str());
    temp_rate_agent = Round(temp_rate_agent, 4);

    auto temp_rate_player = Atof(commission_rate_player.c_str());
    temp_rate_player = Round(temp_rate_player, 4);

    eosio::print(" temp_rate_platform:", comission_rate_platform_default, " temp_rate_agent:", temp_rate_agent, " temp_rate_player", temp_rate_player);

    asset oneRoundDealerMaxPay_temp = oneRoundMaxTotalBet_push * 11 * 2 + max(oneRoundMaxTotalBet_bp * 1, oneRoundMaxTotalBet_tie * 8);
    eosio::print(" before====oneRoundDealerMaxPay_temp:", oneRoundDealerMaxPay_temp);
    oneRoundDealerMaxPay_temp += (oneRoundMaxTotalBet_tie + oneRoundMaxTotalBet_bp + oneRoundMaxTotalBet_push)*(comission_rate_platform_default + temp_rate_agent + temp_rate_player);
    eosio::print(" end====oneRoundDealerMaxPay_temp:", oneRoundDealerMaxPay_temp);
    asset deposit_tmp = oneRoundDealerMaxPay_temp * minTableRounds;

    eosio_assert(deposit >= deposit_tmp, "Table deposit is not enough!");
    INLINE_ACTION_SENDER(eosio::token, transfer)
    (
        cur_ex_sym.get_contract(), {{dealer, "active"_n}},
        {dealer, _self, deposit, std::string("new:tabledeposit")});
    // table init.
    std::vector<uint16_t> validCardVec_empty;
    uint64_t tableId_temp;
    tableround.emplace(_self, [&](auto &s) {
        s.tableId = tableround.available_primary_key();
        tableId_temp = s.tableId;
        s.cardBoot = 1;
        s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_SHUFFLE;
        s.dealer = dealer;
        s.dealerBalance = deposit;
        s.isPrivate = isPrivate;
        s.validCardVec = validCardVec_empty;
        s.oneRoundMaxTotalBet_BP = oneRoundMaxTotalBet_bp;
        s.minPerBet_BP = minPerBet_bp;
        s.oneRoundMaxTotalBet_Tie = oneRoundMaxTotalBet_tie;
        s.minPerBet_Tie = minPerBet_tie;
        s.oneRoundMaxTotalBet_Push = oneRoundMaxTotalBet_push;
        s.minPerBet_Push = minPerBet_push;
        s.oneRoundDealerMaxPay = oneRoundDealerMaxPay_temp;
        s.minTableDeposit = deposit_tmp;
        s.amountSymbol = cur_ex_sym;
        s.commission_rate_agent = temp_rate_agent;
        s.commission_rate_player = temp_rate_player;
    });
}

ACTION mallard::edittable(uint64_t tableId, bool isPrivate, name code, string sym, string commission_rate_agent, string commission_rate_player, asset oneRoundMaxTotalBet_bp, asset minPerBet_bp,asset oneRoundMaxTotalBet_tie, asset minPerBet_tie,asset oneRoundMaxTotalBet_push, asset minPerBet_push)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "The currenct round isn't end!");

    bool symbol_exist_flag = false; // flag if user symbol(code,sym) is including in sysconfig(symOptions).
    asset minPerBet_default_temp;
    for (auto p : mallard::symOptions)
    {
        if (p.code == code && 0 == p.symName.code().to_string().compare(sym))
        { // found, exist
            symbol_exist_flag = true;
            minPerBet_default_temp = p.minPerBet_default;
        }
    }
    extended_symbol cur_ex_sym = defaultSym;
    if (symbol_exist_flag)
    {
        cur_ex_sym = extended_symbol(symbol(symbol_code(sym), 4), code);
    }

    asset init_asset_empty = asset(0, cur_ex_sym.get_symbol());

    eosio_assert(oneRoundMaxTotalBet_bp > init_asset_empty && minPerBet_bp > minPerBet_default_temp && oneRoundMaxTotalBet_tie > init_asset_empty && minPerBet_tie > minPerBet_default_temp && oneRoundMaxTotalBet_push > init_asset_empty && minPerBet_push > minPerBet_default_temp, "max bet amount is < 0 || min bet amount < minPerBet_default_temp!");

    auto temp_rate_platform = Round(comission_rate_platform_default, 4);
    auto temp_rate_agent = Atof(commission_rate_agent.c_str());
    temp_rate_agent = Round(temp_rate_agent, 4);

    auto temp_rate_player = Atof(commission_rate_player.c_str());
    temp_rate_player = Round(temp_rate_player, 4);

    asset oneRoundDealerMaxPay_temp = oneRoundMaxTotalBet_push * 11 * 2 + max(oneRoundMaxTotalBet_bp * 1, oneRoundMaxTotalBet_tie * 8);
    oneRoundDealerMaxPay_temp += (oneRoundMaxTotalBet_tie + oneRoundMaxTotalBet_bp + oneRoundMaxTotalBet_push)*(temp_rate_platform + temp_rate_agent + temp_rate_player);
    asset deposit_tmp = oneRoundDealerMaxPay_temp * minTableRounds;

    tableround.modify(existing, _self, [&](auto &s) {
        s.isPrivate = isPrivate;
        s.oneRoundMaxTotalBet_BP = oneRoundMaxTotalBet_bp;
        s.minPerBet_BP = minPerBet_bp;
        s.oneRoundMaxTotalBet_Tie = oneRoundMaxTotalBet_tie;
        s.minPerBet_Tie = minPerBet_tie;
        s.oneRoundMaxTotalBet_Push = oneRoundMaxTotalBet_push;
        s.minPerBet_Push = minPerBet_push;
        s.oneRoundDealerMaxPay = oneRoundDealerMaxPay_temp;
        s.minTableDeposit = deposit_tmp;
        s.amountSymbol = cur_ex_sym;
        s.commission_rate_agent = temp_rate_agent;
        s.commission_rate_player = temp_rate_player;
    });
}

ACTION mallard::dealerseed(uint64_t tableId, checksum256 encodeSeed)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);

    if (!existing->trusteeship)
    {
        eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END,
                     "tableStatus != end");
        require_auth(existing->dealer);
        if (existing->dealerBalance < existing->oneRoundDealerMaxPay * 2)
        {
            INLINE_ACTION_SENDER(mallard, pausetabledea)
            (
                _self, {{existing->dealer, "active"_n}},
                {existing->tableId});
            return;
        }
        // start a new round. table_round init.
        eosio::print(" before===validCardVec.size:", existing->validCardVec.size());
        checksum256 hash;
        std::vector<player_bet_info> emptyPlayers;
        std::vector<card_info> emptyCards;
        asset init_asset_empty = asset(0, existing->amountSymbol.get_symbol());
        tableround.modify(existing, _self, [&](auto &s) {
            s.betStartTime = 0;
            s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_START;
            s.currRoundBetSum_BP = init_asset_empty;
            s.currRoundBetSum_Tie = init_asset_empty;
            s.currRoundBetSum_Push = init_asset_empty;
            s.dealerSeedHash = encodeSeed;
            s.serverSeedHash = hash;
            s.dealerSeed = "";
            s.serverSeed = "";
            s.dSeedVerity = 0;
            s.sSeedVerity = 0;
            s.playerInfo = emptyPlayers;
            s.roundResult = "";
            s.playerHands = emptyCards;
            s.bankerHands = emptyCards;
        });
    }
}

ACTION mallard::serverseed(uint64_t tableId, checksum256 encodeSeed)
{
    require_auth(serveraccount);
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);

    if (existing->trusteeship)
    {
        eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "The currenct round isn't end!");
        if (existing->dealerBalance < existing->oneRoundDealerMaxPay * 2)
        {
            INLINE_ACTION_SENDER(mallard, pausetablesee)
            (
                _self, {{serveraccount, "active"_n}},
                {existing->tableId});
            return;
        }
        // start a new round. table_round init.
        eosio::print(" before===validCardVec.size:", existing->validCardVec.size());

        checksum256 hash;
        std::vector<player_bet_info> emptyPlayers;
        std::vector<card_info> emptyCards;
        asset init_asset_empty = asset(0, existing->amountSymbol.get_symbol());
        tableround.modify(existing, _self, [&](auto &s) {
            s.betStartTime = now();
            s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_BET;
            s.currRoundBetSum_BP = init_asset_empty;
            s.currRoundBetSum_Tie = init_asset_empty;
            s.currRoundBetSum_Push = init_asset_empty;
            s.dealerSeedHash = hash;
            s.serverSeedHash = encodeSeed;
            s.dealerSeed = "";
            s.serverSeed = "";
            s.dSeedVerity = 0;
            s.sSeedVerity = 0;
            s.playerInfo = emptyPlayers;
            s.roundResult = "";
            s.playerHands = emptyCards;
            s.bankerHands = emptyCards;
        });
    }
    else
    {
        eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_START, "Dealer haven't started a new round yet!");
        tableround.modify(existing, _self, [&](auto &s) {
            s.serverSeedHash = encodeSeed;
            s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_BET;
            s.betStartTime = now();
        });
    }
}

ACTION mallard::playerbet(uint64_t tableId, name player, asset betDealer, asset betPlayer, asset betTie, asset betDealerPush, asset betPlayerPush, name agent, string nickname)
{
    require_auth(player);
    require_auth(serveraccount);
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_BET, "tableStatus != bet");
    eosio_assert((now() - existing->betStartTime) < betPeriod, "Timeout, can't bet!");
    asset init_asset_empty = asset(0, existing->amountSymbol.get_symbol());
    if (betDealer > init_asset_empty)
        eosio_assert(betDealer >= existing->minPerBet_BP, "Banker bet is too small!");
    if (betPlayer > init_asset_empty)
        eosio_assert(betPlayer >= existing->minPerBet_BP, "Player bet is too small!");
    if (betTie > init_asset_empty)
        eosio_assert(betTie >= existing->minPerBet_Tie, "Tie bet is too small!");
    if (betDealerPush > init_asset_empty)
        eosio_assert(betDealerPush >= existing->minPerBet_Push, "BankerPush bet is too small!");
    if (betPlayerPush > init_asset_empty)
        eosio_assert(betPlayerPush >= existing->minPerBet_Push, "PlayerPush bet is too small!");

    asset player_amount_sum_bp = existing->currRoundBetSum_BP;
    asset player_amount_sum_tie = existing->currRoundBetSum_Tie;
    asset player_amount_sum_push = existing->currRoundBetSum_Push;

    bool flag = false;
    for (const auto &p : existing->playerInfo)
    {
        if (p.player == player)
        {
            flag = true;
            break;
        }
    }

    eosio_assert(!flag, "player have bet");
    player_amount_sum_bp += betDealer;
    player_amount_sum_bp += betPlayer;
    eosio_assert(player_amount_sum_bp < existing->oneRoundMaxTotalBet_BP, "Over the peak of total bet_bp amount of this round!");

    player_amount_sum_tie += betTie;
    eosio_assert(player_amount_sum_tie < existing->oneRoundMaxTotalBet_Tie, "Over the peak of total bet_tie amount of this round!");

    player_amount_sum_push += betDealerPush;
    player_amount_sum_push += betPlayerPush;
    eosio_assert(player_amount_sum_push < existing->oneRoundMaxTotalBet_Push, "Over the peak of total bet_push amount of this round!");

    asset depositAmount = (betDealer + betPlayer + betTie + betDealerPush + betPlayerPush);
    if (depositAmount > init_asset_empty)
    {
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (
            existing->amountSymbol.get_contract(), {{player, "active"_n}},
            {player, _self, depositAmount, std::string("playerbet")});
    }
    player_bet_info temp;
    temp.player = player;
    temp.betDealer = betDealer;
    temp.betPlayer = betPlayer;
    temp.betTie = betTie;
    temp.betDealerPush = betDealerPush;
    temp.betPlayerPush = betPlayerPush;
    temp.pBonus = init_asset_empty;
    temp.dBonus = init_asset_empty;
    temp.agent = agent;
    temp.nickname = nickname;

    asset sum_bet_amount = betDealer + betPlayer + betDealerPush + betPlayerPush + betTie;
    auto temp_rate_platform = comission_rate_platform_default;
    asset platformtotransfer = asset(sum_bet_amount.amount*comission_rate_platform_default, existing->amountSymbol.get_symbol());

    eosio::print(" sum_bet_amount:", sum_bet_amount.amount, " platformtotransfer:", platformtotransfer, " temp_rate_platform:", temp_rate_platform, " ");
    if (platformtotransfer > init_asset_empty)
    {
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (
             existing->amountSymbol.get_contract(), {{_self, "active"_n}},
             {_self, platformaccount, platformtotransfer, std::string("platform odds")});
    }

    asset agenttransfer = asset(sum_bet_amount.amount*existing->commission_rate_agent, existing->amountSymbol.get_symbol());

    eosio::print(" sum_bet_amount:", sum_bet_amount, " agenttransfer:", agenttransfer, " commission_rate_agent:", existing->commission_rate_agent, " ");
    if (agenttransfer > init_asset_empty)
    {
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (
              existing->amountSymbol.get_contract(), {{_self, "active"_n}},
              {_self, agent, agenttransfer, std::string("platform odds")});
    }
    asset playertransfer = asset(sum_bet_amount.amount*existing->commission_rate_player, existing->amountSymbol.get_symbol());
    eosio::print(" sum_bet_amount:", sum_bet_amount, " playertransfer:", playertransfer, " commission_rate_player:", existing->commission_rate_player, " ");
    if (playertransfer > init_asset_empty)
    {
        INLINE_ACTION_SENDER(eosio::token, transfer)
        (
              existing->amountSymbol.get_contract(), {{_self, "active"_n}},
              {_self, player, playertransfer, std::string("platform odds")});
    }

    tableround.modify(existing, _self, [&](auto &s) {
        s.playerInfo.emplace_back(temp);
        s.currRoundBetSum_BP = player_amount_sum_bp;
        s.currRoundBetSum_Tie = player_amount_sum_tie;
        s.currRoundBetSum_Push = player_amount_sum_push;
    });
}

// server defer action: end bet
ACTION mallard::endbet(uint64_t tableId)
{
    require_auth(serveraccount);
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_BET, "tableStatus != bet");
    uint64_t useTime = now() - existing->betStartTime;
    eosio::print("spend time : ", useTime, "s, need ", betPeriod, "s!");
    eosio_assert(useTime > betPeriod, "Bet time is not end now, wait... ");

    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_REVEAL;
    });
}

ACTION mallard::verdealeseed(uint64_t tableId, string seed)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(existing->dealer);
    if (!existing->trusteeship)
    {
        eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_REVEAL, "tableStatus != reveal");
        eosio_assert((now() - existing->betStartTime) > betPeriod, "It's not time to verify dealer seed yet.");
        assert_sha256(seed.c_str(), seed.size(), ((*existing).dealerSeedHash));
        tableround.modify(existing, _self, [&](auto &s) {
            s.dSeedVerity = true;
            s.dealerSeed = seed;
        });
    }
}
// Server push defer 3' action, once got ROUND_REVEAL.
ACTION mallard::verserveseed(uint64_t tableId, string seed)
{
    require_auth(serveraccount);
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_REVEAL, "tableStatus != reveal");
    eosio_assert((now() - existing->betStartTime) > betPeriod, "It's not time to verify server seed yet.");
    assert_sha256(seed.c_str(), seed.size(), ((*existing).serverSeedHash));
    tableround.modify(existing, _self, [&](auto &s) {
        s.sSeedVerity = true;
        s.serverSeed = seed;
    });
    // root_seed.
    string root_seed = seed;
    if (existing->trusteeship)
    {
        eosio::print("Dealer trusteeship, don't need dealer seed.");
    }
    // non-trustee server, so table_round is waiting for ACTION::mallard::dealerseed until dealer reconnect.
    // TODO Can be considered: auto trustee server until dealer reconnect and ACTION::mallard::exitruteship.
    else if (!existing->dSeedVerity)
    { // dealer disconnect notify
        INLINE_ACTION_SENDER(mallard, disconnecthi)
        (
            _self, {{serveraccount, "active"_n}},
            {existing->dealer, existing->tableId});
    }
    else if (existing->dSeedVerity)
    { // dealer online and not trusteeship
        root_seed += existing->dealerSeed;
    }
    string roundResult;
    std::vector<card_info> bankerHands;
    std::vector<card_info> playerHands;
    std::vector<uint16_t> validCardTemp;
    reveal(root_seed, existing->validCardVec, playerHands, bankerHands, roundResult, validCardTemp);

    std::vector<player_bet_info> tempPlayerVec;
    asset dealerBalance_temp = existing->dealerBalance;
    asset init_asset_empty = asset(0, existing->amountSymbol.get_symbol());
    for (auto playerBet : existing->playerInfo)
    {
        auto pBonus = init_asset_empty;
        auto dBonus = init_asset_empty;
        // Banker field
        if (roundResult[0] == '1')
            pBonus = playerBet.betDealer * (1 + 0.95);
        else
            dBonus = playerBet.betDealer;
        // Player field
        if (roundResult[1] == '1')
            pBonus += playerBet.betPlayer * (1 + 1);
        else
            dBonus += playerBet.betPlayer;
        // Tie field
        if (roundResult[2] == '1')
            pBonus += playerBet.betTie * (1 + 8);
        else
            dBonus += playerBet.betTie;
        // DealerPush field
        if (roundResult[3] == '1')
            pBonus += playerBet.betDealerPush * (1 + 11);
        else
            dBonus += playerBet.betDealerPush;
        // PlayerPush field
        if (roundResult[4] == '1')
            pBonus += playerBet.betPlayerPush * (1 + 11);
        else
            dBonus += playerBet.betPlayerPush;

        eosio::print(" [player:", playerBet.player, ", total bonus:", pBonus, "] ");

        if (pBonus > init_asset_empty)
        {
            INLINE_ACTION_SENDER(eosio::token, transfer)
            (
                existing->amountSymbol.get_contract(), {{_self, "active"_n}},
                {_self, playerBet.player, pBonus, std::string("playerbet win")});
        }
        dealerBalance_temp -= pBonus;
        dealerBalance_temp += dBonus;
        playerBet.pBonus = pBonus;
        playerBet.dBonus = dBonus;
        tempPlayerVec.emplace_back(playerBet);
    }

    uint64_t tableStatus_temp = (uint64_t)table_stats::status_fields::ROUND_END;
    if (existing->validCardVec.size() <= CardsMinLimit)
        tableStatus_temp = (uint64_t)table_stats::status_fields::ROUND_SHUFFLE;

    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = tableStatus_temp;
        s.playerHands = playerHands;
        s.bankerHands = bankerHands;
        s.validCardVec = validCardTemp;
        s.roundResult = roundResult;
        s.dealerBalance = dealerBalance_temp;
        s.playerInfo = tempPlayerVec;
    });
}

ACTION mallard::trusteeship(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "tableStatus != end");
    require_auth(existing->dealer); // dealer trustee server.
    tableround.modify(existing, _self, [&](auto &s) {
        s.trusteeship = true;
    });
}

ACTION mallard::exitruteship(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "tableStatus != end");
    require_auth(existing->dealer); // dealer exit trusteeship from server.
    tableround.modify(existing, _self, [&](auto &s) {
        s.trusteeship = false;
    });
}

ACTION mallard::disconnecthi(name informed, uint64_t tableId)
{
    require_auth(serveraccount);
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_REVEAL, "tableStatus != reveal");
    eosio_assert(existing->dealer == informed, "People informed is not the dealer of table!");
    eosio::print("SC disconnecthi has already informed :", informed.to_string());
}

ACTION mallard::erasingdata(uint64_t key)
{
    require_auth(_self);
    if (key == -1)
    {
        auto itr = tableround.begin();
        while (itr != tableround.end())
        {
            eosio::print("[Removing data: ", _self, ", condition: ", key, ", itr: ", itr->tableId, "]");
            itr = tableround.erase(itr);
        }
    }
    else if (key == -2)
    {
        auto itr = tableround.begin();
        while (itr != tableround.end())
        {
            if (itr->tableStatus == (uint64_t)table_stats::status_fields::CLOSED)
            {
                eosio::print("[Removing data: ", _self, ", condition: ", key, ", itr: ", itr->tableId, "]");
                itr = tableround.erase(itr);
            }
            else
                itr++;
        }
    }
    else
    {
        auto itr = tableround.find(key);
        eosio_assert(itr != tableround.end(), "the erase key is not existe");
        eosio::print("Removing data ", _self, ", condition: ", key, ", itr: ", itr->tableId);
        tableround.erase(itr);
    }
}

ACTION mallard::pausetabledea(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(existing->dealer); // dealer of the table permission.
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "The round isn't end, can't pause table");
    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::PAUSED;
    });
}

ACTION mallard::pausetablesee(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(serveraccount); // server permission.
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "The round isn't end, can't pause table");
    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::PAUSED;
    });
}

ACTION mallard::continuetable(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    eosio_assert(existing->dealerBalance >= existing->oneRoundDealerMaxPay * 2, "Can't recover table, dealer balance isn't enough!");
    require_auth(existing->dealer);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::PAUSED, "The tableid not paused, can`t continuetable");
    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_END;
    });
}

ACTION mallard::closetable(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(existing->dealer);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_END, "The round isn't end, can't close!");

    INLINE_ACTION_SENDER(eosio::token, transfer)
    (
        existing->amountSymbol.get_contract(), {{_self, "active"_n}},
        {_self, existing->dealer, existing->dealerBalance, std::string("closetable, withdraw all")});

    asset init_asset_empty = asset(0, existing->amountSymbol.get_symbol());
    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::CLOSED;
        s.dealerBalance = init_asset_empty;
    });
}

ACTION mallard::depositable(name dealer, uint64_t tableId, asset deposit)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(dealer);
    eosio_assert(deposit >= existing->minTableDeposit, "Table deposit is not enough!");

    INLINE_ACTION_SENDER(eosio::token, transfer)
    (
        existing->amountSymbol.get_contract(), {{dealer, "active"_n}},
        {dealer, _self, deposit, std::string("re:tabledeposit")});
    tableround.modify(existing, _self, [&](auto &s) {
        s.dealerBalance += deposit;
    });
    // automate recover the table round.
    if (existing->tableStatus == (uint64_t)table_stats::status_fields::PAUSED)
    {
        INLINE_ACTION_SENDER(mallard, continuetable)
        (
            _self, {{existing->dealer, "active"_n}},
            {existing->tableId});
    }
}

ACTION mallard::dealerwitdaw(uint64_t tableId, asset withdraw)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(existing->dealer);
    eosio_assert((existing->dealerBalance - withdraw) > existing->minTableDeposit, "Table dealerBalance is not enough to support next round!");

    INLINE_ACTION_SENDER(eosio::token, transfer)
    (
        existing->amountSymbol.get_contract(), {{_self, "active"_n}},
        {_self, existing->dealer, withdraw, std::string("withdraw")});
    tableround.modify(existing, _self, [&](auto &s) {
        s.dealerBalance -= withdraw;
    });
}
ACTION mallard::shuffle(uint64_t tableId)
{
    auto existing = tableround.find(tableId);
    eosio_assert(existing != tableround.end(), notableerr);
    require_auth(existing->dealer);
    eosio_assert(existing->tableStatus == (uint64_t)table_stats::status_fields::ROUND_SHUFFLE, "The round isn't shuffle, can't shuffle!");

    std::vector<uint16_t> cardVec_temp = existing->validCardVec;
    shuffleFun(tableId, cardVec_temp);

    tableround.modify(existing, _self, [&](auto &s) {
        s.tableStatus = (uint64_t)table_stats::status_fields::ROUND_END;
        s.validCardVec = cardVec_temp;
        s.cardBoot += 1;
    });
}
ACTION mallard::pushaliasnam(string alias, name account)
{
    auto existing = tablealias.find(account.value);
    eosio_assert(existing == tablealias.end(), "account exist...");
    require_auth(account);

    tablealias.emplace(_self, [&](auto &s) {
        s.alias = alias;
        s.account = account;
    });
}
EOSIO_DISPATCH(mallard, (newtable)(dealerseed)(serverseed)(endbet)(playerbet)(verdealeseed)(verserveseed)(trusteeship)(exitruteship)(disconnecthi)(erasingdata)(pausetabledea)(pausetablesee)(continuetable)(closetable)(depositable)(dealerwitdaw)(shuffle)(edittable)(pushaliasnam))